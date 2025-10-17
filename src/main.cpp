#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "lcd_display.h"
#include <WiFi.h>
#include <time.h>

// Create the I2C LCD object (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
LCDDisplay lcdDisplay(lcd);

// Create the AsyncWebServer object on port 80
AsyncWebServer server(80);

const char *ssid = "ELECTRO";
const char *password = "electroelectro123";

const char *PARAM_INPUT_1 = "input1";
const char *PARAM_INPUT_2 = "input2";

// HTML web page to handle 2 input fields (input1, input2)
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

// Set timezone (seconds offset from UTC and DST offset)
const long gmtOffset_sec = 1 * 3600; // e.g. -5*3600 for UTC-5
const int daylightOffset_sec = 3600; // 3600 if DST applies

// Day-specific messages (Sunday=0 .. Saturday=6)
const char *dayMessages[7] = {
    "Closed today  ",
    "10:00-16:00   ",
    "10:00-16:00   ",
    "10:00-16:00   ",
    "10:00-16:00   ",
    "Closed today  ",
    "Closed today  "};

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

  // init LCD
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  // configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
  // wait for time to be set
  Serial.print("Waiting for time");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam(PARAM_INPUT_1) && request->getParam(PARAM_INPUT_1)->value() != "") {
        input1Message = request->getParam(PARAM_INPUT_1)->value();
    }
    if (request->hasParam(PARAM_INPUT_2) && request->getParam(PARAM_INPUT_2)->value() != "") {
        input2Message = request->getParam(PARAM_INPUT_2)->value();
    }

    // Show both messages on LCD
    delay(500);
    lcd.setCursor(0, 0); // First line
    lcd.print(input1Message);
    delay(100);

    lcd.setCursor(0, 1); // Second line
    lcd.print(input2Message);
    delay(100);

    request->send(200, "text/html", "HTTP GET request sent to your ESP<br><a href=\"/\">Return to Home Page</a>"); });
  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
  // current time for day lookup
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  // --- sources with requested priorities ---
  String src1 = (input1Message.length() > 0) ? input1Message : String("Office Hour:");
  String src2 = (input2Message.length() > 0) ? input2Message : String(dayMessages[timeinfo->tm_wday]);

  // Scrolling params (shared)
  const unsigned long holdBeforeScroll = 5000; // ms to hold start view
  const unsigned long scrollInterval = 600;    // ms between scroll steps
  const int dispWidth = 16;

  // Per-line persistent state
  static String cur1 = "", cur2 = "";
  static unsigned long lastChange1 = 0, lastChange2 = 0;
  static unsigned long lastScroll1 = 0, lastScroll2 = 0;
  static int pos1 = 0, pos2 = 0;
  static String shown1 = "", shown2 = "";

  unsigned long m = millis();

  // Reset state when source changes
  if (src1 != cur1)
  {
    cur1 = src1;
    pos1 = 0;
    lastChange1 = m;
    lastScroll1 = m;
  }
  if (src2 != cur2)
  {
    cur2 = src2;
    pos2 = 0;
    lastChange2 = m;
    lastScroll2 = m;
  }

  // Helper to compute visible window for a source string and line-specific state
  auto visibleWindow = [&](const String &cur, unsigned long lastChange, unsigned long &lastScroll, int &pos) -> String
  {
    if (cur.length() <= dispWidth)
    {
      String out = cur;
      while (out.length() < dispWidth)
        out += ' ';
      return out;
    }
    // build scrollable text with trailing gap equal to display width
    String gap;
    for (int i = 0; i < dispWidth; ++i)
      gap += ' ';
    String scrollable = cur + gap;
    // determine window start
    if (m - lastChange < holdBeforeScroll)
    {
      pos = 0;
    }
    else if (m - lastScroll >= scrollInterval)
    {
      lastScroll = m;
      pos++;
      if (pos > (int)scrollable.length() - dispWidth)
        pos = 0;
    }
    return scrollable.substring(pos, pos + dispWidth);
  };

  // Compute windows
  String win1 = visibleWindow(cur1, lastChange1, lastScroll1, pos1);
  String win2 = visibleWindow(cur2, lastChange2, lastScroll2, pos2);

  // Update LCD only when visible content changed (reduces flicker / I2C traffic)
  if (win1 != shown1)
  {
    shown1 = win1;
    lcd.setCursor(0, 0);
    lcd.print(win1);
  }
  if (win2 != shown2)
  {
    shown2 = win2;
    lcd.setCursor(0, 1);
    lcd.print(win2);
  }
}