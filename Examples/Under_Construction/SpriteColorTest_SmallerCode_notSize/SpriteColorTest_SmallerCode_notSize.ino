// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);

const unsigned long Mario[] =
{
  0x00000222, 0x22000000,
  0x00002222, 0x22222000,
  0x00009994, 0x41400000,
  0x00094944, 0x41444000,
  0x00094994, 0x44144400,
  0x00009444, 0x41111000,
  0x00000444, 0x44400000,
  0x00002262, 0x26220000,
  0x00022262, 0x26222000,
  0x00222266, 0x66222200,
  0x00442646, 0x64624400,
  0x00444666, 0x66644400,
  0x00446666, 0x66664400,
  0x00006660, 0x06660000,
  0x00099900, 0x00999000,
  0x00999900, 0x00999900
};

const unsigned int colors[] =
{
  0x0000, //transparent
  0x0001, //black
  0xF800, //red
  0xFBE0, //orange
  0xFF00, //yellow
  0x07E0, //green
  0x001F, //blue
  0x4810, //indigo
  0xEC1D, //violet
  0xA000,
  0x0FF0, // light green
  0x0FFF, // cyan
  0x5455,
  0x8888, // dark red
  0x7412,
  0xFFFF  // white
};

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0x1111);
  //myGLCD.setColor(0x1111);

  drawSprite(Mario, 0, 0, 20);
}


void loop()
{

}

void drawSprite(const unsigned long * img, int x, int y, int Ssize)
{
  for (int i = 0; i <= 0xFF; i++)
  {
    unsigned int C = ( 0x0F & (img[i / 8] >> (28 - (4 * (i % 8)))) );
    myGLCD.setColor(colors[C]);

    if (C == 0) // 0x0 = transparent, if you want black then use 0x01
      continue;
    myGLCD.fillRect(x + Ssize * (i % 16) + 1, y + Ssize * (i / 16) + 1, x + Ssize * (i % 16) + Ssize, y + Ssize * (i / 16) + Ssize);
  }
}

