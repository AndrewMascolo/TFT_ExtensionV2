/*
 Demonstrate functions to copy, scan, read and delete lines from a file

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 created   March 29 2016
 by Andrew Mascolo

 */

#include <SPI.h>
#include <SD.h>

#define isSpace(x) (x == ' ')
#define isComma(x) (x == ',')
template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

struct LinesAndPositions
{
  int NumberOfLines; // number of lines in file
  int SOL[50]; // start of line in file
  int EOL[50]; // end of line in file
};

template<typename T, size_t N>
char* ReadSelectedLinesFromFile(T (&buf)[N], char * filename, char * StrOfLines)
{
  char tempBuf[N];

  Serial << "Reading multiple lines, please wait";
  for (unsigned short i = 0, j = strlen(StrOfLines), index = 0; i <= j; i++)
  {
    char C = (*StrOfLines++);
    if (isComma(C) || (i == j))
    {
      ReadLineFromFile(tempBuf, filename, index);
      strncat(buf, tempBuf, N);
      index = 0;
    }
    else if (isSpace(C)) continue;
    else
      index = (index * 10) + C - '0';
    if ((i % 2) == 0)
      Serial << ".";
  }

  Serial.println();
  return buf;
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Uncomment this if you already have a copy of COPY.txt
  CopyFiles("test.txt", "COPY.txt");

  ShowFileContents("test.txt");

  // Uncomment this if you don't already have a copy of COPY.txt
  //CopyFiles("COPY.txt", "test.txt");
  //ShowFileContents("COPY.txt");

  LinesAndPositions X = FindLinesAndPositions("test.txt");
  Serial << "\nNumber of lines from test.txt:" << X.NumberOfLines << "\n";
  Serial << "--------------------------------\n\n";

  char buf[50] = {NULL};
  //ReadLineFromFile(buf, "test.txt", 1) << "\n";
  //ReadMultipleLinesFromFile(buf, "test.txt", 1, 2);
  ReadSelectedLinesFromFile(buf, "test.txt", "1,3,4");
  Serial << "From passed in buffer:\n" << buf << "\n";

  Serial << "--------------------------------\n";

  //DeleteLineFromFile("test.txt", 2); // delete line 2 from file
  //DeleteMultipleLinesFromFile("test.txt", 2, 3); delete lines 2 through 3 from file
  DeleteSelectedLinesFromFile("test.txt", "1, 3, 4"); // delete individual lines 1,3,4 from file. **Takes a few seconds**
  Serial.println("Done.");

  ShowFileContents("test.txt");
  X = FindLinesAndPositions("test.txt");
  Serial << "\nNumber of lines from test.txt:" << X.NumberOfLines << "\n";

}

void loop()
{
  // nothing happens after setup
}

struct LinesAndPositions FindLinesAndPositions(char* filename)
{
  File myFile;
  LinesAndPositions LNP;

  myFile = SD.open(filename);
  if (myFile)
  {
    LNP.NumberOfLines = 0;
    LNP.SOL[0] = 0; //the very first start-of-line index is always zero

    unsigned int Pos;
    while (myFile.available())
    {
      Pos = myFile.position(); // get the position. Put before .read(), otherwise it will not start at 0 like it should.
      if (myFile.read() == '\n') // read until the newline character has been found
      {
        LNP.EOL[LNP.NumberOfLines] = Pos; // record the location of where it is in the file
        LNP.NumberOfLines++; // update the number of lines found
        LNP.SOL[LNP.NumberOfLines] = Pos + 1; // the start-of-line is always 1 character more than the end-of-line location
      }
    }
    LNP.EOL[LNP.NumberOfLines] = Pos + 1; // record the last locations
    LNP.NumberOfLines += 1; // record the last line

    myFile.close();
  }

  return LNP;
}

void CopyFiles(char* ToFile, char* FromFile)
{
  File myFileOrig;
  File myFileTemp;

  if (SD.exists(ToFile))
    SD.remove(ToFile);

  //Serial << "\nCopying Files. From:" << FromFile << " -> To:" << ToFile << "\n";

  myFileTemp = SD.open(ToFile, FILE_WRITE);
  myFileOrig = SD.open(FromFile);
  if (myFileOrig)
  {
    while (myFileOrig.available())
    {
      myFileTemp.write( myFileOrig.read() ); // make a complete copy of the original file
    }
    myFileOrig.close();
    myFileTemp.close();
    //Serial.println("done.");
  }
}

void ShowFileContents(char* filename)
{
  Serial.println(filename);
  File myFile = SD.open(filename);
  if (myFile)
  {
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    Serial.println();
    myFile.close();
  }
}

void DeleteLineFromFile(char * filename, int Line)
{
  DeleteMultipleLinesFromFile(filename, Line, Line);
}

void DeleteSelectedLinesFromFile(char * filename, char * StrOfLines)
{
  byte offset = 0;
  Serial << "Deleting multiple lines, please wait";
  for (unsigned short i = 0, j = strlen(StrOfLines), index = 0; i <= j; i++)
  {
    char C = (*StrOfLines++);
    if (isComma(C) || (i == j))
    {
      DeleteLineFromFile(filename, index - offset);
      offset++;
      index = 0;
    }
    else if (isSpace(C)) continue;
    else
      index = (index * 10) + C - '0';
    if ((i % 2) == 0)
      Serial << ".";
  }
  Serial.println();
}

void DeleteMultipleLinesFromFile(char * filename, int SLine, int ELine)
{
  File myFileOrig;
  File myFileTemp;

  // If by some chance it exists, remove the temp file from the card
  if (SD.exists("tempFile.txt"))
    SD.remove("tempFile.txt");

  // Get the start and end of line positions from said file
  LinesAndPositions FileLines = FindLinesAndPositions(filename);

  if ((SLine > FileLines.NumberOfLines) || (ELine > FileLines.NumberOfLines))
    return;

  myFileTemp = SD.open("tempFile.txt", FILE_WRITE);
  myFileOrig = SD.open(filename);

  if (myFileOrig)
  {
    unsigned int position = 0;
    char C;

    while (myFileOrig.available())
    {
      position = myFileOrig.position();
      C = myFileOrig.read();

      // Copy the file but exclude the entered lines by user
      if ((position < FileLines.SOL[SLine - 1]) || (position > FileLines.EOL[ELine - 1]))
        myFileTemp.write(C);
    }
    myFileOrig.close();
    myFileTemp.close();
  }

  //copy the contents of tempFile back to the original file
  CopyFiles(filename, "tempFile.txt");

  // Remove the tempfile from the SD card
  if (SD.exists("tempFile.txt"))
    SD.remove("tempFile.txt");
}

char* ReadLineFromFile(char* Buffer, char* filename, const byte Line)
{
  return ReadMultipleLinesFromFile(Buffer, filename, Line, Line);
}

char* ReadMultipleLinesFromFile(char* Buffer, char* filename, byte SLine, byte ELine)
{
  SLine -= 1;
  ELine -= 1;

  File myFile = SD.open(filename);
  if (myFile)
  {
    LinesAndPositions FileLines = FindLinesAndPositions(filename);

    // go to beginning of line
    myFile.seek(FileLines.SOL[SLine]);

    while (myFile.available())
    {
      unsigned int Pos = myFile.position();
      if (Pos <= (FileLines.EOL[ELine]))
      {
        Buffer[Pos - FileLines.SOL[SLine]] = myFile.read();
        Buffer[Pos - FileLines.SOL[SLine] + 1] = '\0';
      }
      else
        break;
    }

    myFile.close();
    return Buffer;
  }
  return 0;
}


