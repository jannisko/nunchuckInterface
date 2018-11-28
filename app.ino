#include "LCD.hpp"
#include "Nunchuck.hpp"
#include <Arduino.h>

void print_data_text(Nunchuck::Data data)
{
    Serial.print("Z Button:  ");
    Serial.print(data.zbut);
    Serial.print("\tC Button:  ");
    Serial.print(data.cbut);
    Serial.print("\tX Joy:  ");
    Serial.print(data.joyx);
    Serial.print("\tY Joy:  ");
    Serial.print(data.joyy);
    Serial.print("\tX Accel:  ");
    Serial.print(data.accx);
    Serial.print("\tY Accel:  ");
    Serial.print(data.accy);
    Serial.println("\tZ Accel:  ");
    Serial.print(data.accz);
    Serial.print("\n");
}

void setup()
{
    // init serial
    Serial.begin(115200);

    // init nunchuck
    Nunchuck::handshake();
    Serial.println("Wii Nunchuck Ready");

// Receive data from nunchuck
// returns 1 if success, 0 if failure
static int nunchuck_get_data()
}

int loop_cnt = 0;
void loop()
{
    if (loop_cnt % 10 == 0) { // every 100 msecs get new data
        Nunchuck::Data newData = Nunchuck::getNewData();

// returns value of x-axis accelerometer
static int nunchuck_accelx()

        if (loop_cnt % 100 == 0) {
            print_data_text(newData);
        }
    }

    loop_cnt++;
    delay(10);
}