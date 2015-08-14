#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 25,26,27,28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base Obj(&myGLCD, &myTouch);

MobileKeyboard myKB(&Obj);
char* msg;

void setup()
{
  Serial1.begin(9600); // default bluetooth baudrate.
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  
  myGLCD.fillScr(0,0,0);

  myKB.SetupMobileKB(0,150, 400, 200); //NEEDED TO WORK!
}

void loop()
{ 
  myKB.ReceiveMsg(0,0,YELLOW); // X,Y(position on screen), Color
  msg = myKB.Mobile_KeyBoard(BLUE);
  if(msg != NULL)
    myGLCD.print(msg,0,50);

}


