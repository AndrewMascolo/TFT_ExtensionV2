#include <UTFT.h>

UTFT    myGLCD(CTE70, 25, 26, 27, 28);

//=====================COLOR_PALLET==========================
#define BLACK   0x0
#define LIGHT_RED	0xFD14
#define RED     0xF800
#define DARK_RED    0x6000
#define LIGHT_ORANGE 0xFF52
#define ORANGE  0xFD00
#define DARK_ORANGE 0xFA80
#define LIGHT_YELLOW 0xFFF4
#define YELLOW  0xD7E0
#define DARK_YELLOW 0xCE40
#define LIGHT_GREEN 0xB7F6
#define GREEN   0x07E0
#define DARK_GREEN 0x0320
#define LIGHT_BLUE 0xA51F
#define BLUE    0x001F
#define DARK_BLUE 0x000C
#define PURPLE  0xF81F
#define CYAN    0x07FF
#define GREY    0x8410
#define WHITE   0xFFFF

// Technically a 16 bit image.

// color, length... 0xFFFFFFFF = transparent
long test[][15] =
{
  {7, RED, 1, 0xFFFFFFFF, 4, GREEN, 2, YELLOW, 3, RED, 1, CYAN, 2, PURPLE, 3},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
  {6, WHITE, 3, GREEN, 3, 0xFFFFFFFF, 3, RED, 3, ORANGE, 2, PURPLE, 2},
  {4, GREEN, 5, YELLOW, 3, ORANGE, 3, RED, 5},
};

long test2[][8] =
{
  {16}, // how many rows of colors. Also tells how many rows to skip to find the length
  {7, RED, 0xFFFFFFFF, GREEN, YELLOW, RED, CYAN, PURPLE}, // number of colors in this line, color
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {6, WHITE, GREEN, 0xFFFFFFFF, RED, ORANGE, PURPLE},
  {4, GREEN, YELLOW, ORANGE, RED},
  {1, 4, 2, 3, 1, 2, 3}, // length of lines
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5},
  {3, 3, 3, 3, 2, 2},
  {5, 3, 3, 5}
};

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.fillScr(0x0);
  //Serial.println(sizeof(test)/sizeof(test[0]));
  long time = micros();
  TestIMG(20, 20, 5);
  Serial.println(micros() - time);

  delay(10); // take a break

  time = micros();
  TestIMG2(20, 120, 5);
  Serial.println(micros() - time);
}

void loop()
{

}

void TestIMG2(int x, int y, int scale)
{
  for (int i = 1, j = test2[0][0]; i <= j; i++)
    for (int r = 0 + (scale * i); r < scale * (i + 1); r++)
      for (int M = 0, c = 0, S = test2[i][0]; c < S; c++)
      {
        //Serial.println(S);
        byte length = test2[i + j][c] * scale;
        if (test2[i][c + 1] != 0xFFFFFFFF) // if not transparent
        {
          myGLCD.setColor(test2[i][c + 1]);
          myGLCD.drawHLine(M + x, r + y, length);
        }
        M += length;
      }
}

void TestIMG( int x,  int y,  int scale)
{
  for (int i = 0, rows = sizeof(test) / sizeof(test[0]); i < rows; i++)
    for (int r = 0 + (scale * i); r < scale * (i + 1); r++)
      for (int M = 0, c = 1, S = (test[i][0] * 2); c < S; c += 2)
      {
        byte length = test[i][c + 1] * scale;
        if (test[i][c] != 0xFFFFFFFF) // if not transparent
        {
          myGLCD.setColor(test[i][c]);
          myGLCD.drawHLine(M + x, r + y, length);
        }
        M += length;
      }
}




