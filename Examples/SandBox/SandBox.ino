// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);

Slider slideLF(&B, VERTICAL);
Slider slideLR(&B, VERTICAL);
Slider slideRF(&B, VERTICAL);
Slider slideRR(&B, VERTICAL);

struct touchData
{
  int X;
  int Y;
  byte Z;
};

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK);
  myGLCD.setColor(0xf800);
  myGLCD.fillCircle(400, 240, 100);
  myGLCD.setColor(0x001E);
  myGLCD.fillCircle(400, 240, 25);

  slideLF.Coords(550, 10, 610, 150);
  slideLR.Coords(550, 150, 610, 290);
  slideRF.Coords(650, 10, 710, 150);
  slideRR.Coords(650, 150, 710, 290);

  slideLF.Colors(ORANGE, BLUE, ROUNDED, FILL);
  slideLF.SetDirection(BTT);
  slideLF.SetRange(-255, 255, 1);
  slideLF.SetStartingValue(0);

  slideLR.Colors(BLUE, ORANGE, ROUNDED, FILL);
  slideLR.SetDirection(BTT);
  slideLR.SetRange(-255, 255, 1);
  slideLR.SetStartingValue(0);

  slideRF.Colors(ORANGE, BLUE, ROUNDED, FILL);
  slideRF.SetDirection(BTT);
  slideRF.SetRange(-255, 255, 1);
  slideRF.SetStartingValue(0);

  slideRR.Colors(BLUE, ORANGE, ROUNDED, FILL);
  slideRR.SetDirection(BTT);
  slideRR.SetRange(-255, 255 , 1);
  slideRR.SetStartingValue(0);

  slideLF.Draw();
  slideLR.Draw();
  slideRF.Draw();
  slideRR.Draw();
}

// How far down until the logic reverses?
#define RevFlip -20

void loop()
{
  static bool touched = false;
  touchData T = getTouchState(400, 240, 75);
  if (T.Z)
  {
    touched = true;
    myGLCD.setColor(0xf800);
    myGLCD.fillCircle(myGLCD.getDisplayXSize() / 2, myGLCD.getDisplayYSize() / 2, 100);
    myGLCD.setColor(0x001E);
    myGLCD.fillCircle(T.X, T.Y, 25);

    int X = map(T.X, (myGLCD.getDisplayXSize() / 2) - 75, (myGLCD.getDisplayXSize() / 2) + 75, 0, 1023);
    int Y = map(T.Y, (myGLCD.getDisplayYSize() / 2) + 75, (myGLCD.getDisplayYSize() / 2) - 75, 0, 1023);

    X -= 512; // make center equal 0
    Y -= 512;
    X /= 2; // divide range by 2 to give 0 - 255
    Y /= 2;
    
    Serial << "L:" << constrain((Y + X), -255, 255)  << " R:" << constrain((Y - X), -255, 255) << "\n";
    
    if (Y >= RevFlip)
    {
      slideLF.SetValue(constrain((Y + X), -255, 255)); slideRF.SetValue(constrain((Y - X), -255, 255));
      slideLR.SetValue(constrain((Y + X), -255, 255)); slideRR.SetValue(constrain((Y - X), -255, 255));
    }
    else
    {
      slideLF.SetValue(constrain((Y - X), -255, 255)); slideRF.SetValue(constrain((Y + X), -255, 255));
      slideLR.SetValue(constrain((Y - X), -255, 255)); slideRR.SetValue(constrain((Y + X), -255, 255));
    }

    slideLF.Update();
    slideLR.Update();
    slideRF.Update();
    slideRR.Update();
  }
  else
  {
    if (touched)
    {
      myGLCD.setColor(0xf800);
      myGLCD.fillCircle(myGLCD.getDisplayXSize() / 2, myGLCD.getDisplayYSize() / 2, 100);
      myGLCD.setColor(0x001E);
      myGLCD.fillCircle(myGLCD.getDisplayXSize() / 2, myGLCD.getDisplayYSize() / 2, 25);
      
      // return to center
      slideLF.SetValue(0); slideRF.SetValue(0);
      slideLR.SetValue(0); slideRR.SetValue(0);

      slideLF.Update();
      slideLR.Update();
      slideRF.Update();
      slideRR.Update();
      touched = false;
    }
  }
}

struct touchData getTouchState(int X, int Y, int Radius)
{
  touchData T;
  myTouch.read();
  int touchX = myTouch.getX();
  int touchY = myTouch.getY();
  int xc = (touchX > myGLCD.getDisplayXSize() ? 0 : touchX);
  int yc = (touchY > myGLCD.getDisplayYSize() ? 0 : touchY);

  T.X = xc;
  T.Y = yc;
  T.Z = ((pow((xc - X), 2) + pow((yc - Y), 2)) <= pow(Radius, 2));
  return T;
}
