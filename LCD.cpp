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
    int lastButtonState, buttonState;
    int debounceDelay = 200;
    void debounce_onButtonPress()
    {
        int reading = digitalRead(buttonPin);
        if (reading != lastButtonState) {
            // reset the debouncing timer
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:

            // if the button state has changed:
            if (reading != buttonState) {
                buttonState = reading;

                // only toggle the LED if the new button state is HIGH
                if (buttonState == HIGH) {
                    buttonPressed = true;
                }
            }
        }

        // save the reading. Next time through the loop, it'll be the lastButtonState:
        lastButtonState = reading;
    }

    void setupButton()
    {
        pinMode(buttonPin, INPUT);
        attachInterrupt(
            digitalPinToInterrupt(buttonPin),
            debounce_onButtonPress,
            FALLING);
    }

    void valToDirection_int(unsigned int valToPrint, int extreme, char bigChar, char smallChar)
    {
        int val = valToPrint - extreme / 2;
        if (val > 0) {
            lcd.print(bigChar);
            if (val > extreme / 4)
                lcd.print(bigChar);
            return;
        }

        if (val == 0) {
            lcd.print("0");
            return;
        }

        // val < 0
        lcd.print(smallChar);
        if (val < -(extreme / 4))
            lcd.print(smallChar);
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

    void valToDirection_uint16(uint16_t valToPrint, char bigChar, char smallChar)
    {
        valToDirection_int(valToPrint, 1024, bigChar, smallChar);
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
        lcd.print("Accel-XY: ");
        valToDirection_uint16(data.accy, 'N', 'S');
        valToDirection_uint16(data.accx, 'E', 'W');

        lcd.setCursor(0, 1);
        lcd.print("Accel-Z: ");
        valToDirection_uint16(data.accz, '+', '-');
    }

    bool dataIsValid(){
        // 255 cannot be done using the real joystick, but it can be used to check if the data is valid
        return data.joyx != 255 && data.joyy != 255;
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

        /* Possible race condition with button. But it should be very unlikely as the */
        buttonPressed = false;
    }

    if (!dataIsValid()) {
        lcd.clear();
        lcd.print("Nunchuck");
        lcd.setCursor(0,1);
        lcd.print("disconnected");
        Nunchuck::handshake();
        return;
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