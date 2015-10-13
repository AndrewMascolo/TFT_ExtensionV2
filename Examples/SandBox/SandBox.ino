// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <UTFT_ScaleFonts.h>
#include <TFT_Goodies.h>

extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

//UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
UTFT_ScaleFonts myFonts(&myGLCD);
Gauge myGauge(&myGLCD);

#define deg_to_rad 0.01745 + 3.14159265

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x00);
  myGLCD.setColor(0xFFFF);

  myGauge.Coords(160, 150, 120, 20);
  myGauge.Range(180, 0, 6);
  myGauge.Colors(YELLOW, BLUE);

  //DrawArc(160, 120, 160, 10, 20, 180, 0);
  //  for (byte i = 0; i <= 100; i += 1)
  //  {
  //    myGauge.drawGauge(i);
  //    myFonts.printNumI(i*1.8F, 90, 100, 3, 3);
  //  }
  //  gauge(400, 240, 150, 10, 30, 50);
  //  gauge(400, 240, 100, 10, 0, 50);
  //myGLCD.print("Hello", 50, 50);
  myGLCD.setColor(0xFFFF);
  //myFonts.printNumI(100, 50, 10, 0, 10);

  unsigned long time = micros();
  printChar('A', 400, 240, 1);
  Serial.println(micros() - time);
  delay(1);
  time = micros();
  myGLCD.printChar('A', 50, 200);
  Serial.println(micros() - time);
  //printChar('A', 400, 240, 1);
  //myGLCD.printChar('C', 380, 240);
}

void loop()
{

}


void printChar(byte c, int x, int y, int S)
{
  word temp;
  unsigned char ch;

  cbi(myGLCD.P_CS, myGLCD.B_CS);

#if 0
  temp = ((c - myGLCD.cfont.offset) * ((myGLCD.cfont.x_size / 8) * myGLCD.cfont.y_size)) + 4;

  for (int Y = 0; Y < ((myGLCD.cfont.x_size / 8) * myGLCD.cfont.y_size); Y += (myGLCD.cfont.x_size / 8) )
  {
    myGLCD.setXY(x, y + (Y / (myGLCD.cfont.x_size / 8)), x + myGLCD.cfont.x_size, y + (Y / (myGLCD.cfont.x_size / 8)) );
    for (int X = 0; X < (myGLCD.cfont.x_size); X++)
    {
      ch = pgm_read_byte(&( myGLCD.cfont.font[temp + (X / 8)] ) );

      if (ch & (1 << (7 - (X >= 8 ? X - 8 : X) - 1)) )
        myGLCD.setPixel((myGLCD.fch << 8) | myGLCD.fcl);
      else
        myGLCD.setPixel((myGLCD.bch << 8) | myGLCD.bcl);
    }
    temp += (myGLCD.cfont.x_size / 8);
  }
#endif

#if 1
  temp = ((c - myGLCD.cfont.offset) * ((myGLCD.cfont.x_size / 8) * myGLCD.cfont.y_size)) + 4;

  for (int Y = 0; Y < ((myGLCD.cfont.x_size / 8) * myGLCD.cfont.y_size)*S; Y += (myGLCD.cfont.x_size / 8) * S)
  {
    for (byte s = 0; s < S; s++)
    {
      myGLCD.setXY(x, y + s + (Y / (myGLCD.cfont.x_size / 8)), x + myGLCD.cfont.x_size * S - 1, y + s + (Y / (myGLCD.cfont.x_size / 8)));

      for (int xx = 0; xx < (myGLCD.cfont.x_size / 8)*S; xx++)
      {
        ch = pgm_read_byte(&myGLCD.cfont.font[temp + xx]);
        for (byte i = 0; i < 8 * S; i++)
        {
          if ((ch & (1 << 7 - i / S)) != 0)
          {
            myGLCD.setPixel((myGLCD.fch << 8) | myGLCD.fcl);
          }
          else
          {
            myGLCD.setPixel((myGLCD.bch << 8) | myGLCD.bcl);
          }
        }
      }
    }
    temp += (myGLCD.cfont.x_size / 8);
  }

#endif

  sbi(myGLCD.P_CS, myGLCD.B_CS);
  myGLCD.clrXY();
}


