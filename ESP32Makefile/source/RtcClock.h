#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>


#define NTP_SERVER_NAME             "time.nist.gov"
#define NTP_PACKET_SIZE             48
#define LOCAL_PORT                  2390
#define TIME_SYNC_PERIOD            1000 // s
#define RETRY_SYNC_DELAY            10 // s
#define DAY_SECONDS                 (24 * 60 * 60)
#define SECONDS_SINCE_MIDNIGHT(t)   (t.sec + t.min * 60 + t.hour * 3600) 
#define SEVENTY_YEARS_IN_SECONDS    2208988800UL


class RtcClock
{
public:
    RtcClock(int _timeZone = 0);
    void Process();
    bool GetCurrentTime(time_t* time);
    void begin();
    void setTimeZone(int _timeZone) { timeZone = _timeZone; }
    uint32_t getNextTimeUpdate();
    void setTimeUpdateCallback(void(*callback)(void)) { timeUpdateCallback = callback; }
private:
    void synchronize();
    void sendNTPpacket(IPAddress& address);
    friend void TimeUpdateHandler(TimerHandle_t xTimer);

    static void(*timeUpdateCallback)(void);
    TimerHandle_t clock_timer;
    int timeZone;
    time_t lastSyncTime;
    TickType_t lastSyncTick;
    bool synhronized;
    uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP
};


extern RtcClock rtc_time;
