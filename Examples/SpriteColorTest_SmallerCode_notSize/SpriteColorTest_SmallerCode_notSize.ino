// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);

const unsigned long Mario[] =
{
  0x00000222, 0x22000000,
  0x00002222, 0x22222000,
  0x00007773, 0x31300000,
  0x00073733, 0x31333000,
  0x00073773, 0x33133300,
  0x00007333, 0x31111000,
  0x00000333, 0x33300000,
  0x00002252, 0x25220000,
  0x00022252, 0x25222000,
  0x00222255, 0x55222200,
  0x00332535, 0x53523300,
  0x00333555, 0x55533300,
  0x00335555, 0x55553300,
  0x00005550, 0x05550000,
  0x00077700, 0x00777000,
  0x00777700, 0x00777700
};

const unsigned int colors[] =
{
  0x0000, //transparent
  0x0001, //black
  0xF800, //red
  0xFF00, //yellow
  0x07E0, // green
  0x001F, // blue
  0x0FF0,
  0xA000,
  0xAAAA,
  0x00FF,
  0x5455,
  0x8888,
  0x7412,
  0x8520,
  0x5555,
  0xFFFF
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

