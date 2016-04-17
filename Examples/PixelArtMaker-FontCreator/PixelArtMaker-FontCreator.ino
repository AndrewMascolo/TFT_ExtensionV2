// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <SPI.h>
#include <SD.h>

// do not modify these four
#define MONO  1
#define COLOR 2
#define BYTES 1
#define INTS  2

//================SKETCH SETTINGS FOR FONT/SPRITE FILE====================
#define ROWS 16
#define COLS 16 // this value decides the size as either 8 bits or 16 bits and up
#define CHAROFFSET ' ' 
#define numOfLetters 28

//===================DON'T MODIFY THESE====================================
#define SIZE (COLS/8)
#define TYPE (COLS/8)
//=========================================================================

#define NUM (COLS/4)+1

template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

File myFile;

extern uint8_t BigFont[];
extern uint8_t SmallFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
// Main screen
Box Pixel(&B), Bit(&B), clean(&B), saveImg(&B), loadImg(&B), editImg(&B), FileButton(&B), SidePanel(&B);

Box PrevChar(&B), NextChar(&B);
// File manager screen
Box OkButton(&B), CancelButton(&B), ListSelect(&B), BackButton(&B), NewButton(&B), DeleteButton(&B);

MobileKeyboard myKB(&B);
char message[30];
byte Comm = 0;

struct Canvas
{
  void setCoords(short X, short Y, short W, short H, word fore, word back)
  {
    x = X; y = Y; w = W; h = H; FC = fore; BC = back;
  }
  short x, y, w, h;
  word FC, BC;
};

Canvas Grid, imgCanvas;

short Pixels[ ROWS ] = {0};
short MasterCopy[ ROWS * numOfLetters ] = {0};
char Comments[numOfLetters][25] = {0};

template<typename T, size_t N>
void MakePixelFile(T(&MCpixels)[N], char *fileName, char *imgName)
{
  Serial.println(SIZE);
  if (SD.exists(fileName))
    SD.remove(fileName);

  myFile = SD.open(fileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile)
  {
    Serial.print(F("Writing to file ")); Serial.println(fileName);

    short i = 0;
    char buf[(24 / SIZE) + 1];
    myFile.print("#include <avr/pgmspace.h> \n#if defined(__AVR__)\n\n#elif defined(__PIC32MX__)\n\t#define PROGMEM \n#elif defined(__arm__)\n\t#define PROGMEM \n#endif \n\n");
    myFile.print("const uint8_t ");

    myFile.print(imgName);
    myFile.print(" [");
    myFile.print(((ROWS * SIZE) * numOfLetters) + 4);
    myFile.println("] PROGMEM = {");

    char headerBuf[30] = {'\0'};
    sprintf(headerBuf, "0x%02X, 0x%02X, 0x%02X, 0x%02X, ", ROWS, COLS, 0, numOfLetters );

    myFile.println(headerBuf);

    Comm = 0;
    // this writes the pixel data to the SD card
    while (i < N)
    {
      if ((COLS / 8) == 4) // break up into bytes **I doubt it will ever get here
        sprintf(buf, "0x%02X, 0x%02X, 0x%02X, 0x%02X,", byte(MasterCopy[i] >> 32), byte(MasterCopy[i] >> 16), byte(MasterCopy[i] >> 8), byte(MasterCopy[i] & 0x00ff) );
      else if ((COLS / 8) == 2) // break up into bytes
      {
        sprintf(buf, "0x%02X, 0x%02X, ", byte(MasterCopy[i] >> 8), byte(MasterCopy[i] & 0x00ff) );
        Serial.println(i);
      }
      else // save as shorts
        sprintf(buf, "0x%02X, ", short(MasterCopy[i]) & 0x00FF );

      myFile.print(buf);
      memset(buf, 0, 14 / SIZE); // clear the buffer contents
      ++i;
      if ((i > 0) && ((i % ROWS) == 0))
      {
        myFile.print("//");
        myFile.print(Comments[Comm + 1]);
        Comm++;
        myFile.println();
      }
    }

    //once the pixel data is written, add the closing bracket and colon
    myFile.print("};");

    // close the file:
    myFile.close();
    Serial.println("done.");
  }
}

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_LOW);

  loadingScreen();
  delay(2000);
  myGLCD.fillScr(0);

  Grid.setCoords(10, 10, COLS * 28, ROWS * 28, BLUE, BLACK);
  imgCanvas.setCoords(550, 300, COLS * 11, ROWS * 11, BLUE, BLACK); // 16*11 -> best results should be a divisor of both the images width and height
  drawGrid(Grid, COLS, ROWS, 0);

  Bit.Coords(550, 60, 660, 100);
  Bit.Colors(GREEN, RED, ROUNDED, FILL);
  Bit.Text("SET", "CLEAR", BLACK, Big);
  Bit.SetState(true);
  Bit.Draw();

  clean.Coords(550, 110, 660, 150);
  clean.Colors(GREEN, RED, ROUNDED, FILL);
  clean.Text("CLEAN", BLACK, Big);
  clean.Draw();

  saveImg.Coords(680, 60, 790, 100);
  saveImg.Colors(GREEN, RED, ROUNDED, FILL);
  saveImg.Text("SAVE", BLACK, Big);
  saveImg.Draw();

  loadImg.Coords(680, 110, 790, 150);
  loadImg.Colors(GREEN, RED, ROUNDED, FILL);
  loadImg.Text("LOAD", BLACK, Big);
  loadImg.Draw();

  editImg.Coords(imgCanvas.x, imgCanvas.y, imgCanvas.x + imgCanvas.w, imgCanvas.y + imgCanvas.h);
  editImg.Colors(imgCanvas.FC, SQUARED, NOFILL);
  editImg.Text("", BLACK, Big);
  editImg.Padding(0, BLACK);

  FileButton.Coords(550, 10, 790, 50);
  FileButton.Colors(BLUE, RED, ROUNDED, FILL);
  FileButton.Text("", WHITE, Big);
  FileButton.Draw();

  SidePanel.Coords(545, 1, 799, myGLCD.getDisplayYSize());
  SidePanel.Padding(0, BLACK);
  SidePanel.Draw(false); // disable text
  SidePanel.Colors(BLACK, SQUARED, NOFILL);
  SidePanel.SetState(true);

  PrevChar.Coords(550, 160, AUTO, 200);
  PrevChar.Colors(GREEN, CYAN, ROUNDED, FILL);
  PrevChar.Text("Prev", BLACK, Big);
  PrevChar.Draw();

  NextChar.Coords(718, 160, AUTO, 200);
  NextChar.Colors(GREEN, CYAN, ROUNDED, FILL);
  NextChar.Text("Next", BLACK, Big);
  NextChar.Draw();
}

void loop()
{
  static int CharIndex = 0;
  static short Color = GREEN;
  static unsigned long RefreshTimer = millis();
  static bool _Refresh = false;
  static bool pc_last = 1, nc_last = 1;

  if (SidePanel.Touch(false) || _Refresh) // disable drawing the frame
  {
    if (clean.Delay(500))
    {
      drawGrid(Grid, COLS, ROWS, CharIndex);
      memset(Pixels, '\0', sizeof(Pixels));
      RefreshTimer = millis();
      _Refresh = true;
    }

    if (saveImg.Delay(500))
    {
      SaveToMaster(CharIndex);
      char fname[30] = {NULL};
      FileButton.getText(fname);
      MakePixelFile(MasterCopy, fname, "SpriteSheet");
      SidePanel.SetState(true);
      RefreshTimer = millis();
      _Refresh = true;
    }

    if (loadImg.DoubleClick())
    {
      drawGrid(Grid, COLS, ROWS, CharIndex);
      memset(Pixels, '\0', sizeof(Pixels));

      char fname[30] = {NULL};
      FileButton.getText(fname);
      LoadImageFromSD(fname);
      RefreshTimer = millis();
      _Refresh = true;
    }

    if (FileButton.Touch())
    {
      SetupButtonsOtherButtons(message);
      for (byte i = 0, j = strlen(message); i < j; i++)
      {
        message[i] = tolower(message[i]);
      }
      FileButton.Text(message, WHITE, Big);

      drawGrid(Grid, COLS, ROWS, CharIndex);
      Bit.ReDraw();
      clean.ReDraw();
      saveImg.ReDraw();
      loadImg.ReDraw();
      editImg.ReDraw();
      FileButton.ReDraw();
      PrevChar.ReDraw();
      NextChar.ReDraw();
      RefreshTimer = millis();
      _Refresh = true;
    }

    bool pcn = PrevChar.Touch();
    if (pcn != pc_last)
    {
      if (pcn)
      {
        CharIndex--;
        if (CharIndex <= 0)
        {
          CharIndex = 0;
          PrevChar.Colors(GREY, ROUNDED, FILL);
          PrevChar.ReDraw();
        }
        else
        {
          PrevChar.Colors(GREEN, CYAN, ROUNDED, FILL);
          PrevChar.ReDraw();
        }
        char fname[30] = {NULL};
        FileButton.getText(fname);
        LoadFromMaster(CharIndex);
        myGLCD.setColor(BLACK);
        myGLCD.fillRect(imgCanvas.x, imgCanvas.y - 20, imgCanvas.x + 25 * 8, imgCanvas.y - 2);
        myGLCD.setColor(WHITE);
        myGLCD.setFont(SmallFont); // small font for the comments
        myGLCD.print(Comments[CharIndex + 1], imgCanvas.x, imgCanvas.y - 20);
        myGLCD.setFont(BigFont); // change font back to big for everything else
      }
      pc_last = pcn;
      editImg.ReDraw();
    }

    bool ncn = NextChar.Touch();
    if (ncn != nc_last)
    {
      if (ncn)
      {
        CharIndex++;
        if (CharIndex >= numOfLetters)
        {
          CharIndex = numOfLetters - 1;
          NextChar.Colors(GREY, ROUNDED, FILL);
          NextChar.ReDraw();
        }
        else
        {
          NextChar.Colors(GREEN, CYAN, ROUNDED, FILL);
          NextChar.ReDraw();
        }
        char fname[30] = {NULL};
        FileButton.getText(fname);
        LoadFromMaster(CharIndex);
        myGLCD.setColor(BLACK);
        myGLCD.fillRect(imgCanvas.x, imgCanvas.y - 20, imgCanvas.x + 25 * 8, imgCanvas.y - 2);
        myGLCD.setColor(WHITE);
        myGLCD.setFont(SmallFont); // small font for the comments

        myGLCD.print(Comments[CharIndex + 1], imgCanvas.x, imgCanvas.y - 20);
        myGLCD.setFont(BigFont);
      }
      nc_last = ncn;
      editImg.ReDraw();
    }

    myGLCD.setColor(WHITE);
    char *idxText = "00";
    idxText[0] = CharIndex % 10;
    idxText[1] = CharIndex % 10;
    myGLCD.printNumI(CharIndex, 660, 180);
  }

  if (editImg.DoubleClick())
    drawLoadedImg(Grid, Pixels, COLS, ROWS, true);

  //refresh the side buttons after 1 second or until loading is done
  if (_Refresh && (millis() - RefreshTimer >= 1000UL))
  {
    RefreshTimer = millis();
    _Refresh = false;
  }

  drawPixels(Grid, COLS, ROWS, Bit.Toggle(), Color, CharIndex);
}

void LoadImageFromSD(char * filename)
{
  short startChar, Size, imgRows, imgCols;

  Serial.println(filename);
  for (int index = 0; index < numOfLetters; index++)
  {
    File myImg = SD.open(filename);

    if (myImg)
    {
      // get the header
      if (myImg.available())
      {
        while (myImg.read() != '{');
        imgRows = Filter(myImg);
        imgCols = Filter(myImg);
        startChar = Filter(myImg);
        Size = Filter(myImg);
      }
      Serial << imgRows << " " << imgCols << " " << startChar << " " << Size << "\n";

      // read from the file until there's nothing else in it:
      unsigned short _start = 0, j = ROWS, _end, cycle;
      _start = (index >= 0 ? index * j : 0);
      _end = _start + j;

      cycle = _start;
      while (cycle != 0)
      {
#if SIZE == BYTES
        Filter(myImg);
#else
        Filter(myImg);
        Filter(myImg);
#endif
        cycle--;
      }

      Comm = 0; // for the comments

      while (true)
      {
#if SIZE == BYTES
        MasterCopy[_start] = Filter(myImg);
#else
        MasterCopy[_start] = (short((Filter(myImg) & 0xffff) << 8) | (Filter(myImg) & 0x00ff));
#endif
        _start++;
        if (_start >= _end) break;
      }

      myImg.close();
    }
    else
    {
      // if the file didn't open, print an error:
      Serial.println(F("error opening file"));
    }
  }
}

void LoadFromMaster(int index)
{
  for (byte i = 0; i < ROWS; i++)
    Pixels[i] = MasterCopy[index * ROWS + i];

  drawLoadedImg(imgCanvas, Pixels, COLS, ROWS, false); // false = no grid
}

void SaveToMaster(int index)
{
  for (byte i = 0; i < ROWS; i++)
    MasterCopy[index * ROWS + i] = Pixels[i];
}


