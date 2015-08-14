#include <UTFT.h>
#include <UTouch.h>
#include <TFT_Extension.h> // Being used just for the colors.

#include <SPI.h>
#include <SD.h>
#include <avr/pgmspace.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

const char Mobile_KB[3][13] PROGMEM = {
  {0, 13, 10, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {1, 12, 9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
  {3, 10, 7, 'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
};

const char Mobile_NumKeys[3][13] PROGMEM = {
  {0, 13, 10, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {0, 13, 10, '-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char Mobile_SymKeys[3][13] PROGMEM = {
  {0, 13, 10, '[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {4, 9, 6, '_', '\\', '|', '~', '<', '>'}, //4
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char textLimit = 25;
char MyBuffer[textLimit];
int X_, Y_, _SX, _SY;

void Coords(int X, int Y, int SX, int SY)
{
  X_ = X; // 15
  Y_ = Y; // 100
  _SX = SX;
  _SY = SY; 
}

void MakeKB_Button(const char type[][13])
{
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0xffff);
  myGLCD.setBackColor(0x0);
  for (int y = 0; y < 3; y++)
  {
    int ShiftRight = X_ * pgm_read_byte(&(type[y][0]));
    for (int x = 3; x < 13; x++)
    {
      if (x >= pgm_read_byte(&(type[y][1]))) break;
      int xp = X_ + (30 * (x - 3)) + ShiftRight;
      int yp = Y_ + (30 * y);
      drawButton(xp, yp, xp + 20, yp + 25); // this will draw the button on the screen by so many pixels
      myGLCD.setColor(0xffff);
      myGLCD.printChar(char(pgm_read_byte(&(type[y][x]))), xp + 2, yp + 5);
    }
  }
  //ShiftKey
  drawButton(X_, Y_ + 60, X_+35, Y_ + 60+25);
  myGLCD.print("^",((X_*2) + 35)/2 - 8,  (((Y_ +60)*2) + 25)/2 - 4);

  //Special Characters
  drawButton(15, 190, 15+35, 190+25);
  myGLCD.print("SP",21-3, 195);

  //BackSpace
  drawButton(270, 160, 270+35, 160+25);
  myGLCD.print("BS",276-3, 165);

  //Return
  drawButton(270, 190, 270+35, 190+25);
  myGLCD.print("RT",276-3, 195);

  //Spacebar
  drawButton(60, 190, 60+200, 190+25);
  myGLCD.print("SPACE BAR",105-3, 195);
}

void drawButton(int x, int y, int w, int h)
{
  // white
  myGLCD.setColor(0xffff);
  myGLCD.drawRoundRect(x, y, w, h);// outter button color
  myGLCD.setColor(0xffff);
}

void GetKeyPress(char * textBuffer)
{
  char key = 0;
  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
  static char bufIndex = 0;

  if (myTouch.dataAvailable())
  {
    //ShiftKey
    if (getTouchArea(15, 160, 15+35, 160+25))
    {
      shift = !shift;
      delay(200);
    }

    //Special Characters
    if (getTouchArea(15, 190, 15+35, 190+25))
    {
      special = !special;
      delay(200);
    }

    if (special != lastSp || shift != lastSh)
    {
      if (special)
      {
        if (shift)
        {
          myGLCD.fillScr(0x0);
          MakeKB_Button(Mobile_SymKeys);
        }
        else
        {
          myGLCD.fillScr(0x0);
          MakeKB_Button(Mobile_NumKeys);
        }
      }
      else
      {
        myGLCD.fillScr(0x0);
        MakeKB_Button(Mobile_KB);
        myGLCD.setColor(0xffff);
      }

      if (special)
        myGLCD.setColor(0x0FF0);
      else
        myGLCD.setColor(0xFFFF);

      myGLCD.print(F("SP"),21-3, 195);

      if (shift)
        myGLCD.setColor(0x0FF0);
      else
        myGLCD.setColor(0xffff);

      myGLCD.printChar('^',27-3, 168);

      lastSh = shift;

      lastSp = special;
      lastSh = shift;
    }

    for (int y = 0; y < 3; y++)
    {
      int ShiftRight;
      if (special)
      {
        if (shift)
          ShiftRight = 15 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
        else
          ShiftRight = 15 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
      }
      else
        ShiftRight = 15 * pgm_read_byte(&(Mobile_KB[y][0]));

      for (int x = 3; x < 13; x++)
      {
        if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) break;

        if (getTouchArea(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 15 + (30 * (x - 3)) + ShiftRight+20, 100 + (30 * y)+25)) // this will draw the button on the screen by so many pixels
        {
          if (bufIndex < (textLimit - 1))
          {
            delay(200);

            if (special)
            {
              if (shift)
                textBuffer[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
              else
                textBuffer[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
            }
            else
              textBuffer[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

            bufIndex++;
          }
          break;
        }
      }
    }

    //Spacebar
    if (getTouchArea(60, 190, 60+200, 190+25))
    {
      textBuffer[bufIndex++] = ' ';
      delay(200);
    }

    //BackSpace
    if (getTouchArea(270, 160, 270+35, 160+25))
    {
      if ((bufIndex) > 0)
        bufIndex--;
      textBuffer[bufIndex] = 0;
      myGLCD.setBackColor(BLUE);
      myGLCD.print(F("                          "),15, 80);
      delay(200);
    }

    //Return
    if (getTouchArea(270, 190, 270+35, 190+25))
    {
      Serial.println(textBuffer);
      while (bufIndex > 0)
      {
        bufIndex--;
        textBuffer[bufIndex] = 0;
      }

      myGLCD.setBackColor(BLUE);
      myGLCD.print(F("                         "),15, 80);
    }
  }
  myGLCD.setColor(0xffff);
  myGLCD.print(textBuffer,15, 80);
}


void setup(void)
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  myGLCD.fillScr(BLACK);
  Coords(15,100,0,0);
  MakeKB_Button(Mobile_KB);
  
}

void loop()
{
  // See if there's any  touch data for us
  GetKeyPress(MyBuffer);
}


int getTouchArea(int X1, int Y1, int X2, int Y2)
{
  myTouch.read();
  int touchX = myTouch.getX();
  int touchY = myTouch.getY();
  int xc = (touchX > myGLCD.getDisplayXSize() ? 0 : touchX);
  int yc = (touchY > myGLCD.getDisplayYSize() ? 0 : touchY);
  Serial.print(xc); Serial.print('\t'); Serial.println(yc);
  return ((xc >= X1) && (xc <= X2) && (yc >= Y1) && (yc <= Y2));
}
