// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch); // Base class, NEEDED!!!

Box Button(&B), Button1(&B), Button2(&B); // basic boxed buttons

Circle CButton(&B), CButton1(&B); // basic circle buttons

Triangle TButton(&B), TButton1(&B), TButton2(&B); // basic triangle buttons
Triangle * myTButtons[] = {&TButton, &TButton1, &TButton2}; // array of triangle buttons

Radio myTRB; // create a RadioButton instance to be used with the triangle buttons

void setup()
{
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);

  Button.Coords(10, 10, 60, 60);
  Button.Colors(0xD7E0, 0x001F, FILL, ROUNDED);
  Button.Draw();

  Button1.Coords(110, 10, 160, 60);
  Button1.Colors(0xD7E0, 0x001F, NOFILL, ROUNDED);
  Button1.Draw();

  Button2.Coords(210, 10, 260, 60);
  Button2.Colors(0xD7E0, 0x001F, FILL, SQUARED);
  Button2.Draw();

  CButton.Coords(80, 120, 50);
  CButton.Colors(RED, PURPLE, FILL);
  CButton.Text(GREEN, Big, "Hello");
  CButton.Draw();

  CButton1.Coords(220, 120, 50);
  CButton1.Colors(YELLOW, BLUE, NOFILL);
  CButton1.Text(RED, Big, "Hello", "Seeya");
  CButton1.Draw();

  TButton.Coords(50, 210, 40);
  TButton.Colors(GREEN, RED, FILL);
  TButton.Draw();

  TButton1.Coords(150,210,40);
  TButton1.Colors(GREEN, RED, FILL);
  
  TButton2.Coords(250,210,40);
  TButton2.Colors(GREEN, RED, FILL);
}

void loop()
{
   Button.Touch();
   Button1.Latch();
   Button2.Delay(1000);
   
   CButton.Touch();
   CButton1.Touch();
   
   //The triangles are there but there is a glitch I am still working out.
   myTRB.RadioButtons(myTButtons);  
}
