#include "LCD.hpp"
#include "Nunchuck.hpp"
#include <Arduino.h>

void print_data_text(Nunchuck::Data data)
{
    Serial.print("Z=");
    Serial.print(data.zbut);
    Serial.print("\tC=");
    Serial.print(data.cbut);
    Serial.print("\tJoyX=");
    Serial.print(data.joyx);
    Serial.print("\tJoyY=");
    Serial.print(data.joyy);
    Serial.print("\tAccelX=");
    Serial.print(data.accx);
    Serial.print("\tAccelY=");
    Serial.print(data.accy);
    Serial.print("\tAccelZ=");
    Serial.println(data.accz);
}

void setup()
{
    // init serial
    Serial.begin(115200);

    // init nunchuck
    Nunchuck::handshake();
    Serial.println("Wii Nunchuck Ready");

    // init LCD
    LCD::init();
    Serial.println("LCD Ready");
}

int loop_cnt = 0;
void loop()
{
    /*
     * We want to get new data and display it every 100ms.
     * Originally we had a delay(100), but somehow broke it.
     * This way works.
     */
    if (loop_cnt % 10 == 0) { // 10*10 ms => every 100ms
        // get data from nunchuck
        Nunchuck::Data newData = Nunchuck::getNewData();

        // pass that data to the lcd
        LCD::setData(newData);
        // actually refresh the screen
        LCD::refresh();

        // slow debug output to serial (100*10 ms => every second)
        if (loop_cnt % 100 == 0) {
            print_data_text(newData);
        }
    }

    loop_cnt++;
    delay(10);
}