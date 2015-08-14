// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
Box Main(&B), FrontLights(&B), RearLights(&B);
ProgressBar Voltage_Bar(&B, VERTICAL), Current_Bar(&B, VERTICAL);

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);

  Voltage_Bar.Coords(600, 50, 690, 300);
  Voltage_Bar.Attributes(ROUNDED, FILL);  // FIRST, must set coordinates before setting Range and optional starting value
  Voltage_Bar.Colors(RED, YELLOW, GREEN, BLACK ); // Optional but good to have some color
  Voltage_Bar.Percentages(80, 50, 0);
  Voltage_Bar.SetDirection(TTB); // bottom to top   // Optional
  Voltage_Bar.SetRange(0, 1023);                 // SECOND, Must be set after coordinates
  Voltage_Bar.SetStartingValue(0);                // THIRD (Optional, but must be set after range)
  Voltage_Bar.Text("VOLTS", WHITE, Big, TOP);
  Voltage_Bar.ProgressText(WHITE, Big, BOTTOM);
  Voltage_Bar.Draw();

  Current_Bar.Coords(700, 50, 790, 300);
  Current_Bar.Attributes(ROUNDED, FILL);  // FIRST, must set coordinates before setting Range and optional starting value
  Current_Bar.Colors(RED, YELLOW, GREEN, BLACK ); // Optional but good to have some color
  Current_Bar.Percentages(75, 50, 0);
  Current_Bar.SetDirection(TTB); // bottom to top   // Optional
  Current_Bar.SetRange(0, 50);                 // SECOND, Must be set after coordinates
  Current_Bar.SetStartingValue(0);  // THIRD (Optional, but must be set after range)
  Current_Bar.Text("AMPS", WHITE, Big, TOP);
  Current_Bar.ProgressText(WHITE, Big, BOTTOM);
  Current_Bar.Draw();

  Main.Coords(20, 350, 250, 450);
  Main.Colors(GREEN, RED, FILL, ROUNDED);
  Main.Text(WHITE, Big, "CONNECTED", "DISCONNECTED");
  Main.HeadFootText(WHITE, Big, "MAIN POWER", "");
  Main.Draw();

  FrontLights.Coords(270, 350, 350, 450);
  FrontLights.Colors(GREEN, RED, FILL, ROUNDED);
  FrontLights.Text(WHITE, Big, "ON", "OFF");
  FrontLights.HeadFootText(WHITE, Big, "Front", "");
  FrontLights.Draw();

  RearLights.Coords(370, 350, 450, 450);
  RearLights.Colors(GREEN, RED, FILL, ROUNDED);
  RearLights.Text(WHITE, Big, "ON", "OFF");
  RearLights.HeadFootText(WHITE, Big, "Rear", "");
  RearLights.Draw();

  myGLCD.setColor(WHITE);
  for (int i = 0; i <= 1024; i += 16) // demo voltage
  {
    Voltage_Bar.SetValue(i);
    Voltage_Bar.Progress();
  }

  for (int i = 0; i <= 50; i ++) // demo current
  {
    Current_Bar.SetValue(i);
    Current_Bar.Progress();
    delay(100);
  }

  //Solar Panels
  drawArc(300, 200, 250, 150, 0, 90, WHITE, false);
  drawArc(300, 200, 250, 50, 0, 360, WHITE, false);
  ShowDegrees(300, 200, 250, 150, 45, RED);  // currently set at 45 degrees 
  ShowDegrees(300, 200, 250, 50, 90, GREEN); // currently set to 90 degrees
}

void loop()
{
  Main.Latch();
  FrontLights.Latch();
  RearLights.Latch();
}


//====================OTHER_FUNCTIONS==================================================================
void drawArc(int cx, int cy, int radius_W, int radius_H, int start, int stop, word color, bool lines)
{
  stop -= 1;
  if (start > stop)
    Swap(start, stop);

  int XI, YI, XR, YR, XL, YL;

  myGLCD.setColor(color);
  for (float rotate = start - 90; rotate <= stop - 90; rotate += 0.5)
  {
    XI = cx + sin(rotate * 0.01745 + 3.14159) * radius_W;
    YI = cy + cos(rotate * 0.01745 + 3.14159) * radius_H;
    if (rotate == 0) XI -= 1; // this gets rid of the extra pixel at 0 deg

    if (rotate == start) {
      XR = XI;
      YR = YI;
    }
    if (rotate == stop) {
      XL = XI;
      YL = YI;
    }
    myGLCD.drawPixel(XI, YI);
  }
  if (stop - start >= 359)
    lines = false;

  if (lines)
  {
    myGLCD.drawLine(cx, cy, XR, YR); // draw the line for the bottom of the arc
    myGLCD.drawLine(cx, cy, XL, YL); // =======================================
  }
}

void ShowDegrees(int cx, int cy, int radius_W, int radius_H, int D, int color)
{
  int XI, YI;
  D -= 90;
  myGLCD.setColor(color);

  XI = cx + sin(D * 0.01745 + 3.14159) * radius_W;
  YI = cy + cos(D * 0.01745 + 3.14159) * radius_H;
  if (D == 0) XI -= 1; // this gets rid of the extra pixel at 0 deg
  myGLCD.drawLine(cx, cy, XI, YI); // draw the line for the bottom of the arc
  myGLCD.fillCircle(XI, YI, 5);
}

