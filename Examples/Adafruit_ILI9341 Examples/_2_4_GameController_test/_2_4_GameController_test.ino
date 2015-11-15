#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <VirtualWire.h>

#define IsWithin(x, a, b) ((x>=a)&&(x<=b))
// For the Adafruit shield, these are the default.
#define Mega 1
#define Debug 0
#define showJoystickValues 1
#define showJoysticks 0

#if Mega
#define TFT_RST 47
#define TFT_DC 46
#define TFT_CS 44
#define TFT_MOSI 51 
#define TFT_MISO 50 // Direct Connection to Arduino
#define TFT_CLK  52 //
#else  // Smaller controller
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 7
#define TFT_MOSI 11 //
#define TFT_MISO 12 // Direct Connection to Arduino
#define TFT_CLK  13 //
#endif

#define Deg_to_rad(x) (x*0.01745 + 3.14159265)

const byte AcelX = A1;
const byte AcelY = A3;
const byte AcelZ = A5;

const byte Ly = A0;
const byte Lx = A2;
const byte Ry = A4;
const byte Rx = A6;

char Array[8] = {'\0'};

#if Debug
unsigned int
joyLymax = 0, joyLymin = 0xffff,
joyLxmax = 0, joyLxmin = 0xffff,
joyRymax = 0, joyRymin = 0xffff,
joyRxmax = 0, joyRxmin = 0xffff;
#endif

typedef struct Joysticks
{
  long Ly;
  long Lx;
  long Ry;
  long Rx;
};

/*
I have it connected up to a Mega 2560.

Wiring:

PIN     MEGA       CD4050     2.2TFT
SCK       52           YES         SCK
MOSI      51           YES         MOSI
MISO      50            NO         MISO
D/C        4            YES         D/C
RST        5            YES         RST
CS         11           YES         CS

*/

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC

UTouch  myTouch(11, 13, 15, 17, 19);
// If using the breakout, change pins as desired
#if Mega
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
#else
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

Base B(&tft, &myTouch); // Base class, NEEDED!!!

Box menuButton(&B), toggleSendJoysticks(&B);
Swipe showKB(&B);
MobileKeyboard myKB(&B);

const char textLimit = 25;
char MyBuffer[textLimit];

void setup()
{
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");

  tft.begin();
  tft.setRotation(3);
  vw_setup(2000);	 // Bits per sec
  vw_set_tx_pin(33);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  tft.fillScreen(DARK_PURPLE);
#if showJoysticks
  tft.setTextColor(0xffff, 0x0);
  tft.drawCircle(60, 120, 50, ILI9341_WHITE);
  tft.drawCircle(260, 120, 50, ILI9341_WHITE);
  tft.setCursor(0, 0);
#endif

  menuButton.Coords(10, 10, AUTO, AUTO);
  menuButton.Colors(GREEN, RED, ROUNDED, FILL);
  menuButton.Text("Back", "Menu", WHITE, Big);
  menuButton.Draw();

  toggleSendJoysticks.Coords(100, 10, AUTO, AUTO);
  toggleSendJoysticks.Colors(GREEN, RED, ROUNDED, FILL);
  toggleSendJoysticks.Text("ON", "OFF", WHITE, Big);
  toggleSendJoysticks.Draw();

  showKB.Coords(0, 220, 319, 239);
  showKB.ShowSwipeArea(true); // Just here to show the swipe box
  showKB.setLimits(5, 10);

  myKB.Colors(WHITE, BLACK, BLUE, NOFILL);
}

void loop(void)
{
  bringUpKB();
  menuButton.Toggle();
  if(toggleSendJoysticks.Toggle())
    SendJoyStickData();
}

void bringUpKB()
{
  static byte show, lastState, type = false;

  show = showKB.SwipeFromArea(Swipe_up | Swipe_down, 50);
  if (show != lastState)
  {
    if (show == Swipe_up)
    {
      tft.fillScreen(DARK_PURPLE);
      showKB.Coords(0, 80, 319, 100);
      showKB.ShowSwipeArea(true);
      myKB.make_Mobile_Keyboard(Mobile_KB);
      type = true;
    }
    if (show == Swipe_down)
    {
      showKB.Coords(0, 220, 319, 239);
      tft.fillScreen(DARK_PURPLE);
      type = false;
      
      menuButton.ReDraw();
      toggleSendJoysticks.ReDraw();
      showKB.ShowSwipeArea(true);
    }
    lastState = show;
  }

  if (type)
    myKB.Mobile_KeyBoard(MyBuffer);
}




