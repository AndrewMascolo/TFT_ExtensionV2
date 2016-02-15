#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <SPI.h>
#include <SD.h>

File myFile;
// Declare which fonts we will be using
extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25,26,27,28);
//UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);

MobileKeyboard myKB(&B);

void setup()
{
  Serial.begin(115200); // default bluetooth baudrate.
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.fillScr(0, 0, 0);

  myKB.Colors(WHITE, BLACK, BLUE, FILL);
  myKB.SetupMobileKB(0, 120, 800, 300); //NEEDED TO WORK!
  pinMode(53, OUTPUT);

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
}

void loop()
{
  char buf[30] = {'\0'};
  //myKB.ReceiveMsg(0, 0, YELLOW); // X,Y(position on screen), Color

  if (myKB.Mobile_KeyBoard(buf, BLUE))
  {
    myGLCD.setColor(WHITE);
    myGLCD.print(buf, 0, 0);
 
    myFile = SD.open(buf, FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print("Writing to test.txt...");
      myFile.println("testing 1, 2, 3.");
      // close the file:
      myFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }

    // re-open the file for reading:
    myFile = SD.open(buf);
    if (myFile) {
      Serial.println(buf);

      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      // close the file:
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.print("error opening test.txt");
    }
  }

}


