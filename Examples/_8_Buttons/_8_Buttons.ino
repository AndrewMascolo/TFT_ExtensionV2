 // Use IDE 1.0.6
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];
//myGLCD(RS,WR,CS,RST,ALE,mode);
UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
//TFT_Extension myTFT(&myGLCD, &myTouch);

byte id = 0;
Base B(&myGLCD, &myTouch);

Box *Button[4][2];

void setup()
{
  for (byte i = 0; i < 8; i++)
    Button[i % 4][i / 4] = new Box(&B);
    
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_LOW);
  myGLCD.fillScr(0, 0, 0);

  for (byte i = 0; i < 8; i++)
  {
    Button[i % 4][i / 4]->Coords(50 + (i % 4) * 60, 50 + (i / 4) * 60, 100 + (i % 4) * 60, 100 + (i / 4) * 60);
    Button[i % 4][i / 4]->Colors(GREEN, RED, FILL, ROUNDED);
    Button[i % 4][i / 4]->Text(BLACK, Small, "ON", "OFF");
    Button[i % 4][i / 4]->Draw();
  }
}

void loop()
{
  if (id < 8)
  {
    Button[id % 4][id / 4]->Latch();
    id++;
  }
  else id = 0;
}

