//Library
#include <Wire.h>
#include <DFRobot_LIS2DH12.h>
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

//Pins
#define trigger 5

//Others
DFRobot_LIS2DH12 LIS; //Accelerometer
int16_t x, y, z;
int fire,fires;
int angleX,angleY,T1P;
//int angleXa,angleYa;
const unsigned long interval = 10;  //ms  // How often to send data to the other unit
unsigned long last_sent;            // When did we last send?

//comms
RF24 radio(7, 8);               // nRF24L01 (CE,CSN) DIGITAL
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

//Data package
struct Data_Package {
  byte Dx;
  byte Dy;
  byte Df;
};

Data_Package data;              //Create a variable with the above structure

/*components
accelerometer: use 2 axis == 2 pins /sigitalread
switch for firing/ digitalread
data transfer/ 4 pins / data to be transfer in 
*/

void setup() {
  //comm
  /*radio.begin();
  radio.openWritingPipe(address);  //SETUP channel IP for comm, complement .openReadingPipe
  radio.setPALevel(RF24_PA_MIN); //strength of signal, maybe using max
  radio.stoplistening();*/
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  
  Wire.begin();
  Serial.begin(115200);
  while(!Serial);
  delay(100);

  // Set measurement range
  // Ga: LIS2DH12_RANGE_2GA
  // Ga: LIS2DH12_RANGE_4GA
  // Ga: LIS2DH12_RANGE_8GA
  // Ga: LIS2DH12_RANGE_16GA
  while(LIS.init(LIS2DH12_RANGE_8GA) == -1){  //Equipment connection exception or I2C address error
    Serial.println("No I2C devices found");
    delay(1000);
    }
  
  //initialise pin
  pinMode(trigger, INPUT);
}

void loop() {
  //data reading
  acceleration();
  fire = digitalRead(trigger);

  //data processing
    if (x<0)
  {
    angleX = map(z, 1000, -1000, 120, 60);          //150 to 30 left right
    angleY = map(y, 1000, -1000, 70, 110);        //30 to 150 up down
  }
  
  data.Dx = angleX;                                           //drive servo to its position
  data.Dy = angleY;

  if (fire == HIGH) fires = 1;      //HIGH means fire. circuit: ground to 5v
  else if (fire ==LOW) fires = 0;
  data.Df = fires;
  
  Serial.print("fire:");            //checking purpose, baud 115200
  Serial.print("\t");
  Serial.println(fires);
  Serial.print(angleX);
  Serial.print("\t");
  Serial.println(angleY);

  //data sending. Possible problem is unable to identify the data sent.
  network.update();
  //===== Sending =====//
  unsigned long now = millis();
  if (now - last_sent >= interval) {   // If it's time to send a data, send it!
    last_sent = now;
    RF24NetworkHeader header2(master00);   // (Address where the data is going)
    bool ok = network.write(header2, &data, sizeof(Data_Package)); // Send the data
  }
}

void acceleration(void)
{
  //int16_t x, y, z;

  //delay(1000);
  LIS.readXYZ(x, y, z);
  LIS.mgScale(x, y, z);

  /*Serial.print("Acceleration x: "); //print acceleration
  Serial.print(x);
  Serial.print(" mg \ty: ");
  Serial.print(y);
  Serial.print(" mg \tz: ");
  Serial.print(z);
  Serial.println(" mg");*/
}
