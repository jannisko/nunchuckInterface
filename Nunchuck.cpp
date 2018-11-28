#include "Nunchuck.hpp"

// everything related to talking with the nunchuck
namespace Nunchuck {

// make those things invisible to outside of the namespace
namespace {
    // array to store nunchuck data
    uint8_t buf[6];
    uint8_t address = 0x52;

    // sends a data request to WII Nunchuck
    void send_request()
    {
        // https://bootlin.com/labs/doc/nunchuk.pdf
        Wire.beginTransmission(0x52);
        Wire.write((uint8_t)0x00);
        Wire.endTransmission();
    }

    char decode_byte(char x)
    {
        return (x ^ 0x17) + 0x17; // data XOR with 10111 (magic)
    }

    // Receive data from nunchuck
    // returns 1 if success, 0 if failure
    int get_data()
    {
        int cnt = 0;
        Wire.requestFrom(0x52, 6); // request 6 bytes
        while (Wire.available()) {
            buf[cnt] = decode_byte(Wire.read());
            cnt++;
        }

        send_request(); // send request for next get_data
        // success if 6 bytes received
        if (cnt >= 5) {
            return 1;
        }
        return 0;
    }

    // returns zbutton state 0/1
    int zbutton()
    {
        return ((buf[5] >> 0) & 0x01) ? 0 : 1; // checks the first bit and returns it
    }

    // returns cbutton state 0/1
    int cbutton()
    {
        return ((buf[5] >> 1) & 0x01) ? 0 : 1; // checks the second bit and returns it
    }

    // returns value of x-axis joystick
    int joyx()
    {
        return buf[0];
    }

    // returns value of y-axis joystick
    int joyy()
    {
        return buf[1];
    }

    // returns value of x-axis accelerometer
    int accelx()
    {
        // bit 9-2 are at buf[2], bit 1-0 are at buf[5]
        // https://bootlin.com/labs/doc/nunchuk.pdf
        return (buf[2] << 2) | ((buf[5] >> 2) & 0x03);
    }

    // returns value of y-axis accelerometer
    int accely()
    {
        return (buf[3] << 2) | ((buf[5] >> 4) & 0x03);
    }

    // returns value of z-axis accelerometer
    int accelz()
    {
        return (buf[4] << 2) | ((buf[5] >> 6) & 0x03);
    }
}

// sends handshake signal to WII Nunchuck
void handshake()
{
    Serial.print("Nunchuck: sending handshake...");

    // https://bootlin.com/labs/doc/nunchuk.pdf
    Wire.begin();
    Wire.beginTransmission(0x52);
    Wire.write((uint8_t)0x40);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();

    Serial.println(" Sent!");
}

Data getNewData()
{
    get_data();

    Data result;
    result.zbut = zbutton(); //  0 - 1
    result.cbut = cbutton(); //  0 - 1
    result.joyx = joyx(); //  0 - 255
    result.joyy = joyy(); //  0 - 255
    result.accx = accelx(); //  0 - 1024
    result.accy = accely(); //  0 - 1024
    result.accz = accelz(); //  0 - 1024

    return result;
}
}
