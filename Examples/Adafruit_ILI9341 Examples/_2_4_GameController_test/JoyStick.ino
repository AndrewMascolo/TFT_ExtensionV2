//const byte LY = A0;
//const byte LX = A2;
//const byte RY = A4;
//const byte RX = A6;

#define LSYH 860
#define LSYL 209
#define LSXH 848
#define LSXL 211
#define RSYH 865
#define RSYL 222
#define RSXH 893
#define RSXL 239

#ifdef Core_Ext_h
#define myMap map<long>
#else
#define myMap map
#endif

struct Joysticks getValues(const byte Ly, const byte Lx, const byte Ry, const byte Rx)
{
  static int lLy, lLx, lRy, lRx;
  static Joysticks J;
  J.Ly = myMap(analogRead(Ly), LSYL, LSYH, 255, 0);
  J.Lx = myMap(analogRead(Lx), LSXL, LSXH, 255, 0);
  J.Ry = myMap(analogRead(Ry), RSYL, RSYH, 255, 0);
  J.Rx = myMap(analogRead(Rx), RSXL, RSXH, 255, 0);
 
#if showJoySticks
  if (J.Lx != lLx || J.Ly != lLy)
  {
    tft.fillCircle(60, 120, 48, 0x0);
    tft.fillCircle(30 + (J.Lx>>4)*4 , 90 + (16-(J.Ly>>4))*4, 10, 0x001f);
    lLx = J.Lx;
    lLy = J.Ly;
  }
  if (J.Rx != lRx || J.Ry != lRy)
  {
    tft.fillCircle(260, 120, 48, 0x0);
    tft.fillCircle(230 + (J.Rx>>4)*4, 90 + (16-(J.Ry>>4))*4, 10, 0x001f);
    lRx = J.Rx;
    lRy = J.Ry;
  }
#endif

#if showJoystickValues
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 0);
  tft.print(J.Ly); tft.print(F(" ")); tft.print(J.Lx);
  tft.print('\t');
  tft.print(J.Ry); tft.print(F(" ")); tft.print(J.Rx);
#endif

  return J;
}

#if Debug
void DebugJoySticks()
{
  joyLymax = max(analogRead(Ly), joyLymax);
  joyLymin = min(analogRead(Ly), joyLymin);
  joyLxmax = max(analogRead(Lx), joyLxmax);
  joyLxmin = min(analogRead(Lx), joyLxmin);
  joyRymax = max(analogRead(Ry), joyRymax);
  joyRymin = min(analogRead(Ry), joyRymin);
  joyRxmax = max(analogRead(Rx), joyRxmax);
  joyRxmin = min(analogRead(Rx), joyRxmin);
  
  tft.setCursor(0, 0);
  tft.print(joyLymax); tft.print(" "); tft.println(joyLymin); 
  tft.print(joyLxmax); tft.print(" "); tft.println(joyLxmin); 
  tft.print(joyRymax); tft.print(" "); tft.println(joyRymin); 
  tft.print(joyRxmax); tft.print(" "); tft.println(joyRxmin); 
}
#endif

void SendJoyStickData()
{
  static Joysticks joy;
  
  joy = getValues(A0, A2, A4, A6);

  //DebugJoySticks();

  Array[0] = joy.Ly / 100 + '0';
  Array[1] = (joy.Ly / 10) % 10 + '0';
  Array[2] = (joy.Ly % 10) + '0';

  Array[3] = joy.Ry / 100 + '0';
  Array[4] = (joy.Ry / 10) % 10 + '0';
  Array[5] = (joy.Ry % 10) + '0';
  Array[6] = '.';
  
  vw_send((uint8_t*)Array, strlen(Array));
  vw_wait_tx();
}
