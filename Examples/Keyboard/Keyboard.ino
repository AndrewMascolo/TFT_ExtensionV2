#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
// Declare which fonts we will be using
extern uint8_t BigFont[];

//UTFT    myGLCD(CTE70, 25,26,27,28);
UTFT    myGLCD(ITDB32S, 38,39,40,41);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);

MobileKeyboard myKB(&B);
char* msg;

void setup()
{
  Serial1.begin(9600); // default bluetooth baudrate.
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  
  myGLCD.fillScr(0,0,0);
  
  myKB.Colors(WHITE, BLACK, BLUE, FILL);
  myKB.SetupMobileKB(0,20, 320, 115); //NEEDED TO WORK!
}

void loop()
{ 
  myKB.ReceiveMsg(0,0,YELLOW); // X,Y(position on screen), Color
  msg = myKB.Mobile_KeyBoard(BLUE);
  if(msg != NULL)
    myGLCD.print(msg,0,0);

}


