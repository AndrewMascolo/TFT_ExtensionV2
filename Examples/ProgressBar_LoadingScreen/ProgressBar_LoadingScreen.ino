// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
//#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
//UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD);
ProgressBar Loading(&B);

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x00);
  myGLCD.setColor(0xFFFF);
  
  int Dx = myGLCD.getDisplayXSize();
  int Dy = myGLCD.getDisplayYSize();
  Loading.Coords(0, Dy/2, Dx, Dy/2);
  Loading.Attributes("NOW LOADING, PLEASE WAIT", WHITE, Big, ':');
  Loading.Progress();

}

void loop()
{

}
  
