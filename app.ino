#include <Wire.h>

#include <Arduino.h>

static uint8_t nunchuck_buf[6];   // array to store nunchuck data

// sends handshake signal to WII Nunchuck
static void nunchuck_init()
{
  Wire.begin();
  Wire.beginTransmission(0x52);// transmit 0x52
  Wire.write((uint8_t)0x40);// sends memory address
  Wire.write((uint8_t)0x00);// sends sent a zero
  Wire.endTransmission();// stop transmitting
}

// Send a request for data to the nunchuck
static void nunchuck_send_request()
{
  Wire.beginTransmission(0x52);// transmit to device 0x52
  Wire.write((uint8_t)0x00);// sends one byte
  Wire.endTransmission();// stop transmitting
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
static char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

// Receive data back from the nunchuck,
// returns 1 on successful read. returns 0 on failure
static int nunchuck_get_data()
{
  int cnt = 0;
  Wire.requestFrom (0x52, 6);// request data from nunchuck
  while (Wire.available ()) {
    // receive byte as an integer
    nunchuck_buf[cnt] = nunchuk_decode_byte( Wire.read() );
    cnt++;
  }
  nunchuck_send_request();  // send request for next data payload
  // If we recieved the 6 bytes, then go print them
  if (cnt >= 5) {
    return 1;   // success
  }
  return 0; //failure
}



// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_zbutton()
{
  return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;  // voodoo
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_cbutton()
{
  return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;  // voodoo
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
  return nunchuck_buf[2];   // FIXME: this leaves out 2-bits of the data
}

// returns value of y-axis accelerometer
static int nunchuck_accely()
{
  return nunchuck_buf[3];   // FIXME: this leaves out 2-bits of the data
}

// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
  return nunchuck_buf[4];   // FIXME: this leaves out 2-bits of the data
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