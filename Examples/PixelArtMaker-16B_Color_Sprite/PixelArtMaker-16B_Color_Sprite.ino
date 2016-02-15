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

//================SKETCH SETTINGS FOR 16BIT COLOR SPRITE====================
#define TYPE COLOR
#define SIZE INTS
#define ROWS 64
#define COLS 64
//=========================================================================

template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
} 

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

short Pixels[ ROWS * COLS ] = {0};

short ColorPalletDefinedColors[ROWS] =
{
  BLACK, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, REG_PINK,
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
    myFile.print("const uint16_t ");

    myFile.print(imgName);
    myFile.print(" [");
    myFile.print((ROWS * COLS) * 2);
    myFile.println("] PROGMEM = {");

    // this writes the pixel data to the SD card
    while (i < N)
    {
      sprintf(buf, "0x%04X, ", short(Pixels[i]) & 0xFFFF );
      myFile.print(buf);
      memset(buf, 0, 14 / SIZE);
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

  Grid.setCoords(10, 10, COLS * (myGLCD.getDisplayYSize()/COLS), ROWS * (myGLCD.getDisplayYSize()/ROWS), BLUE, BLACK);
  imgCanvas.setCoords(550, 300, COLS * (160/COLS), ROWS * (160/COLS), BLUE, BLACK); // 16*11 -> best results should be a divisor of both the images width and height
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

  // create color pallet.
  for (byte i = 0; i < 16; i++)
  {
    ColorPallet[i] = new Box(&B);
    ColorPallet[i]->Coords(550 + 30 * (i % 8), 180 + 30 * (i / 8), AUTO, AUTO);
    ColorPallet[i]->Colors(ColorPalletDefinedColors[i], ROUNDED, FILL);
    ColorPallet[i]->Text(" ", BLACK, Big);
    ColorPallet[i]->Draw();
  }
}

void loop()
{
  static byte i = 0;
  static short Color = GREEN;

  if (SidePanel.Touch(false)) // disable drawing the frame
  {
    if (ColorPallet[i]->Touch())
    {
      Color = ColorPallet[i]->getButtonPressedColor();
      myGLCD.setColor(Color);
      myGLCD.fillRect(550, 250, 785, 270);
    }

    if (clean.Delay(500))
    {
      drawGrid(Grid, COLS, ROWS);
      memset(Pixels, '\0', sizeof(Pixels));
    }
    i++;
    if (i > 15) i = 0;

    if (saveImg.Delay(500))
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
      drawLoadedImg(Grid, Pixels, COLS, ROWS, true);

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
  File myImg = SD.open(filename);
  if (myImg)
  {
    Serial.println(filename);

    if (myImg.available())
    {
      while (myImg.read() != '{');

      // read from the file until there's nothing else in it:
      unsigned short i = 0;
      while (1)
      {
        Pixels[i] = Filter(myImg);
        //Serial << i << F(" : ") << Pixels[i] << "\n";
        if ((i + 1) >= (ROWS * COLS)) break;
        i++;
      }

      myImg.close();
    }
    else
    {
      // if the file didn't open, print an error:
      Serial.println("error opening file");
    }

    drawLoadedImg(imgCanvas, Pixels, COLS, ROWS, false); // false = no grid
  }
}

