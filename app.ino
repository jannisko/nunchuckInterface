#include <Wire.h>

#include <Arduino.h>

static uint8_t nunchuck_buf[6];   // array to store nunchuck data
static boolean data_arr[8][8]; // array to feed 8x8 LED Matrix

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
  return ((nunchuck_buf[5] >> 0) & 0x01) ? 0 : 1;  // checks the first bit and returns it
}

// returns cbutton state 0/1
static int nunchuck_cbutton()
{
  return ((nunchuck_buf[5] >> 1) & 0x01) ? 0 : 1;  // checks the second bit and returns it
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

byte joyx, joyy;
bool zbut, cbut; 
// acceleration is 10 bit long
int16_t accx, accy, accz;

void print_data() {

  Serial.print("--------------------\n");

  for(int y=0;y<8;y++){
    for(int x=0;x<8;x++){
      Serial.print(data_arr[x][y] ? 1 : 0);
      Serial.print(" ");
    }
    Serial.print("\n");
  }

  // Serial.print("Z Button:  ");
  // Serial.print(zbut);
  // Serial.print("\tC Button:  ");
  // Serial.print(cbut);
  // Serial.print("\tX Joy:  ");
  // Serial.print(joyx);
  // Serial.print("\tY Joy:  ");
  // Serial.print(joyy);
  // Serial.print("\tX Accel:  ");
  // Serial.print(accx);
  // Serial.print("\tY Accel:  ");
  // Serial.print(accy);
  // Serial.println("\tZ Accel:  ");
  // Serial.print(accz);
  // Serial.print("\n");
}

void setup() {
  Serial.begin(115200);
  nunchuck_init(); // send the initilization handshake
  Serial.println("Wii Nunchuck Ready");

  // fill array with 0
  for(int x=0;x<8;x++){
    for(int y=0;y<8;y++){
      data_arr[x][y]=false;
    }
  }

}

void loop() {
  if ( loop_cnt > 10 ) { // every 100 msecs get new data
    loop_cnt = 0;

    nunchuck_get_data();

    zbut = nunchuck_zbutton();  //  0 - 1
    cbut = nunchuck_cbutton();  //  0 - 1
    joyx = nunchuck_joyx();     //  0 - 255
    joyy = nunchuck_joyy();     //  0 - 255
    accx = nunchuck_accelx();   //  0 - 1024
    accy = nunchuck_accely();   //  0 - 1024
    accz = nunchuck_accelz();   //  0 - 1024

    // map the joy values to 0-2
    // last integer of the map function begins at upper bound
    // so 3 only shows up at 1024 (which is stupid)
    joyx = map(joyx,0,255,0,3);
    joyy = map(joyy,0,255,0,3);

    // map the accel values to 0-4
    // last integer of the map function begins at upper bound
    // so 5 only shows up at 1024 (which is stupid)
    accx = map(accx,0,1024,0,5);
    accy = map(accy,0,1024,0,5);
    accz = map(accz,0,1024,0,5);

    for(int x=0; x<3; x++){
      for(int y=0; y<3; y++){
        // fill joystick array
        // joystick in y direction is inverse
        data_arr[x][y] = (joyx==x & joyy==map(y,0,2,2,0));
      }
    }

    for(int x=0; x<5; x++){
      for(int y=0; y<5; y++){
        // fill accel x and y array
        // acc in y direction is inverse
        data_arr[x+3][y+3] = (accx==x & accy==map(y,0,4,4,0));
      }
    }

    for(int z=0; z<5; z++){
      // fill accel z row
      data_arr[z+3][1] = (accz==z);
    }

    // fill c and z button bits
    data_arr[1][4] = zbut;
    data_arr[1][5] = cbut;

    // joy  joy joy 0     0     0     0     0
    // joy  joy joy accz  accz  accz  accz  accz
    // joy  joy joy 0     0     0     0     0
    // 0    0   0   accxy accxy accxy accxy accxy
    // 0    z   0   accxy accxy accxy accxy accxy
    // 0    c   0   accxy accxy accxy accxy accxy
    // 0    0   0   accxy accxy accxy accxy accxy
    // 0    0   0   accxy accxy accxy accxy accxy

    print_data();
  }
  loop_cnt++;
  delay(10);

}