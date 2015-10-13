// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT     myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch); // Base class, NEEDED!!!

Box ButtonUp(&B), ButtonDn(&B); // create Buttons to use

Cycle cycle; // construct cycle class

void (*myFunctions[3])(void) = {text1, text2,text3}; // put the functions you want into an array

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK); // these two should be the same
  myGLCD.setColor(WHITE); // ^^^^^^^^^

  ButtonUp.Coords(10, 50, AUTO, 100);
  ButtonUp.Colors(0xD7E0, 0x001F, ROUNDED, FILL);
  ButtonUp.Text("UP", Big, WHITE);
  ButtonUp.Draw();
  
  ButtonDn.Coords(110, 50, AUTO, 100);
  ButtonDn.Colors(0xD7E0, 0x001F, ROUNDED, FILL);
  ButtonDn.Text("DN", Big, WHITE);
  ButtonDn.Draw();
}

void text1()
{
  myGLCD.print("Test 1", 0,0); 
}

void text2()
{
  myGLCD.print("Test 2", 0,0); 
}

void text3()
{
  myGLCD.print("Test 3", 0,0); 
}

void loop()
{
  cycle.CycleButton(&ButtonUp, &ButtonDn, myFunctions); // default is NoRoll
}
