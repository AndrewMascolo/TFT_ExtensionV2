// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);

Slider SlideV(&B, VERTICAL);
Swipe Swipe(&B);

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);

  SlideV.Coords(10, 50, 70, 145);              // FIRST, must set coordinates before setting Range and optional starting value
  SlideV.Colors(GREEN, RED, ROUNDED, FILL);    // Optional but good to have some color
  SlideV.SetDirection(BTT); // bottom to top   // Optional
  SlideV.SetRange(25,255,25);                  // SECOND, Must be set after coordinates
  SlideV.SetStartingValue(255);                // THIRD (Optional, but must be set after range)
  SlideV.Draw();

  Swipe.Coords(150,110,170,130);
  Swipe.ShowSwipeArea(true); // Just here to show the swipe box
  
  myGLCD.setColor(WHITE);
}

void loop()
{
  myGLCD.printNumI(SlideV.Touch(), 160,50);

  //myGLCD.printNumI(Swipe.SwipeFromArea(Swipe_up | Swipe_down, 50), 0, 20);
}


