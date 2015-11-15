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
Triangle Tri(&B);//TButton2(&B);
Gauge myGauge(&B, &myFonts);

#define Deg_to_rad 0.01745 + 3.14159265

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x0);
  //myTouch.InitTouch(LANDSCAPE);
  //myTouch.setPrecision(PREC_HI);

  myGauge.Coords(400, 240, 150, 10);
  myGauge.ArcDegrees(90, -270);
  //myGauge.TextRange(0, 150, 15, INSIDE, 2);
  myGauge.Colors(WHITE, BLUE);
  //myGauge.HeaderText("SPEED", 2);
  //myGauge.FooterText("MPH", 2);
  myGauge.Draw(false);

  //  TButton2.Coords(250,210,40);
  //  TButton2.Colors(GREEN, RED, NOFILL);
  //  TButton2.Draw();
  //Tri.fillTriangle(400,50,350,200,450,200);
}

void loop()
{
  //TButton2.Touch();
  static byte i = 0, tog = true;
  myGauge.Progress(i, false);
  drawNeedle(i, 400, 240, 135, 20, WHITE, BLACK);
  drawNeedle(i, 400, 240, 100, 90, GREEN, BLACK);
  i++;
  if (i > 100)
  {
    i = 0;
    tog = !tog;
    if(tog)
      myGauge.Colors(WHITE, BLUE);
    else
      myGauge.Colors(BLUE, BLUE);
  }

  delay(100);

}

void drawNeedle(int val, int cx, int cy, int radius, int thick, int C1, int C2)
{
  static int LXo, LYo, LXi1, LYi1, LXi2, LYi2
  int rotate = map(val, 0, 100, 87, -272);
  int fix = map(radius-thick, 0, radius, 90, -thick);

  int Xo = cx + (-cos(rotate * Deg_to_rad) * radius);
  int Yo = cy + (sin(rotate * Deg_to_rad) * radius);

  int Xi1 = cx + (-cos((rotate - fix) * Deg_to_rad) * (radius - thick));
  int Xi2 = cx + (-cos((rotate + fix) * Deg_to_rad) * (radius - thick));

  int Yi1 = cy + (sin((rotate - fix) * Deg_to_rad) * (radius - thick));
  int Yi2 = cy + (sin((rotate + fix) * Deg_to_rad) * (radius - thick));
  
  if(rotate < 87)
  {
    myGLCD.setColor(C2);
    Tri.drawTriangle( LXo, LYo, LXi1, LYi1, LXi2, LYi2);
  }
  
  myGLCD.setColor(C1);
  Tri.drawTriangle( Xo, Yo, Xi1, Yi1, Xi2, Yi2);
  
  LXo = Xo;
  LYo = Yo;
  LXi1 = Xi1;
  LYi1 = Yi1;
  LXi2 = Xi2;
  LYi2 = Yi2;

}
