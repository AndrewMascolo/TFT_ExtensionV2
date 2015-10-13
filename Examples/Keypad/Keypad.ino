// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch); // Base class, NEEDED!!!

Circle *keypadButtons[12];

char *KPtext[12] = {
  "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  
void setup()
{
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);
  for (byte i = 0; i < 12; i++)
    keypadButtons[i] = new Circle(&B);

  KPtext[9] = "*";
  KPtext[10] = "0";
  KPtext[11] = "#";

  for (byte i = 0; i < 12; i++)
  {
    keypadButtons[i]->Coords(70 + (80 * (i % 3)), 30 + (55*(i/3)), 26);
    keypadButtons[i]->Colors(GREEN, BLUE, FILL);
    keypadButtons[i]->Padding(2);
    keypadButtons[i]->Text(KPtext[i],WHITE, Big);
    keypadButtons[i]->Draw();
  }
}

void loop()
{
  static byte index = 0;

  index++;
  if(index > 11)
    index = 0;

  keypadButtons[index]->Touch(); 
}

