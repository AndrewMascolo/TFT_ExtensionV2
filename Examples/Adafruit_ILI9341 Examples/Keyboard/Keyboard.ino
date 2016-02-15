#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

#define TFT_RST 47
#define TFT_DC 46
#define TFT_CS 44
#define TFT_MOSI 51 //11
#define TFT_MISO 50 //12 Direct Connection to Arduino
#define TFT_CLK  52 //13


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
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

Base B(&tft, &myTouch); // Base class, NEEDED!!!

MobileKeyboard myKB(&B);
const char textLimit = 30;
char MyBuffer[textLimit];

void setup()
{
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");

  tft.begin();
  tft.setRotation(2);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  tft.fillScreen(ILI9341_BLACK);

  myKB.Colors(WHITE, BLACK, BLUE, NOFILL);
  myKB.make_Mobile_Keyboard(Mobile_KB);
}

void loop()
{
  myKB.Mobile_KeyBoard(MyBuffer, BLUE);
}

