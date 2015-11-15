#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

#define Mega 1

#if Mega
#define TFT_RST 47
#define TFT_DC 46
#define TFT_CS 44
#define TFT_MOSI 51 //11
#define TFT_MISO 50 //12 Direct Connection to Arduino
#define TFT_CLK  52 //13
#else  // Smaller controller
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 7
#define TFT_MOSI 11 //11
#define TFT_MISO 12 //12 Direct Connection to Arduino
#define TFT_CLK  13 //13
#endif

/*
I have it connected up to a Mega 2560.

Wiring:

PIN     MEGA       CD4050     2.2TFT
SCK       52           YES         SCK
MOSI      51           YES         MOSI
MISO      50            NO         MISO
D/C        4            YES         D/C
RST        5            YES         RST
CS         11           YES         CS

*/

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC

UTouch  myTouch( 11, 13, 15, 17, 19);
// If using the breakout, change pins as desired
#if Mega
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
#else
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

Base B(&tft, &myTouch); // Base class, NEEDED!!!

Gauge myGauge(&B);

void setup()
{
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");

  tft.begin();
  tft.setRotation(3);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  tft.fillScreen(ILI9341_BLACK);
  
  myGauge.Coords(160, 120, 80, 20);
  myGauge.ArcDegrees(225, -45);
  myGauge.TextRange(0, 150, 15, OUTSIDE, 2);
  myGauge.Colors(YELLOW, BLUE);
  myGauge.HeaderText("SPEED", 2);
  myGauge.FooterText("MPH", 2);
  myGauge.Draw();
  
  for(int i = 0; i <= 100; i++)
    myGauge.Progress(i);
  
  delay(2000);
  
  for(int i = 100; i >= 0; i--)
    myGauge.Progress(i); 
  
}

void loop()  
{
  
}

