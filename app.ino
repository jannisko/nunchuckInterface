#include <Wire.h>

#include <Arduino.h>

static uint8_t nunchuck_buf[6];   // array to store nunchuck data

// sends handshake signal to WII Nunchuck
static void nunchuck_init()
{
  //https://bootlin.com/labs/doc/nunchuk.pdf
  Wire.begin();
  Wire.beginTransmission(0x52);
  Wire.write((uint8_t)0x40);
  Wire.write((uint8_t)0x00);
  Wire.endTransmission();
}

// sends a data request to WII Nunchuck
static void nunchuck_send_request()
{
  //https://bootlin.com/labs/doc/nunchuk.pdf
  Wire.beginTransmission(0x52);
  Wire.write((uint8_t)0x00);
  Wire.endTransmission();
}

static char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17; // data XOR with 10111 (magic)
  return x;
}

// Receive data from nunchuck
// returns 1 if success, 0 if failure
static int nunchuck_get_data()
{
  int cnt = 0;
  Wire.requestFrom (0x52, 6); // request 6 bytes
  while (Wire.available ()) {
    nunchuck_buf[cnt] = nunchuk_decode_byte( Wire.read() );
    cnt++;
  }

  nunchuck_send_request();  // send request for next nunchuck_get_data
  // success if 6 bytes received
  if (cnt >= 5) {
    return 1;
  }
  return 0;
}

// returns zbutton state 0/1
static int nunchuck_zbutton()
{
  return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;  // checks the first bit and returns it
}

// returns cbutton state 0/1
static int nunchuck_cbutton()
{
  return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;  // checks the second bit and returns it
}

// returns value of x-axis joystick
static int nunchuck_joyx()
{
  return nunchuck_buf[0];
}

// returns value of y-axis joystick
static int nunchuck_joyy()
{
  return nunchuck_buf[1];
}

// returns value of x-axis accelerometer
static int nunchuck_accelx()
{
  // bit 9-2 are at buf[2], bit 1-0 are at buf[5]
  // https://bootlin.com/labs/doc/nunchuk.pdf
  return (nunchuck_buf[2] << 2) | ((nunchuck_buf[5] >> 2) & 0x03);   
}

// returns value of y-axis accelerometer
static int nunchuck_accely()
{
  return (nunchuck_buf[3] << 2) | ((nunchuck_buf[5] >> 4) & 0x03);
}

// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
  return (nunchuck_buf[4] << 2) | ((nunchuck_buf[5] >> 6) & 0x03);
}

int loop_cnt = 0;

byte joyx, joyy, zbut, cbut, accx, accy, accz;

void print_data() {
  Serial.print("Z Button:  ");
  Serial.print(zbut);
  Serial.print("\tC Button:  ");
  Serial.print(cbut);
  Serial.print("\tX Joy:  ");
  Serial.print(map(joyx, 15, 221, 0, 255));
  Serial.print("\tY Joy:  ");
  Serial.print(map(joyy, 29, 229, 0, 255));
  Serial.print("\tX Accel:  ");
  Serial.print(accx);
  Serial.print("\tY Accel:  ");
  Serial.print(accy);
  Serial.println("\tZ Accel:  ");
  Serial.print(accz);
  Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  nunchuck_init(); // send the initilization handshake
  Serial.println("Wii Nunchuck Ready");
}

void loop() {
  if ( loop_cnt > 10 ) { // every 100 msecs get new data
    loop_cnt = 0;

    nunchuck_get_data();

    zbut = nunchuck_zbutton();  //  0 - 1
    cbut = nunchuck_cbutton();  //  0 - 1
    joyx = nunchuck_joyx();     //  15 - 221
    joyy = nunchuck_joyy();     //  29 - 229
    accx = nunchuck_accelx();   //  70 - 182
    accy = nunchuck_accely();   //  65 - 173
    accz = nunchuck_accelz();   //  0 - 255

    print_data();
  }
  loop_cnt++;
  delay(100);

}