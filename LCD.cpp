#include "LCD.hpp"

namespace LCD {

namespace {
    // object for controlling lcd screen
    LiquidCrystal_I2C lcd(adress, 16, 2);

    Nunchuck::Data data;
    DisplayMode displayMode;
    bool buttonPressed = false; // set to true by interrupt, reset to false by refresh

    // switch between the to views
    void setDisplayMode(DisplayMode mode)
    {
        displayMode = mode;
        lcd.clear();
    }

    unsigned long lastDebounceTime = 0;
    void debounce_onButtonPress()
    {
        unsigned long time = millis();
        if ((time - lastDebounceTime) > 200 && digitalRead(buttonPin) == HIGH) {
            buttonPressed = true;
            lastDebounceTime = time;
        }
    }

    void setupButton()
    {
        pinMode(buttonPin, INPUT);
        attachInterrupt(
            digitalPinToInterrupt(buttonPin),
            debounce_onButtonPress,
            RISING);
    }

    void valToDirection_accel(uint16_t val, char bigChar, char smallChar)
    {
        if (val < 120)
            lcd.print(smallChar);
        if (val < 50)
            lcd.print(smallChar);

        if (val > 130)
            lcd.print(bigChar);
        if (val > 205)
            lcd.print(bigChar);
    }

    void valToDirection_joy(uint8_t val, char bigChar, char smallChar)
    {
        if (val < 120)
            lcd.print(smallChar);
        if (val < 50)
            lcd.print(smallChar);

        if (val > 130)
            lcd.print(bigChar);
        if (val > 205)
            lcd.print(bigChar);
    }

    void printJoystick()
    {
        // print joy stick postion
        lcd.print("Joy: ");
        valToDirection_joy(data.joyy, 'N', 'S');
        valToDirection_joy(data.joyx, 'E', 'W');

        // clear rest of line
        lcd.print("      ");
    }

    void printButtons()
    {
        lcd.print("Buttons: ");
        if (data.cbut) {
            lcd.print("C");

            if (data.zbut)
                lcd.print(", ");
        }

        if (data.zbut)
            lcd.print("Z");

        // clear rest of line
        lcd.print("       ");
    }

    // print accelerometer info
    void printAccelerometer()
    {
        // accx
        lcd.print("Accel-X: ");
        if (data.accx < 400) {
            lcd.print("Left");
        } else if (data.accx < 600) {
            lcd.print("Middle");
        } else {
            lcd.print("Right");
        }

        // clear rest of line
        lcd.print("      ");

        // accy
        lcd.setCursor(0, 1);
        lcd.print("Y: ");
        if (data.accy < 450){
            lcd.print("Frnt");
        } else if (data.accy < 650) {
            lcd.print("Top");
        } else {
            lcd.print("Back");
        }

        // accz
        lcd.print("  Z: ");
        if (data.accz < 450){
            lcd.print("Bot");
        } else if (data.accz < 650) {
            lcd.print("Side");
        } else {
            lcd.print("Top");
        }

        // clear rest of line
        lcd.print("   ");
    }
}

void init()
{
    // initialize the LCD
    Serial.print("LCD: initializing screen... ");
    lcd.begin();
    setDisplayMode(DisplayMode::BUTTONS_ANALOGSTICK);
    Serial.println("Done!");

    // initialize button interrupt
    Serial.print("LCD: initializing button interrupt... ");
    setupButton();
    Serial.println("Done!");
}

void setData(Nunchuck::Data newData)
{
    data = newData;
}

void refresh()
{
    // reposition cursor
    lcd.setCursor(0, 0);

    if (buttonPressed) {
        displayMode = displayMode == DisplayMode::ACCELEROMETER
            ? DisplayMode::BUTTONS_ANALOGSTICK
            : DisplayMode::ACCELEROMETER;
        lcd.clear(); // maybe sometimes something could get left over
        /* Possible race condition with button, but it should be very unlikely  */
        buttonPressed = false;
    }


    if (displayMode == DisplayMode::ACCELEROMETER) {
        printAccelerometer();
    } else {
        printJoystick();
        lcd.setCursor(0, 1);
        printButtons();
    }
}
}