//Library
#include <Servo.h>            //Servo library
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <SoftwareSerial.h>   //for LCD purpose

//Pins. numbers to be reassign during assembly
#define X_pin1 A1    //joystick left right to control DC motor   NOTE 470 to 550 as center buffer
#define X_pin2 A3
#define Y_pin1 A2    //joystick up down to control servo on target board
#define Y_pin2 A4
#define start1 22   //joystick button to start game
#define start2 23
#define laser1 24   //laser for firing
#define laser2 25
#define board1 A7   //target board receiver for registering hit
#define board2 A8
#define LimitL1 30   //limit switch for braking left side
#define LimitL2 32
#define LimitR1 31   //limit switch for braking right side
#define LimitR2 33
#define Buzz1 36     //Play music OPTIONAL
#define Buzz2 37
#define RxD 40      //LCD CONNECTION
#define TxD 41

//Servo (1 stands for player1 control; 2 for player2 control)
Servo T1;     //target board
Servo T2;
Servo Belt1;     //target belt
Servo Belt2;
Servo Y1;     //up down
Servo Y2;
Servo X1;     //left right
Servo X2;

/*//LCD pins
#define LCD_CS A3 // Chip Select 
#define LCD_CD A2 // Command/Data 
#define LCD_WR A1 // LCD Write 
#define LCD_RD A0 // LCD Read
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //LCD pin

//color
#define BLACK   (0x0000 ^ 0xFFFF)
#define BLUE    (0x001F ^ 0xFFFF)
#define RED     (0xF800 ^ 0xFFFF)
#define GREEN   (0x07E0 ^ 0xFFFF)
#define CYAN    (0x07FF ^ 0xFFFF)
#define MAGENTA (0xF81F ^ 0xFFFF)
#define YELLOW  (0xFFE0 ^ 0xFFFF)
#define WHITE   (0xFFFF ^ 0xFFFF)*/

//Others
int SS1,SS2,SS3;        //start state
int X1P,X2P,Y1P,Y2P;    //turret position value
int T1P,T2P,B1P,B2P;    //target board position value
int Jx1,Jx2,Jy1,Jy2;    //joystick position
int speed1,speed2;      //speed of motor moving left
int laserS1,laserS2;    //laser firing state from data xfer
int TL1,TL2,TR1,TR2;    //limit switch state
int score1=0;           //score of player1
int score2=0;           //score of player2
int hit1,hit2;          //laser registered hit state
int CD1,CD2;            //hit cooldown/invincible
int time1,time2;        //time for tracking invicible state
const int invic = 2000;  //2sec invicible time

//comm nodes
RF24 radio(7, 8);               // nRF24L01 (CE,CSN) PINS
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this main arduino in Octal format 
const uint16_t node01 = 01;      // Address of the gloves in Octal format
const uint16_t node02 = 02;

//Data Packages
struct Data_Package {
  byte Dx;
  byte Dy;
  byte Df;
};

Data_Package data;

//Software Serial comm for LCD score system
SoftwareSerial mega(RxD, TxD);                      //Master Arduino acts as a serial communication device

/*
components
6 Servos: 2each for turret, 1ea for target
2 Motor
2 lasers
2 laser sensors
2 wireless mod
2 buzzers
4 limit switches to limit motors
2 joystick
LCD
 */


void setup() {
  //initialise comm
  /*radio.begin();
  radio.openReadingPipe(address);  //SETUP channel IP for comm, complement .openReadingPipe
  radio.setPALevel(RF24_PA_MIN); //strength of signal, maybe using max
  radio.stoplistening();*/
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);

  //initialise input pins
  pinMode(X_pin1, INPUT);
  pinMode(X_pin2, INPUT);
  pinMode(Y_pin1, INPUT);
  pinMode(Y_pin2, INPUT);
  pinMode(board1, INPUT);
  pinMode(board2, INPUT);
  pinMode(LimitL1, INPUT);
  pinMode(LimitR1, INPUT);
  pinMode(LimitL2, INPUT);
  pinMode(LimitR2, INPUT);
  pinMode(start1, INPUT);
  pinMode(start2, INPUT);
  
  //initialise output pins
  pinMode(laser1, OUTPUT);
  pinMode(laser2, OUTPUT);
  pinMode(Buzz1, OUTPUT);
  pinMode(Buzz2, OUTPUT);

  //initialise Servo pin. ANY FREE DIGITAL
  X1.attach(2);
  X2.attach(3);
  Y1.attach(4);
  Y2.attach(5);
  T1.attach(6);
  T2.attach(7);
  Belt1.attach(8);
  Belt2.attach(9);
  
  /*//initialise LCD
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  tft.setCursor(20,100);
  tft.setTextColor(GREEN); 
  tft.setTextSize(4);
  tft.println("PRESS JOYSTICK BUTTON TO START");*/

  //setting up the initial position of turret
  X1P = 90;             //turret, 90 center
  X2P = 90;
  Y1P = 90;             //90 center
  Y2P = 90;
  T1P = 90;             //target, 90 center
  T2P = 90;
  B1P = 90;             //90 = stop
  B2P = 90;
  X1.write(X1P);
  X2.write(X2P);
  Y1.write(Y1P);
  Y2.write(Y2P);
  T1.write(T1P);
  T2.write(T2P);
  Belt1.write(B1P);
  Belt2.write(B2P);

  //others
  SS3=0; //game not start upon resetting/turning on for first time
  mega.begin(9600); //initialise mega serial comm
  CD1 = 0;          //make sure no cooldown at start
  CD2 = 0;
  time1 = 0;        //time tracking is 0
  time2 = 0;
  Serial.begin(9600); //checking purpose
}

void loop() {                            //one loop is one cycle. current period is 50ms
  //starting the game by pressing 
  if (SS3 = 0)                          //to check for game start
  {
  SS1 = digitalRead(start1);
  SS2 = digitalRead(start2);
  }
  if (SS1 == LOW && SS2 == LOW && SS3 == 0) //To start the game, need press down the joystick button together. NOTE check LOW/HIGH for button press
  {
    /*tft.fillScreen(BLACK);
    tft.setCursor(20,100);
    tft.setTextColor(YELLOW); 
    tft.setTextSize(4);
    tft.println("GAME START");*/
    SS3 = 1;
    Serial.println("START");               //checking purpose
    mega.write('0');                       //send indication to LCD that the game has started  
  }
  
  //=== Receiving data from 2 gloves ===//
  /*NOTE scaling and range limit should be done on 
  glove arduino itself to save processing power here.
  data recieve should be in written form*/
  
  network.update();
  
  while ( network.available() ) // check for incoming data
  {     
    RF24NetworkHeader header;
    network.read(header, &data, sizeof(Data_Package)); // Read the incoming data
    if (header.from_node == 01) {             // If data comes from glove 1
      X1P = data.Dx;                          //extract
      Y1P = data.Dy;
      laserS1 = data.Df;
      Serial.println("GLOVE 111111 ONLINE:");      //checking purpose
      Serial.print(X1P);
      Serial.print("\t");
      Serial.print(Y1P);
      Serial.print("\t");
      Serial.println(laserS1);
    }
    if (header.from_node == 02) {             // If data comes from glove 2
      X2P = data.Dx;
      Y2P = data.Dy;
      laserS2 = data.Df;
      Serial.println("GLOVE 222222 ONLINE:");      //checking purpose
      Serial.print(X2P);
      Serial.print("\t");
      Serial.print(Y2P);
      Serial.print("\t");
      Serial.println(laserS2);
    }
  }
  
  
  
  if (SS3=1)                                                //when game starts
  {
    //== control for player1's turret ==//
    X1.write(X1P);
    Y1.write(Y1P);
    if (laserS1 == 1) digitalWrite(laser1, HIGH);           //fire laser if state is 1
    else if (laserS1 == 0) digitalWrite(laser1, LOW);       //off laser if state is 0
  
    //==control for player2's turret==//
    X2.write(X2P);
    Y2.write(Y2P);
    if (laserS2 == 1) digitalWrite(laser2, HIGH);           //fire laser if state is 1
    else if (laserS2 == 0) digitalWrite(laser2, LOW);       //fire laser if state is 0
  
    //==control for player1's target==//
    Jy1 = analogRead(Y_pin1);                     //Read up-down value of joystick Yaxis
    T1P = map(Jy1, 0, 1023, 0, 180);              //if direction is opposite: swap 0 and 180 value. up and down
    T1.write(T1P);                                //move the servo in direction

    Jx1 = analogRead(X_pin1);                 //Read left-right value of joystick Xaxis
    TL1 = digitalRead(LimitL1);               //Read left limit status
    TR1 = digitalRead(LimitR1);               //Read right limit status
    
    if (Jx1 > 470 && Jx1 < 550 && TR1 == HIGH && TL1 == HIGH)               //470 to 550 as center buffer. when limit switch is not pressed
    {                                                                       //NOTE check if HIGH is pressed or not, assuming LOW means pressed
      Belt1.write(90);                                                         //STOP
    }
    else if (Jx1 >= 0 && Jx1 <= 470 &&( (TR1 == HIGH && TL1 == HIGH)||(TR1 == LOW && TL1 == HIGH) ))         //tilt left, also when RIGHT limit is pressed
    {
      B1P = map(Jx1, 0, 470, 180, 90);                                    //scale, modulation with left and right movement.  NOTE CHECK DIRECTION, LEFT MAYBE RIGHT. 
      Belt1.write(B1P);
    }
    else if (Jx1 >= 550 && Jx1 <= 1023 &&( (TR1 == HIGH && TL1 == HIGH)||(TR1 == HIGH && TL1 == LOW) ))       //tilt right, also when LEFT limit is pressed
    {
      B1P = map(Jx1, 550, 1023, 90, 0);                                 //scale
      Belt1.write(B1P);
    }
  
    //==control for player2's target==//
    Jy2 = analogRead(Y_pin2);                 //Read up-down value of joystick Yaxis
    T2P = map(Jy2, 0, 1023, 0, 180);          //if direction is opposite: swap 0 and 180 value
    T2.write(T2P);                            //move the servo in direction

    Jx2 = analogRead(X_pin2);                 //Read left-right value of joystick Xaxis
    TL2 = digitalRead(LimitL2);               //Read left limit status
    TR2 = digitalRead(LimitR2);               //Read right limit status

    if (Jx2 > 470 && Jx2 < 550 && TR2 == HIGH && TL2 == HIGH)               //470 to 550 as center buffer. when limit switch is not pressed
    {                                                                       //NOTE check if HIGH is pressed or not, assuming LOW means pressed
      Belt2.write(90);                                                         //STOP
    }
    else if (Jx2 >= 0 && Jx2 <= 470 &&( (TR2 == HIGH && TL2 == HIGH)||(TR2 == LOW && TL2 == HIGH) ))         //tilt left, also when RIGHT limit is pressed
    {
      B2P = map(Jx2, 0, 470, 180, 90);                                    //scale
      Belt2.write(B2P);                                                     
    }
    else if (Jx2 >= 550 && Jx2 <= 1023 &&( (TR2 == HIGH && TL2 == HIGH)||(TR2 == HIGH && TL2 == LOW) ))       //tilt right, also when LEFT limit is pressed
    {
      B2P = map(Jx2, 550, 1023, 90, 0);                                 //scale
      Belt2.write(B2P);
    }
    
    //==Scoring system==// NOTE NOW JUST SEND UART FOR OTHER LCD ARDUINO FOR DISPLAY
    hit1 = analogRead(board1);           //read state of target board PHOTORESISTOR. around 560 for normal, 800 for tag. Try miinmal 700 for tag
    hit2 = analogRead(board2);
    if (hit1 >= 700 && CD1 == 0)        //if have not been hit for last 2sec
    {
      score2++;                           //if hit2 is registered plus one point for player 2
      mega.write('2');                   //if target 1 is hit, player 2 score a point
      CD1 = 1;                          //activate invincibility
    }
    if (CD1 == 1) 
    {
      time1 = time1 + 50;               //every loop is 50ms, so if hit, loop for 2 sec, then change to mortal state
      if (time1 >= invic)               //this is to prevent possible multiple hits for one tag, one tag maybe more than 50ms, which result in multiple hits
      {
        CD1 = 0;                        //remove cooldown
        time1 = 0;                      //cooldown time reset to 0
      }
    }
    if (hit2 >= 700 && CD2 == 0) 
    {
      score1++;                           //if hit2 is registered plus one point for player 1
      mega.write('1');                   //if target 2 is hit, player 1 score a point
      CD2 = 1;                          //activate invincibility
    }
    if (CD2 == 1) 
    {
      time2 = time2 + 50;               //every loop is 50ms, so if hit, loop for 2 sec, then change to mortal state
      if (time2 >= invic)               //this is to prevent possible multiple hits for one tag, one tag maybe more than 50ms, which result in multiple hits
      {
        CD2 = 0;                        //remove cooldown
        time2 = 0;                      //cooldown time reset to 0
      }
    }
    

    
    //==stop the game/win condition==//
    if (score1 == 5 || score2 == 5)
    {
      SS3=0;                                              //RESET GAME STATE for another round or just end game
      score1 = 0;
      score2 = 0; 
    }
    //delay(50);                                            //delay for one cycle loop
  } 
  delay(50);                                            //delay for one cycle loop
}
