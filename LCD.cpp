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

    void clearRestOfLine()
    {
        lcd.print("      ");
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

        clearRestOfLine();
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

        clearRestOfLine();
    }

    void valToDirection_accel(uint16_t val, char* bigText, char* middleText, char* smallText)
    {
        if (val < 450) {
            lcd.print(smallText);
        } else if (val < 650) {
            lcd.print(middleText);
        } else {
            lcd.print(bigText);
        }
    }

    // print accelerometer info
    void printAccelerometer()
    {
        // accx
        lcd.print("Accel-X=");
        valToDirection_accel(data.accx, "Right", "Middle", "Left");

        clearRestOfLine();
        lcd.setCursor(0, 1);

        // accy
        lcd.print("Y=");
        valToDirection_accel(data.accy, "Back", "Top", "Front");

        // accz
        lcd.print(" Z=");
        valToDirection_accel(data.accz, "Top", "Side", "Bottom");

        clearRestOfLine();
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