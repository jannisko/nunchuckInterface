#include "LCD.hpp"
// main documentation in LCD.hpp
namespace LCD {

// anonymous namespace -> everything inside LCD can see the contents, everything outside cannot
namespace {
    // object for controlling lcd screen
    LiquidCrystal_I2C lcd(adress, 16, 2);

    Nunchuck::Data data; // currently displayed data
    DisplayMode displayMode; // current display mode
    bool buttonPressed = false; // set to true by interrupt, reset to false by refresh

    // switch between the to views
    void setDisplayMode(DisplayMode mode)
    {
        if (displayMode != mode) {
            // clear screen on display mode change
            lcd.clear();
        }

        displayMode = mode;
    }

    unsigned long lastDebounceTime = 0;
    void buttonISR() // executed on button pin change
    {
        unsigned long time = millis(); // get current time
        // if last rise was more than 200ms ago
        if ((time - lastDebounceTime) > 200) {
            buttonPressed = true; // set flag that btn was pressed
            lastDebounceTime = time; // save the time of the rise
        }
    }

    // init button, setup interrupt
    void setupButton()
    {
        pinMode(buttonPin, INPUT);
        attachInterrupt(
            digitalPinToInterrupt(buttonPin),
            buttonISR,
            RISING);
    }

    // print some whitespace so left over characters get replaced
    void clearRestOfLine()
    {
        lcd.print("      ");
    }

    // decide between the categorys of joy stick values
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

    // print joy stick postion
    void printJoystick()
    {
        lcd.print("Joy: ");
        valToDirection_joy(data.joyy, 'N', 'S');
        valToDirection_joy(data.joyx, 'E', 'W');

        clearRestOfLine();
    }

    // print pressed buttons
    void printButtons()
    {
        lcd.print("Buttons: ");
        if (data.cbut) {
            // if button pressed print button (duh)
            lcd.print("C");

            if (data.zbut) // comma between the buttons if necessary
                lcd.print(", ");
        }

        if (data.zbut)
            lcd.print("Z");

        clearRestOfLine();
    }

    // decide between the categorys of accelerometer data
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

// initialize lcd module
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

    // if a button press is detected by the interrupt, buttonPressed will be true
    if (buttonPressed) {
        // so we have to change the current display mode to the other one
        displayMode = displayMode == DisplayMode::ACCELEROMETER
            ? DisplayMode::BUTTONS_ANALOGSTICK
            : DisplayMode::ACCELEROMETER;

        // we did what we had to do on button press, so we have to reset this to false
        buttonPressed = false;
    }

    // print the data according to display mode
    if (displayMode == DisplayMode::ACCELEROMETER) {
        printAccelerometer();
    } else {
        printJoystick();
        lcd.setCursor(0, 1);
        printButtons();
    }
}
}