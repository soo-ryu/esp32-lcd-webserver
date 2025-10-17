#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "lcd_display.h"
#include <WiFi.h>

// Create the I2C LCD object (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
LCDDisplay lcdDisplay(lcd);

// Create the AsyncWebServer object on port 80
AsyncWebServer server(80);

const char *ssid = "ELECTRO";
const char *password = "electroelectro123";

const char *PARAM_INPUT_1 = "input1";
const char *PARAM_INPUT_2 = "input2";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    First line: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Second line: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String input1Message = "";
String input2Message = "";

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam(PARAM_INPUT_1) && request->getParam(PARAM_INPUT_1)->value() != "") {
        input1Message = request->getParam(PARAM_INPUT_1)->value();
    //     delay(500);
    // lcd.setCursor(0, 0); // First line
    // lcd.print(input1Message);
    // delay(100);
    }
    if (request->hasParam(PARAM_INPUT_2) && request->getParam(PARAM_INPUT_2)->value() != "") {
        input2Message = request->getParam(PARAM_INPUT_2)->value();
        lcd.setCursor(0, 1); // Second line
    lcd.print(input2Message);
    delay(100);
    }

    // Show both messages on LCD
    // delay(500);
    // lcd.setCursor(0, 0); // First line
    // lcd.print(input1Message);
    // delay(100);

    // lcd.setCursor(0, 1); // Second line
    // lcd.print(input2Message);
    // delay(100);

    request->send(200, "text/html", "HTTP GET request sent to your ESP<br><a href=\"/\">Return to Home Page</a>"); });
  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
}