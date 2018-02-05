#include "RtcClock.h"


void TimeUpdateHandler(TimerHandle_t xTimer);

void (*RtcClock::timeUpdateCallback)(void);


RtcClock::RtcClock(int _timeZone) : lastSyncTick(1000000UL), synhronized(false)
{
    timeZone = _timeZone;
}


void RtcClock::Process()
{
    if ((xTaskGetTickCount() - lastSyncTick) / configTICK_RATE_HZ > TIME_SYNC_PERIOD)
    {
        synchronize();
    }
}


bool RtcClock::GetCurrentTime(time_t * time)
{
    if (!synhronized)
    {
        return false;
    }

    *time = lastSyncTime + (xTaskGetTickCount() - lastSyncTick) / configTICK_RATE_HZ;
    *time += timeZone * 3600;

    return true;
}


void RtcClock::begin()
{
    udp.begin(LOCAL_PORT);

    clock_timer = xTimerCreate("TimeUpdateTimer", configTICK_RATE_HZ, pdTRUE, NULL, TimeUpdateHandler);
}


uint32_t RtcClock::getNextTimeUpdate()
{
    if (!synhronized)
    {
        return 0;
    }

    return configTICK_RATE_HZ - ((xTaskGetTickCount() - lastSyncTick) % configTICK_RATE_HZ);
}


void RtcClock::synchronize()
{
    static IPAddress timeServerIP;
    //get a random server from the pool
    WiFi.hostByName(NTP_SERVER_NAME, timeServerIP);

    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
                                 // wait to see if a reply is available
    for (int i = 0; i < 300; ++i)
    {
        if (udp.available())
        {
            break;
        }
        vTaskDelay(10);
    }

    TickType_t currTick = xTaskGetTickCount();

    int cb = udp.parsePacket();
    if (!cb)
    {
        // try again laiter
        lastSyncTick = currTick - (TIME_SYNC_PERIOD - RETRY_SYNC_DELAY) * configTICK_RATE_HZ;
    }
    else
    {
        xTimerStart(clock_timer, 0);
        // We've received a packet, read the data from it
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;

        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        // subtract seventy years:
        lastSyncTime = secsSince1900 - SEVENTY_YEARS_IN_SECONDS;
        lastSyncTick = currTick;
        synhronized = true;
    }
}


void RtcClock::sendNTPpacket(IPAddress & address)
{
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
                             // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}


void TimeUpdateHandler(TimerHandle_t xTimer)
{
    if (RtcClock::timeUpdateCallback != 0)
    {
        RtcClock::timeUpdateCallback();
    }
}
