#include <ITDB02_Graph16.h>
#include <ITDB02_Touch.h>
#include <TFT_ExtensionV2.h>
extern uint8_t SmallFont[];
ITDB02 myGLCD(A1, A2, A0, A3, A5, ITDB32S);
//myTouch(TCLK,TCS,DIN,DOUT,IRQ);
ITDB02_Touch  myTouch(13, 10, 11, 12, A4);

/* // Use IDE 1.0.6
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];
//myGLCD(RS,WR,CS,RST,ALE,mode);
UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
//TFT_Extension myTFT(&myGLCD, &myTouch);
*/

#define TXC 10
#define TYC 10
#define BXC 70
#define BYC 60
#define Spacing BXC

#include <VirtualWire.h>
char remote[4] = {'X', 'Y', 'Z', '\0'};
byte Data = 0;
byte i = 0;

Base B(&myGLCD, &myTouch);  // Base class

Box *Button[4][2]; // Boxed button
Box MenuButton(&B);

void setup()
{
  vw_setup(2000);	// Bits per sec, must match receiver.
  vw_set_tx_pin(51); // Data Pin, change for your needs
  pinMode(53, OUTPUT); // for my purposes
  digitalWrite(53, HIGH); // for my purposes

  for (byte i = 0; i < 8; i++)
    Button[i % 4][i / 4] = new Box(&B);

  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_LOW);
  myGLCD.fillScr(BLACK);

  for (byte i = 0; i < 8; i++)
  {
    Button[i % 4][i / 4]->Coords((TXC + (i % 4) * Spacing), (TYC + (i / 4) * Spacing), (BXC + (i % 4) * Spacing), (BYC + (i / 4) * Spacing));
    Button[i % 4][i / 4]->Text(BLACK, Big, "ON", "OFF");
  }
  Menu_1();

  MenuButton.Coords(10, 150, 280, 190);
  MenuButton.Colors(ORANGE, RED, FILL, ROUNDED);
  MenuButton.Text(BLACK, Big, "MENU 2", "MENU 1");
  MenuButton.Draw();
}

void loop()
{
  static byte last = 255, lastMenu = 255;

  byte MB = MenuButton.Latch();
  if (MB != lastMenu) // set button colors
  {
    lastMenu = MB;

    if (MB == 0)
      Menu_1();
    else
      Menu_2();
  }

  if (i < 8)
  {
    Button[i % 4][i / 4]->Draw();
    Button[i % 4][i / 4]->Latch();
    i++;
  }
  else i = 0;

  if (MB == 0)
    remote[1] = Data;
  else
    remote[2] = Data;

  if (Data != last) // Debug and see what the value is
  {
    myGLCD.setColor(BLACK);
    myGLCD.print("   ", 0, 220);
    myGLCD.printNumI(int(Data), 0, 220);
    last = Data;
  }

  if (Data == 0)
  {
    remote[0] = 'X';
    remote[1] = 0xFF;
    remote[2] = 0xFF;
  }
  else
    remote[0] = 'A';

  myGLCD.printChar(remote[0], 0, 200);
  vw_send((uint8_t *)remote, strlen(remote));
}

void Menu_1()
{
  for (byte i = 0; i < 8; i++)
  {
    Button[i % 4][i / 4]->Colors(GREEN, RED, FILL, ROUNDED);
    Button[i % 4][i / 4]->ReDraw();
  }
}

void Menu_2()
{
  for (byte i = 0; i < 8; i++)
  {
    Button[i % 4][i / 4]->Colors(GREEN, ORANGE, FILL, ROUNDED);
    Button[i % 4][i / 4]->ReDraw();
  }
}


