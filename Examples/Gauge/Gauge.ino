// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ScaleFonts.h>
#include <TFT_ExtensionV2.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
TFT_ScaleFonts myFonts(&myGLCD);
Base B(&myGLCD, &myTouch);

Gauge myGauge(&B, &myFonts);

#define deg_to_rad 0.01745 + 3.14159265

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x5555);
  myGLCD.setColor(0xFFFF);
  myGLCD.setBackColor(0x5555);

  myGauge.Coords(400, 240, 150, 10);
  myGauge.ArcDegrees(225, -45);
  myGauge.TextRange(0, 150, 15, INSIDE, 2);
  myGauge.Colors(YELLOW, BLUE);
  myGauge.HeaderText("SPEED", 2);
  myGauge.FooterText("MPH", 2);
  myGauge.Draw();
  
  for(int i = 0; i <= 100; i++)
    myGauge.Progress(i);
  
  delay(2000);
  
  for(int i = 100; i >= 0; i--)
    myGauge.Progress(i);
  
}

void loop()
{

}
  
