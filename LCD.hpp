#ifndef LCD_H
#define LCD_H 1

#include "Nunchuck.hpp"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Arduino.h> // used for Serial

// everything related to talking with the LCD
namespace LCD {

enum DisplayMode {
    BUTTONS_ANALOGSTICK,
    ACCELEROMETER
};

const int buttonPin = 2;
const int adress = 0x27;

// initialize everything
void init();

// pass nunchuck data
void setData(Nunchuck::Data newData);

// refresh screen contents
void refresh();
}

#endif // LCD_H