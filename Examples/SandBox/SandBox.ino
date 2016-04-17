// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
//#include <UTouch.h>
#include <TFTSpritesV2.h>

template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

extern uint8_t SpriteSheet[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTouch  myTouch( 6, 5, 4, 3, 2);
TFTSpritesV2 TFTG(&myGLCD);

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  //myTouch.InitTouch(LANDSCAPE);
  //myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK);
  myGLCD.setColor(WHITE);
  myGLCD.setFont(SpriteSheet);
  myGLCD.printChar(5, 300,300);
  
  TFTG.setSpriteList(SpriteSheet);
  for (byte i = 0; i < 28; i++)
  {
    static int X = 0;
    TFTG.drawSprite(i, X, 5, 1);
    X += 34;
  }
}

void loop()
{

}
