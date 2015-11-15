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

class needle
{
    int LXo, LYo, LXi1, LYi1, LXi2, LYi2, lP, hP;
  
  public:
    void ChangePercentage(int L, int H)
    {
      lP = L;
      hP = H;
    }

    void drawNeedle(int val, int cx, int cy, int radius, int thick, int C1, int C2)
    {
      int rotate = map(val, lP, hP, 90, -270);

      int fix = map(radius - thick, 0, radius, 90, -thick);

      int Xo = cx + (-cos(rotate * Deg_to_rad) * radius);
      int Yo = cy + (sin(rotate * Deg_to_rad) * radius);

      int Xi1 = cx + (-cos((rotate - fix) * Deg_to_rad) * (radius - thick));
      int Xi2 = cx + (-cos((rotate + fix) * Deg_to_rad) * (radius - thick));

      int Yi1 = cy + (sin((rotate - fix) * Deg_to_rad) * (radius - thick));
      int Yi2 = cy + (sin((rotate + fix) * Deg_to_rad) * (radius - thick));

      if (rotate <= 90)
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
};

needle Hours, Minutes;

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x0);

  myGauge.Coords(400, 240, 150, 10);
  myGauge.ArcDegrees(90, -270);
  myGauge.Colors(WHITE, BLUE);
  myGauge.ChangePercentage(0, 60);
  myGauge.Draw(false);

  Minutes.ChangePercentage(0, 60);
  Hours.ChangePercentage(0, 12);
  
  Minutes.drawNeedle(0, 400, 240, 100, 90, GREEN, BLACK);
  Hours.drawNeedle(0, 400, 240, 135, 20, WHITE, BLACK);
}

void loop()
{
  static int S = 0, M = 0, H = 0, tog = true;
  static long prevTime = micros();
  
  if(micros() - prevTime >= 1000000UL)
  {  
    prevTime += 1000000UL;
    myGauge.Progress(S, false);
    S++;
  }
  
  if (S > 60)
  {
    S = 0;
    M++;
    Minutes.drawNeedle(M, 400, 240, 100, 90, GREEN, BLACK);
    if (M > 60)
    {
      M = 0;
      H++;
      Hours.drawNeedle(H, 400, 240, 135, 20, WHITE, BLACK);
    }
    tog = !tog;
    if (tog)
      myGauge.Colors(WHITE, BLUE);
    else
      myGauge.Colors(BLUE, BLUE);
  }
}

