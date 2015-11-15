// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SevenSegNumFont[];
extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);

Swipe N1Swipe(&B), N2Swipe(&B), N3Swipe(&B), N4Swipe(&B);
Swipe  NSwipe[4] = {N1Swipe, N2Swipe, N3Swipe, N4Swipe};

Slider slideV(&B, VERTICAL);
void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SevenSegNumFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK);

  NSwipe[0].Coords(55, 108, 95, 160);
  NSwipe[0].ShowSwipeArea(true, WHITE, ROUNDED); // Just here to show the swipe box
  NSwipe[0].setLimits(5, 10);

  NSwipe[1].Coords(100, 108, 140, 160);
  NSwipe[1].ShowSwipeArea(true, WHITE, ROUNDED); // Just here to show the swipe box
  NSwipe[1].setLimits(5, 10);

  NSwipe[2].Coords(145, 108, 185, 160);
  NSwipe[2].ShowSwipeArea(true, WHITE, ROUNDED); // Just here to show the swipe box
  NSwipe[2].setLimits(5, 10);

  NSwipe[3].Coords(190, 108, 230, 160);
  NSwipe[3].ShowSwipeArea(true, WHITE, ROUNDED); // Just here to show the swipe box
  NSwipe[3].setLimits(5, 10);
  
  slideV.Coords(5, 10, 40, 150);
  slideV.Colors(ORANGE, BLUE, ROUNDED, FILL);
  slideV.SetDirection(BTT);
  slideV.SetRange(0, 0xffff, 1);
  slideV.SetStartingValue(0x8888);
  slideV.Draw();
  myGLCD.setColor(WHITE);
}

void loop()
{
  static int number[4] = {0}, i = 0, finalNum = 0, MasterVal = random(9999);
  static byte dir[4] = {0}, lastDir[4] = {1, 1, 1, 1};
  unsigned int color = slideV.Touch();
  myGLCD.setColor(color);
  
  myGLCD.setFont(BigFont);
  myGLCD.printNumI(color, 0, 170);
  dir[i] = NSwipe[i].SwipeFromArea(Swipe_up | Swipe_down, 20); // check for a swipe, either up or down
  
  if (dir[i] != lastDir[i] )
  {
    if (dir[i] == Swipe_up)
      number[i]++;
    else if (dir[i] ==  Swipe_down)
      number[i]--;

    if (number[i] > 9) number[i] = 0;
    if (number[i] < 0) number[i] = 9;
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.printNumI(number[i], 60 + (i * 45), 110);
    
    finalNum = (number[0] * 1000) + (number[1] * 100) + (number[2] * 10) + number[3];
    
    lastDir[i] = dir[i];
    myGLCD.setFont(BigFont);
    if(finalNum > MasterVal)
      myGLCD.print(F("Too High"), 80,90);
    else if(finalNum < MasterVal)
      myGLCD.print(F("Too Low "), 80,90);
    else
    {
      myGLCD.print(F("Found it"), 80,90);
      delay(1000);
      MasterVal = random(9999); 
    }
  }

  i++; // cycle through the arrays

  if (i > 3) // if i is greater than the number of arrays, then reset it back to 0
    i = 0;
}


