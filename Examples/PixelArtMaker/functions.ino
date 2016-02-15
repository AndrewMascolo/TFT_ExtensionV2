void loadingScreen()
{
  int DY = myGLCD.getDisplayYSize() / 2;
  myGLCD.setColor(WHITE);
  if (!SD.begin(53))
  {
    myGLCD.print("SD Card initialization failed!", CENTER, DY + 35);
    return;
  }
  myGLCD.print("SD Card initialized.", CENTER, DY + 35);
}

void drawGrid(struct Canvas canvas, byte cols, byte rows)
{
  myGLCD.setColor(canvas.FC);
  myGLCD.drawRect(canvas.x - 1, canvas.y - 1, canvas.x + canvas.w + 1, canvas.y + canvas.h + 1);
  myGLCD.setColor(canvas.BC);
  myGLCD.fillRect(canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h);

  int Xspace = (canvas.w / rows);
  int Yspace = (canvas.h / cols);

  myGLCD.setColor(canvas.FC);
  for ( int box = 0; box < (rows * cols); box++)
  {
    myGLCD.drawRect
    (
      (box % cols)*Xspace + canvas.x, 
      (box / rows)*Yspace + canvas.y, 
      (box % cols)*Xspace + Xspace + canvas.x, 
      (box / rows)*Yspace + Yspace + canvas.y
    );
    Pixels[(box % rows)] = 0;
  }
}

#if TYPE == MONO
void drawPixels(struct Canvas canvas, byte cols, byte rows, byte tog, int color)
{
  static short box = 0;
  static short Xspace = (canvas.w / rows);
  static short Yspace = (canvas.h / cols);

  Pixel.Coords
  (
    (box % cols)*Xspace + canvas.x, 
    (box / rows)*Yspace + canvas.y, 
    (box % cols)*Xspace + Xspace + canvas.x, 
    (box / rows)*Yspace + Yspace + canvas.y
  );
  if (Pixel.Touch(false))
  {
    myGLCD.setColor(tog ? color : canvas.BC);
    myGLCD.fillRect
    (
      (box % cols)*Xspace + canvas.x + 1, 
      (box / rows)*Yspace + canvas.y + 1, 
      (box % cols)*Xspace + Xspace + canvas.x - 1, 
      (box / rows)*Yspace + Yspace + canvas.y - 1
    );
    if (tog)
      bitSet(Pixels[box / rows], rows - 1 - (box % rows));
    else
      bitClear(Pixels[box / rows], rows - 1 - (box % rows));
  }

  if ((box % rows) == 0)
  {
    myGLCD.setColor(WHITE);
    myGLCD.setBackColor(BLACK);

    char buffer[NUM] = {'\0'};
    Dec2Hex(Pixels[box / rows], buffer, SIZE);
    myGLCD.print(buffer, canvas.w + canvas.x + 10, canvas.y + Yspace * (box / rows));
  }

  box++;
  if (box >= (rows * cols))
    box = 0;
}
#endif

#if TYPE == COLOR
void drawPixels(struct Canvas canvas, byte cols, byte rows, byte tog, int color)
{
  static int box = 0;
  static int Xspace = (canvas.w / rows);
  static int Yspace = (canvas.h / cols);

  Pixel.Coords
  (
    (box % cols)*Xspace + canvas.x, 
    (box / rows)*Yspace + canvas.y, 
    (box % cols)*Xspace + Xspace + canvas.x, 
    (box / rows)*Yspace + Yspace + canvas.y
  );
  if (Pixel.Touch(false))
  {
    if (color == canvas.BC) tog = false;
    Pixels[box] = (tog ? color : canvas.BC);
    myGLCD.setColor(Pixels[box]);
    myGLCD.fillRect
    (
      (box % cols)*Xspace + canvas.x + 1, 
      (box / rows)*Yspace + canvas.y + 1, 
      (box % cols)*Xspace + Xspace + canvas.x - 1, 
      (box / rows)*Yspace + Yspace + canvas.y - 1
    );
  }

  box++;
  if (box >= (rows * cols))
    box = 0;
}
#endif

void Dec2Hex(unsigned short D, char * buf, byte Size)
{
  byte i = 0;
  short val;
  while (i != (Size * 2))
  {
    val = (D % 16);
    buf[(Size * 2) - 1 - i] = ((val < 10) ? val + '0' : (val + 'A' - 10)) ;
    D /= 16;
    i++;
  }
}

void drawLoadedImg(struct Canvas canvas, short * img, byte cols, byte rows, byte G)
{
  myGLCD.setColor(canvas.BC);
  myGLCD.fillRect(canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h);

  short Xspace = canvas.w / cols;
  short Yspace = canvas.h / rows;

  for (int box = 0; box < (rows * cols); box++)
  {
#if TYPE == COLOR
    myGLCD.setColor(img[box]);
#else
    myGLCD.setColor((bitRead(img[box / rows], (8*SIZE - 1) - (box % cols)) ? GREEN : canvas.BC));
#endif

    myGLCD.fillRect
    (
      (box % cols)*Xspace + canvas.x, 
      (box / rows)*Yspace + canvas.y, 
      (box % cols)*Xspace + Xspace + canvas.x, 
      (box / rows)*Yspace + Yspace + canvas.y
    );
    
    if (G)
    {
      myGLCD.setColor(canvas.FC);
      myGLCD.drawRect
      (
        (box % cols)*Xspace + canvas.x, 
        (box / rows)*Yspace + canvas.y, 
        (box % cols)*Xspace + Xspace + canvas.x, 
        (box / rows)*Yspace + Yspace + canvas.y
      );
    }
  }
}

short HexChar2Dec(char Ch)
{
  if (IsWithin(Ch, 'A', 'Z') )
    return (Ch - 'A') + 10;
  else if (IsWithin(Ch, 'a', 'z'))
    return (Ch - 'a') + 10;
  else
    return (Ch - '0');
}

/*
   This filter function is designed to read the image file's contents within the curly brackets {...}
   I had to make my own function to read the data because the PROGMEM functions were not returning the correct results
   and they were not flexable to handle both types of file formats, (MONO / COLOR, BYTES / INTS)
*/
int Filter(File myImg)
{
  char currentChar = myImg.read();
  if (IsWithin(currentChar, '0', '9') | IsWithin(currentChar, 'a', 'f') | IsWithin(currentChar, 'A', 'F'))
  {
    char nextChar = myImg.read();
    if ((currentChar == '0') && (nextChar == 'x' || nextChar == 'X'))
    {
      //Serial.println(F("found 0x"));
      char search = 0;
      short val = 0;
      while (1)
      {
        search = myImg.read();
        //Serial.println(search);
        if (search == ',' || search == ' ')
        {
          //Serial.println(F("Found comma or space"));
          break;
        }
        val = (val * 16) + HexChar2Dec(search);
      }
      return val;
    }
    else if ((nextChar == ' ') && (myImg.read() == ','))
    {
      //Serial.println(F("Found _,"));
      Filter(myImg);
    }
    else if (nextChar == ',')
    {
      //Serial.println(F("Found ,"));
      Filter(myImg);
    }
  }
  else if ((currentChar == '}') || (currentChar == ';'))
    return 0;
  else Filter(myImg);
}
