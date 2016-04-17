// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch); // Base class, NEEDED!!!

Box Button(&B); // basic boxed button
Circle CButton(&B); // basic circle button
Triangle TButton(&B); // basic triangle button
Base * myOtherButtons[] {&Button, &CButton, &TButton}; // Array of Mixed Buttons

Radio myTRB; // create a RadioButton instance to be used with the triangle buttons

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK); // these two should be the same
  myGLCD.setBackColor(BLACK); // ^^^^^^^^^

  Button.Coords(20, 10, 80, 60);
  Button.Colors(0xD7E0, 0x001F, ROUNDED, FILL);
  Button.Text("1",RED, Big);
  Button.Draw();

  CButton.Coords(50, 120, AUTO);
  CButton.Colors(RED, PURPLE, FILL);
  CButton.Text("Hello",GREEN, Big);
  CButton.Draw();

  TButton.Coords(50, 210, 40);
  TButton.Colors(GREEN, RED, FILL);
  TButton.Draw();
}

void loop()
{ 
   myTRB.RadioButtons(myOtherButtons); 
}  
