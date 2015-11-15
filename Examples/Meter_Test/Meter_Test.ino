
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>

extern uint8_t BigFont[];

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);

Base B(&myGLCD, &myTouch);
Meter Voltage_Bar(&B, VERTICAL);

void setup()
{
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.setBackColor(TRANSPARENT);
  myGLCD.fillScr(BLACK);

  Voltage_Bar.Coords(0, 150, 90, 350); // FIRST, must set coordinates before setting Range and optional starting value
  Voltage_Bar.Attributes(ROUNDED, FILL);  //
  Voltage_Bar.Colors(RED, YELLOW, GREEN, BLACK ); // Optional but good to have some color
  Voltage_Bar.SetDirection(BTT); // bottom to top   // Optional
  Voltage_Bar.Percentages(90, 50); // RED starts at 90% from bottom, YELLOW start at 50% and GREEN starts at 0
  Voltage_Bar.SetRange(0, 1023);                 // SECOND, Must be set after coordinates
  Voltage_Bar.SetStartingValue(0);                // THIRD (Optional, but must be set after range)
  Voltage_Bar.HeaderText("VOLTS", WHITE, (byte)Big);
  Voltage_Bar.ProgressText(WHITE, GREY, Big, BOTTOM);
  Voltage_Bar.Draw();
  
  for (int i = 0; i <= 1024; i += 16) // demo voltage
  {
    Voltage_Bar.SetValue(i);
    Voltage_Bar.Progress();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
