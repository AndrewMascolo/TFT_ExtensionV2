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

int resistorBands[12] = {BLACK, BROWN, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, GREY, WHITE, SILVER, GOLD};
extern unsigned short resistorImg[0x9FA8];
void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SevenSegNumFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK);

  NSwipe[0].Coords(65, 108, 105, 160);
  NSwipe[0].setLimits(5, 10);

  NSwipe[1].Coords(110, 108, 150, 160);
  NSwipe[1].setLimits(5, 10);

  NSwipe[2].Coords(155, 108, 195, 160);
  NSwipe[2].setLimits(5, 10);

  NSwipe[3].Coords(200, 108, 240, 160);
  NSwipe[3].setLimits(5, 10);
  
  myGLCD.drawBitmap (0, 55, 312, 131, resistorImg, 1);
}

void loop()
{
  static int number[4] = {0}, i = 0; 
  static byte dir[4] = {0}, lastDir[4] = {1, 1, 1, 1};
  
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
    unsigned long finalNum = ((number[0] * 10) + number[1]) * pow(10, number[2]);
    NSwipe[i].ShowSwipeArea(true, resistorBands[( (i == 3)? (number[i]%2) + 10 : number[i] )], SQUARED, FILL);
    
    myGLCD.setColor(number[i]? BLACK : WHITE);
    myGLCD.setBackColor(TRANSPARENT);
    myGLCD.printNumI(((i == 3)? (number[i]%2) : number[i]), 70 + (i * 45), 110);
    
    lastDir[i] = dir[i];
    myGLCD.setFont(BigFont);
    myGLCD.setColor(WHITE);
    myGLCD.setBackColor(BLACK);
    
    char buf[20] = {0};
    sprintf(buf, "%9d +-%02d%c", finalNum, (number[3]? 5 : 10), '%');
    myGLCD.print(buf, 60,70);
  }

  i++; // cycle through the arrays

  if (i > 3) // if i is greater than the number of arrays, then reset it back to 0
    i = 0;
}


