#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_display.h"

LCDDisplay::LCDDisplay(LiquidCrystal_I2C &lcd) : lcd(lcd) {}

void LCDDisplay::init()
{
    lcd.init(); // Use init() instead of begin()
    lcd.backlight();
}

void LCDDisplay::updateText(const String &text)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text);
}
