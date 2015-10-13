// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

//UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);//ITDB32S
UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
CustomButton CB(&B), CB2(&B);
CustomButton * myButtons[] = {&CB, &CB2};
Radio myRB;

CustomButton::Points Octagon[] = {
  {40, 0},
  {70, 0},
  {100, 30},
  {100, 60},
  {70, 90},
  {40, 90},
  {10, 60},
  {10, 30}
};

CustomButton::Points Moon[] = {
  {150, 10},
  {200, 10},
  {165, 30},
  {165, 70},
  {200, 100},
  {150, 100},
  {120, 70},
  {120, 30}
};

void setup()
{
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);
  myGLCD.setColor(RED); // text color
  
  CB.Coords(Octagon);
  CB2.Coords(Moon);
  CB.Colors(RED, BLUE, NOFILL);
  CB2.Colors(GREEN, PURPLE, FILL);
  
  CB.Draw();
  CB2.Draw();
}

void loop()
{
//  myGLCD.print((CB.Delay(1000) ? "TRUE " : "FALSE"), 0, 150);
//  CB2.Touch();
  myRB.RadioButtons(myButtons);
}




