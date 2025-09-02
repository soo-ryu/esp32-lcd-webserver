#include <LiquidCrystal_I2C.h>
#pragma once

class LCDDisplay
{
public:
    LCDDisplay(LiquidCrystal_I2C &lcd);
    void init();
    void updateText(const String &text);

private:
    LiquidCrystal_I2C lcd;
};