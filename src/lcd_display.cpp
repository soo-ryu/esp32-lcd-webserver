#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_display.h"

LCDDisplay::LCDDisplay(LiquidCrystal_I2C &lcd) : lcd(lcd) {}

void LCDDisplay::init()
{
    lcd.begin(16, 2); // Initialize LCD with 16 columns and 2 rows
    lcd.backlight();
}

void LCDDisplay::updateText(const String &text)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text);
}
