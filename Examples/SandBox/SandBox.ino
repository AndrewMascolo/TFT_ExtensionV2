// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
Box Pixel(&B);

struct Canvas
{
  void setCoords(int X, int Y, int W, int H, word fore, word back)
  {
    x = X; y = Y; w = W; h = H; FC = fore; BC = back;
  }
  int x, y, w, h;
  word FC, BC;
};

Canvas Grid;

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(-1);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  
  Grid.setCoords(10, 10, 400, 460, BLUE, BLACK);
  drawGrid(Grid, 16, 16);
}

void loop()
{
  drawPixels(Grid, 16, 16);
}

void drawGrid(struct Canvas canvas, byte cols, byte rows)
{
  myGLCD.setColor(canvas.BC);
  myGLCD.fillRect(canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h);

  myGLCD.setColor(canvas.FC);
  for( int box = 0; box < (rows*cols); box++)
  {
    myGLCD.drawRect((box%16)*rows+canvas.x, (box/16)*cols+canvas.y,(box%16)*rows+canvas.x+rows, (box/16)*cols+canvas.y+col);
    delay(1000);
  }
}

void drawPixels(struct Canvas canvas, byte cols, byte rows)
{
  for ( int r = canvas.y; r <= (canvas.h + canvas.y); r += (canvas.h / rows))
    for (int c = canvas.x; c < (canvas.w + canvas.x); c += (canvas.w / cols))
    {
      Pixel.Coords(r, c, r + (canvas.h / rows), c + (canvas.w / cols));
      if (Pixel.Touch(false))
      {
        myGLCD.setColor(GREEN);
        myGLCD.fillRect(r, c, r + (canvas.h / rows), c + (canvas.w / cols));
        break;
      }
      
    }
}
