#ifndef LCD_H
#define LCD_H 1

#include "Nunchuck.hpp" // for Nunchuck::Data
#include <Arduino.h> // used for Serial
#include <LiquidCrystal_I2C.h> // library for communicating with LCD screen
#include <Wire.h> // arduino I2C
#include <avr/interrupt.h> // interrupt service routine

// everything related to talking with the LCD.
// Wrapper around external library that can easily display the nunchuck data
namespace LCD {

// enum instead of bool so it can be extended later (maybe gesture screen?)
enum DisplayMode {
    BUTTONS_ANALOGSTICK,
    ACCELEROMETER
};

const int buttonPin = 2; // the button used for changing the display mode
const int adress = 0x27; // I2C adress of the screen

// initialize everything
void init();

// pass nunchuck data
void setData(Nunchuck::Data newData);

// refresh screen contents
void refresh();
}

#endif // LCD_H