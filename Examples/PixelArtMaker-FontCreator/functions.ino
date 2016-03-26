void loadingScreen()
{
  int DY = myGLCD.getDisplayYSize() / 2;
  myGLCD.setColor(WHITE);
  myGLCD.print("LOADING FOR: 8BIT COLOR SPRITE", CENTER, DY);
  if (!SD.begin(53))
  {
    myGLCD.print("SD Card initialization failed!", CENTER, DY + 35);
    return;
  }
  myGLCD.print("SD Card initialized.", CENTER, DY + 35);
}

void drawGrid(struct Canvas canvas, byte cols, byte rows, byte index)
{
  myGLCD.setColor(canvas.FC);
  myGLCD.drawRect(canvas.x - 1, canvas.y - 1, canvas.x + canvas.w + 1, canvas.y + canvas.h + 1);
  myGLCD.setColor(canvas.BC);
  myGLCD.fillRect(canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h);

  int Xspace = (canvas.w / cols);
  int Yspace = (canvas.h / rows);

  myGLCD.setColor(canvas.FC);
  for ( int box = 0; box < (rows * cols); box++)
  {
    myGLCD.drawRect
    (
      (box % cols)*Xspace + canvas.x,
      (box / cols)*Yspace + canvas.y,
      (box % cols)*Xspace + Xspace + canvas.x,
      (box / cols)*Yspace + Yspace + canvas.y
    );
    Pixels[box / cols] = 0;
  }
}

void drawPixels(struct Canvas canvas, byte cols, byte rows, byte tog, int color, int index)
{
  static short box = 0;
  static short Xspace = (canvas.w / cols);
  static short Yspace = (canvas.h / rows);

  Pixel.Coords
  (
    (box % cols)*Xspace + canvas.x,
    (box / cols)*Yspace + canvas.y,
    (box % cols)*Xspace + Xspace + canvas.x,
    (box / cols)*Yspace + Yspace + canvas.y
  );
  if (Pixel.Touch(false))
  {
    myGLCD.setColor(tog ? color : canvas.BC);
    myGLCD.fillRect
    (
      (box % cols)*Xspace + canvas.x + 1,
      (box / cols)*Yspace + canvas.y + 1,
      (box % cols)*Xspace + Xspace + canvas.x - 1,
      (box / cols)*Yspace + Yspace + canvas.y - 1
    );
    if (tog)
      bitSet(Pixels[box / cols], cols - 1 - (box % cols));
    else
      bitClear(Pixels[box / cols], cols - 1 - (box % cols));
  }

#if 0  // see hex values per row
  if ((box % cols) == 0)
  {
    myGLCD.setColor(WHITE);
    myGLCD.setBackColor(BLACK);

    char buffer[NUM] = {'\0'};
    Dec2Hex(Pixels[box / cols], buffer, SIZE);
    myGLCD.print(buffer, canvas.w + canvas.x + 10, canvas.y + Yspace * (box / cols));
  }
#endif

  box++;
  if (box >= (rows * cols))
    box = 0;
}

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
    myGLCD.setColor((bitRead(img[(box / cols)], (8 * SIZE - 1) - (box % cols)) ? GREEN : canvas.BC));

    myGLCD.fillRect
    (
      (box % cols)*Xspace + canvas.x,
      (box / cols)*Yspace + canvas.y,
      (box % cols)*Xspace + Xspace + canvas.x,
      (box / cols)*Yspace + Yspace + canvas.y
    );

    if (G)
    {
      myGLCD.setColor(canvas.FC);
      myGLCD.drawRect
      (
        (box % cols)*Xspace + canvas.x,
        (box / cols)*Yspace + canvas.y,
        (box % cols)*Xspace + Xspace + canvas.x,
        (box / cols)*Yspace + Yspace + canvas.y
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
   I had to make my own function to read the data because you can't use the PROGMEM read functions on an SD card
*/
int Filter(File myImg)
{
  char currentChar = myImg.read();
  if (IsWithin(currentChar, '0', '9'))
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

        if (search == ',' || search == ' ') break;
        val = (val * 16) + HexChar2Dec(search);
      }

      return val;
    }
  }
  else if ((currentChar == '}') && (myImg.read() == ';')) // end of data
    return 0;
  else if ((currentChar == '/') && (myImg.read() == '/')) // comment
  {
    char C = 0;
    while (1)
    {
      C = myImg.read();
      //Serial << char(C);
      if (( C == 10) || (C == 13))
      {
        //Serial.println();
        break;
      }
    }
    //Serial.println("End of Comment");
    Filter(myImg);
  }
  else  Filter(myImg);
}
