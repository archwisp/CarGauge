// vim: ts=4:sw=4:et
#include <Arduino.h>

// Pin configuration for ESP32 Thing

const int TFT_RST = 22;
const int TFT_DC = 21;
const int SD_CS = 4;
const int TFT_CS = 2;
const int LED_BOARD = 5;
const int BOARD_BUTTON = 0; // the number of the pushbutton pin

// Already defined in .platformio/packages/framework-arduinoespressif32/variants/esp32thing/pins_arduino.h:28:22:
// const int MOSI = 23;
// const int SCK = 18;
// const int MISO = 19;

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <SPI.h>
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const int OILTEMP_DIN = 36;
#include "OilTempSensor.cpp"
OilTempSensor oilTempSensor = OilTempSensor(OILTEMP_DIN, 3.3, 4095.0, 0.2);

// NOTE: On the ESP32, UART1 is mapped to the flash memory pins by default
// and there are known stability problems when remapping UART pins. So
// we'll use UART2 by passing the UART number to the HarwareSerial
// constructor.
// 
// The ODB2UART library uses Serial1 or Serial2 by default but we can't change
// the mapping of those because they are hardcoded. Instead, we'll redefine
// OBDUART to Uart2

HardwareSerial Uart2(2);
#define OBDUART Uart2

// .pio/libdeps/esp32thing/ArduinoOBD/libraries/OBD2UART/OBD2UART.h:13-23
//
// #ifndef OBDUART
// #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168P__)
// #define OBDUART Serial
// #else
// #define OBDUART Seria1
// #endif
// #endif
// 
// #ifdef ESP32
// extern HardwareSerial Serial1;
// #endif

// ESP 32 Thing:
//   U2_RXD: pin 16
//   U2_TXD: pin 17
// Freematics OBD-II UART:
//   Green: TX - Connect to RX, pin 16
//   White: RX - Connect to TX, pin 17

#include <OBD2UART.cpp>

COBD obd;
int obdAvailable = 0;

void setup()
{
    Serial.begin(9600); 
    delay(500);

    // Initialize TFT display
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setRotation(-45); 

    // Set ADC parameters
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    drawScreen();

    // Don't use the odb.begin() function because the OBDUART.end() call in
    // it crashes the ESP32.
    OBDUART.begin(115200, SERIAL_8N1, 16, 17);
    byte version = obd.getVersion(); 

    if (version > 0) {
        obd.init();
        obdAvailable = 1;
    }
} 

void drawScreen()
{
    tft.setFont(&FreeSans9pt7b);
    tft.setCursor(115, 40);
    tft.println("Oil");
    tft.setCursor(102, 105);
    tft.println("Water");
}

int cToF(int value) {
    return value * 9 / 5 + 32;
}

void updateOilTemp(float temp) {
    if (temp < 0 || temp > 350) {
        return;
    }

    uint16_t bgcolor = ST7735_BLACK;

    if (temp < 180) {
        bgcolor = ST7735_BLUE;
    } else if (temp > 220) {
        bgcolor = ST7735_RED;
    }

    tft.fillRect(0, 0, 90, 60, bgcolor);
    tft.setFont(&FreeSans24pt7b);
    tft.setCursor(3, 45);
    tft.println((int)temp);
}

void updateWaterTemp(float temp) {
    if (temp < -1 || temp > 350) {
        return;
    }

    uint16_t bgcolor = ST7735_BLACK;

    if (temp < 180) {
        bgcolor = ST7735_BLUE;
    } else if (temp > 220) {
        bgcolor = ST7735_RED;
    }

    tft.fillRect(0, 65, 90, 60, bgcolor);
    tft.setFont(&FreeSans24pt7b);
    tft.setCursor(3, 110);

    if (obdAvailable) {
        tft.println((int)temp);
    } else {
        tft.println("N/A");
    }
}

int lastOilTemp = 0;
int lastWaterTemp = 0;
int oilTemp = 0;
int waterTemp = 0;

void loop(){
    oilTemp = cToF(oilTempSensor.getTemp());

    Serial.print("Oil Temp: ");
    Serial.println(oilTemp);

    if (oilTemp != lastOilTemp) {
        updateOilTemp(oilTemp);
        lastOilTemp = oilTemp;
    }

    waterTemp = 0;

    if (obdAvailable) {
        obd.readPID(PID_COOLANT_TEMP, waterTemp);
        waterTemp = cToF(waterTemp);
    } else {
        waterTemp = -1;
    } 

    Serial.print("Water Temp: ");
    Serial.println(waterTemp);

    if (waterTemp != lastWaterTemp) {
        updateWaterTemp(waterTemp);
        lastWaterTemp = waterTemp;
    }

    delay(1000);
}
