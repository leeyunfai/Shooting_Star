//This program reads a keypad and displays a TFT LCD screen
#include <Adafruit_GFX.h>    // Core graphics library
#include <SoftwareSerial.h>   //for LCD purpose

/// new batch of lcd screen
#include "TFT5408.h"
TFT5408 tft;

/// old batch of lcd screen
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
//#define LCD_CS A3 // chip select pin
//#define LCD_CD A2 // SCL (clock) pin
//#define LCD_WR A1 // write pin
//#define LCD_RD A0 // read pin
//#define LCD_RESET A4 // reset pin
//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Assign human-readable names to some common 16-bit color values:
#define BLACK   (0x0000 ^ 0xFFFF)
#define BLUE    (0x001F ^ 0xFFFF)
#define RED     (0xF800 ^ 0xFFFF)
#define GREEN   (0x07E0 ^ 0xFFFF)
#define CYAN    (0x07FF ^ 0xFFFF)
#define MAGENTA (0xF81F ^ 0xFFFF)
#define YELLOW  (0xFFE0 ^ 0xFFFF)
#define WHITE   (0xFFFF ^ 0xFFFF)

#include <Fonts/FreeSerif9pt7b.h>

//pins for serial
#define RxD 11
#define TxD 10

//others
int score1, score2;
char scoreD1[0];        //char the int of score
char scoreD2[0];        //cchar the int of score
SoftwareSerial lcd(RxD, TxD);


void setup(void) {
  Serial.begin(9600);
  lcd.begin(9600);
  score1 = 0;
  score2 = 0;

  //initialize TFT LCD
  uint16_t identifier = tft.readID();

  if (identifier == 0)
    identifier = 0x5408;

  tft.reset();
  tft.begin(identifier); //on-board controller 0x9341 for old LCD screen, 0x5408 for new LCD screen
  tft.setRotation(3);
  tft.fillScreen(WHITE);

  tft.setCursor(50, 100);
  tft.setTextColor(GREEN);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(2);
  tft.println("PRESS DOWN");
  tft.setCursor(80, 140);
  tft.println("JOYSTICK");
  tft.setCursor(78, 180);
  tft.println("TO START");
  tft.fillRect(11, 24, 100, 20, BLACK);
  tft.fillRect(20, 21, 12, 3, BLACK);
  tft.fillRect(15, 44, 21, 30, BLACK);
  tft.fillRect(36, 43, 3, 6, BLACK);
  tft.fillRect(43, 44, 5, 11, BLACK);
  delay(1);
  tft.fillRect(35, 53, 11, 4, BLACK);
  tft.fillRect(104, 21, 3, 3, BLACK);
  tft.fillRect(111, 24, 9, 9, BLACK);
  tft.fillCircle(280, 40, 35, BLACK);
  tft.fillCircle(280, 40, 25, BLUE);
  tft.fillCircle(280, 40, 18, RED);
  tft.fillCircle(280, 40, 10, YELLOW);

  //sample
  /*tft.setCursor(20,20);
    tft.setTextColor(RED|YELLOW);
    tft.setTextSize(8);
    tft.println("MA2012");

    tft.setCursor(20,100);
    tft.setTextColor(GREEN);
    tft.setTextSize(4);
    tft.println("AY 2019/2020");
    tft.drawRect(10,90,300,45,GREEN); //x,y,width,height,colour
    tft.drawCircle(20,170,15,YELLOW); //x,y,radius,colour
    tft.fillRoundRect(270,155,30,30,5,YELLOW);//x,y,width,height,corner_radius,colour
    tft.drawLine(35,170,270,170,YELLOW);

    tft.setTextColor(RED|YELLOW);
    tft.setTextSize(8);*/

  //note : dimension x-320 y-240
}

void loop(void) {
  if (Serial.available() > 0) {
    char h = lcd.read();
    if (h == '0'); {
      tft.fillScreen(BLACK);
      tft.setCursor(50,100);
      tft.setTextColor(YELLOW); 
      tft.setTextSize(2);
      tft.println("GAME START");
    }
    /*score1++;                           //if hit2 is registered plus one point for player 1
      mega.write('1');                   //if target 2 is hit, player 1 score a point*/
    if (h == '1')
    {
      score1++;                           //if hit is registered plus one point
      tft.fillScreen(BLACK);              //update the display
      tft.setCursor(20, 140);
      tft.setTextColor(RED);
      tft.setTextSize(8);
      tft.println(score1);
      tft.setCursor(200, 140);
      tft.setTextColor(BLUE);
      tft.println(score2);
      delay(5);
    }
    if (h == '2')
    {
      score2++;                           //if hit is registered plus one point
      tft.fillScreen(BLACK);              //update the display
      tft.setCursor(20, 140);
      tft.setTextColor(RED);
      tft.setTextSize(8);
      tft.println(score1);
      tft.setCursor(200, 140);
      tft.setTextColor(BLUE);
      tft.println(score2);
      delay(5);
    }
    if (score1 == 5 || score2 == 5)
    {
      delay(1000);
      if (score1 == 5) tft.setTextColor(RED);             //declare winner according to their color
      else if (score2 == 5) tft.setTextColor(BLUE);
      tft.fillScreen(BLACK);
      tft.setCursor(50, 140);                             //try align at center print
      tft.setTextSize(3);
      tft.println("WINNER");
      score1 = 0;
      score2 = 0;
      
      delay(9000);
      
      //main menu
      tft.fillScreen(WHITE);
      tft.setCursor(50, 100);
      tft.setTextColor(GREEN);
      tft.setFont(&FreeSerif9pt7b);
      tft.setTextSize(2);
      tft.println("PRESS DOWN");
      tft.setCursor(80, 140);
      tft.println("JOYSTICK");
      tft.setCursor(78, 180);
      tft.println("TO START");
      tft.fillRect(11, 24, 100, 20, BLACK);
      tft.fillRect(20, 21, 12, 3, BLACK);
      tft.fillRect(15, 44, 21, 30, BLACK);
      tft.fillRect(36, 43, 3, 6, BLACK);
      tft.fillRect(43, 44, 5, 11, BLACK);
      delay(1);
      tft.fillRect(35, 53, 11, 4, BLACK);
      tft.fillRect(104, 21, 3, 3, BLACK);
      tft.fillRect(111, 24, 9, 9, BLACK);
      tft.fillCircle(280, 40, 35, BLACK);
      tft.fillCircle(280, 40, 25, BLUE);
      tft.fillCircle(280, 40, 18, RED);
      tft.fillCircle(280, 40, 10, YELLOW);
    }
    delay(5);
  }
}
