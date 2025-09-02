#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include "lcd_display.h"
#include <WiFi.h>

// Create the I2C LCD object (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
LCDDisplay lcdDisplay(lcd);

// Create the AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup()
{
    WiFi.begin("ELECTRO", "electroelectro123");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    lcdDisplay.init();

    server.on("/setText", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("text", true)) {
            String newText = request->getParam("text", true)->value();
            lcdDisplay.updateText(newText);
            request->send(200, "text/plain", "Text updated");
        } else {
            request->send(400, "text/plain", "Bad Request");
        } });

    server.begin();
}

void loop()
{
    // Nothing needed here for AsyncWebServer
}