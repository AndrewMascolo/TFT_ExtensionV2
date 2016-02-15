// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <SPI.h>
#include <SD.h>

#define MONO  1
#define COLOR 2
#define BYTES 1
#define INTS  2

//=========================SKETCH SETTINGS=================================
#define TYPE COLOR
#define SIZE INTS
#define ROWS 16
#define COLS 16
//=========================================================================

File myFile;

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(CTE70, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
// Main screen
Box Pixel(&B), Bit(&B), clean(&B), saveImg(&B), loadImg(&B), editImg(&B), FileButton(&B), SidePanel(&B);

// File manager screen
Box OkButton(&B), CancelButton(&B), ListSelect(&B), BackButton(&B), NewButton(&B), DeleteButton(&B);

Box * ColorPallet[16];
MobileKeyboard myKB(&B);
char message[30];

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
#define NUM (COLS/4)+1

#if TYPE == COLOR
short Pixels[((ROWS * COLS) * 2) / SIZE] = {0};
#else
short Pixels[(ROWS * 2) / SIZE] = {0};
#endif

short ColorPalletDefinedColors[ROWS] =
{
  BLACK, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK,
  CYAN, GREY, DARK_RED, DARK_ORANGE, DARK_YELLOW, DARK_GREEN, DARK_BLUE, WHITE
};

template<typename T, size_t N>
void MakePixelFile(T(&pixels)[N], char *fileName, char *imgName)
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
    char buf[14 / SIZE];
    if (SIZE == 1) // break up short to bytes
      myFile.print("const uint8_t ");
    else
      myFile.print("const uint16_t ");

    myFile.print(imgName);
    myFile.print(" [");
#if TYPE == COLOR
    myFile.print((((ROWS * COLS) * 2) / SIZE) + 4 / SIZE);
#else
    myFile.print(((ROWS * 2) / SIZE) + 4 / SIZE);
#endif
    myFile.println("] PROGMEM = {");

    char headerBuf[30] = {'\0'};
    if (SIZE == 1) // break up shorto bytes
      sprintf(headerBuf, "0x%02X, 0x%02X, 0x%02X, 0x%02X, ", ((N * SIZE) > 32 ? COLOR : MONO), SIZE, ROWS, COLS );
    else // save as shorts
      sprintf(headerBuf, "0x%04X, 0x%04X, ", short((((N * SIZE) > 32 ? COLOR : MONO) << 8) | SIZE), short((ROWS << 8) | COLS) );

    myFile.println(headerBuf);

    // this writes the pixel data to the SD card
    while (i < N)
    {
      if (SIZE == 1) // break up into bytes
        sprintf(buf, "0x%02X, 0x%02X, ", byte(Pixels[i] >> 8), byte(Pixels[i] & 0x00ff) );
      else // save as shorts
        sprintf(buf, "0x%04X, ", short(Pixels[i]) & 0xFFFF );

      myFile.print(buf);
      //Serial.println(buf);
      memset(buf, 0, 14 / SIZE);
      //Serial.println(buf);
      i++;
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
  drawGrid(Grid, COLS, ROWS);

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
  FileButton.Text("NewImg.c", WHITE, Big);
  FileButton.Draw();

  SidePanel.Coords(545, 1, 799, myGLCD.getDisplayYSize());
  SidePanel.Padding(0, BLACK);
  SidePanel.Draw(false); // disable text
  SidePanel.Colors(BLACK, SQUARED, NOFILL);
  SidePanel.SetState(true);

#if TYPE == COLOR
  // create color pallet.
  for (byte i = 0; i < 16; i++)
  {
    ColorPallet[i] = new Box(&B);
    ColorPallet[i]->Coords(550 + 30 * (i % 8), 180 + 30 * (i / 8), AUTO, AUTO);
    ColorPallet[i]->Colors(ColorPalletDefinedColors[i], ROUNDED, FILL);
    ColorPallet[i]->Text(" ", BLACK, Big);
    ColorPallet[i]->Draw();
  }
#endif

}

void loop()
{
  static byte i = 0;
  static short Color = GREEN;

  if (SidePanel.Touch(false)) // disable drawing the frame
  {
#if TYPE == COLOR
    if (ColorPallet[i]->Touch())
    {
      Color = ColorPallet[i]->getButtonPressedColor();
      myGLCD.setColor(Color);
      myGLCD.fillRect(550, 250, 785, 270);
    }
#endif

    if (clean.Delay(500))
    {
      drawGrid(Grid, COLS, ROWS);
      memset(Pixels, '\0', sizeof(Pixels));
    }
    i++;
    if (i > 15) i = 0;

    if (saveImg.DoubleClick())
    {
      char fname[30] = {NULL};
      FileButton.getText(fname);
      MakePixelFile(Pixels, fname, "Face");
      SidePanel.SetState(true);
    }

    if (loadImg.DoubleClick())
    {
      //clean the canvas
      drawGrid(Grid, COLS, ROWS);
      memset(Pixels, '\0', sizeof(Pixels));

      char fname[30] = {NULL};
      FileButton.getText(fname);
      LoadImageFromSD(Grid, fname);
      SidePanel.SetState(true); 
    }

    if (editImg.DoubleClick())
      drawLoadedImg(Grid, Pixels, 16, 16, true);

    if (FileButton.Touch())
    {
      SetupButtonsOtherButtons(message);
      FileButton.Text(message, WHITE, Big);
      for (byte i = 0; i < 16; i++)
        ColorPallet[i]->ReDraw();

      drawGrid(Grid, COLS, ROWS);
      Bit.ReDraw();
      clean.ReDraw();
      saveImg.ReDraw();
      loadImg.ReDraw();
      editImg.ReDraw();
      FileButton.ReDraw();
    }
  }
  
  drawPixels(Grid, COLS, ROWS, Bit.Toggle(), Color);
  
}

void LoadImageFromSD(struct Canvas canvas, char * filename)
{
  short Type, Size, imgRows, imgCols;

  File myImg = SD.open(filename);
  if (myImg)
  {
    Serial.println(filename);

    if (myImg.available())
    {
      while (myImg.read() != '{');
#if SIZE == BYTES
      Type = Filter(myImg);
      Size = Filter(myImg);
#else
      uint16_t tmp = Filter(myImg);
      Serial.println(tmp);
      Type = (tmp & 0xFF00) >> 8;
      Size = tmp & 0x00FF;
#endif

      if (Size == 1)
      {
        imgRows = Filter(myImg);
        imgCols = Filter(myImg);
      }
      else
      {
        uint16_t tmp = Filter(myImg);
        imgRows = (tmp & 0xFF00) >> 8;
        imgCols = tmp & 0x00FF;
      }
    }

    Serial.print(Type); Serial.print(F("\t"));
    Serial.print(Size); Serial.print(F("\t"));
    Serial.print(imgRows); Serial.print(F("\t"));
    Serial.println(imgCols);

    myGLCD.setColor(RED);
    myGLCD.setBackColor(BLACK);
    if (Type != TYPE)
    {
      myGLCD.print("File type does not match sketch type", CENTER, myGLCD.getDisplayYSize() / 2);
      Serial.println(F("File type does not match sketch type"));
      myImg.close();
      return;
    }

    if (Size != SIZE)
    {
      myGLCD.print("File size does not match sketch size", CENTER, 20 + (myGLCD.getDisplayYSize() / 2));
      Serial.println(F("File size does not match sketch size"));
      myImg.close();
      return;
    }

    // read from the file until there's nothing else in it:
    unsigned short i = 0;
    while (1)
    {
      if (Size == 1)
        Pixels[i] = (Filter(myImg) << 8) | Filter(myImg);
      else
        Pixels[i] = Filter(myImg);

#if TYPE == COLOR
      if ((i + 1) >= (((ROWS * COLS) * 2) / SIZE)) break;
#else
      if ((i + 1) >= ((ROWS * 2) / SIZE)) break;
#endif
      i++;
    }

    myImg.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }

  drawLoadedImg(imgCanvas, Pixels, imgCols, imgRows, false); // false = no grid
}


