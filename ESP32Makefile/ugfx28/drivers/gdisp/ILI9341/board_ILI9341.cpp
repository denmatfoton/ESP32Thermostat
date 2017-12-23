#include <stdarg.h>
#include <Arduino.h>
#include <SPI.h>
#include "gfx.h"
#include "board_ILI9341.h"


SPISettings spiSettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
SemaphoreHandle_t spiMutex = NULL;


void init_board() {
    static bool initialized = false;

    if (initialized)
    {
        return;
    }

    initialized = true;

    pinMode(TFT_RST_PIN, OUTPUT);
    pinMode(TFT_DC_PIN, OUTPUT);
    pinMode(TFT_CS_PIN, OUTPUT);
    pinMode(TOUCH_CS_PIN, OUTPUT);
    pinMode(TOUCH_IRQ_PIN, INPUT);

    digitalWrite(TFT_RST_PIN, LOW);
    digitalWrite(TFT_CS_PIN, HIGH);
    digitalWrite(TOUCH_CS_PIN, HIGH);

    spiMutex = xSemaphoreCreateMutex();

    SPI.begin();
}

void post_init_board(GDisplay *g) {
    (void)g;
}

void setpin_reset(GDisplay *g, bool_t state) {
    (void)g;

    if (state == TRUE)
    {
        digitalWrite(TFT_RST_PIN, LOW);
    }
    else
    {
        digitalWrite(TFT_RST_PIN, HIGH);
    }
}

void set_backlight(GDisplay *g, uint8_t percent) {
    (void)g;
    (void)percent;
}


void setreadmode(GDisplay *g) {
    (void)g;
}

void setwritemode(GDisplay *g) {
    (void)g;
}


bool_t display_pressed()
{
    return (digitalRead(TOUCH_IRQ_PIN) == LOW);
}


void acquire_bus(uint8_t cs_pin) {
    xSemaphoreTake(spiMutex, (TickType_t)0xFFFFFFFFUL);
    digitalWrite(cs_pin, LOW);
    SPI.beginTransaction(spiSettings);
}

void release_bus(uint8_t cs_pin) {
    SPI.endTransaction();
    digitalWrite(cs_pin, HIGH);
    xSemaphoreGive(spiMutex);
}

void write_index(uint16_t index) {
    digitalWrite(TFT_DC_PIN, LOW);
    SPI.write(index);
    digitalWrite(TFT_DC_PIN, HIGH);
}

void write_data(uint16_t data) {
    SPI.write(data);
}

uint16_t read_data16(uint16_t port) {
    return SPI.transfer16(port);
}

uint8_t read_data(uint8_t port) {
    return SPI.transfer(port);
}


#define MAX_UART_STR_SIZE 128

void serial_printf(const char* format, ...)
{
    static char outputStr[MAX_UART_STR_SIZE];
    va_list aptr;
    int i;
    char *pos;
    uint32_t endPos;

    va_start(aptr, format);
    vsnprintf(outputStr, MAX_UART_STR_SIZE, format, aptr);
    va_end(aptr);

    Serial.print(outputStr);
}
