#ifndef AdaFruit_TFTE_Visuals_h
#define AdaFruit_TFTE_Visuals_h

/*
The MIT License (MIT)

Copyright (c) 2016 Andrew Mascolo Jr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

class ProgressBar : public Base
{
    struct Properties
    {
      int x;
      int y;
      int width;
      int height;
      int x2;
      int y2;
      word Color;
      long BackColor;
      bool Type;
      byte FontSize;
    } Props;

  protected:
    TFT_Display *_Disp;
    Base *_base;

  public:
    ProgressBar(Base * B) : _base(B)
    {
      _Disp = _base->getDisplay();
    }

    ~ProgressBar() { }

    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
    }

    void Attributes(word color, bool fontsize, char type, long bgc = 0xFFFFFFFF)
    {
      Props.Color = color;
      Props.BackColor = bgc;
      Props.Type = type;
      Props.FontSize = fontsize;
    }

    void Attributes(char * _text, word color, bool fontsize, char filler = ' ', long bgc = 0xFFFFFFFF)
    {
      Props.Color = color;
      Props.BackColor = bgc;
      text = _text;
      Props.Type = '#';
      Filler = filler;
      Props.FontSize = fontsize;
    }

    /*  Conflict with these two
    void Attributes(char * _text, word color, bool fontsize, char filler = ' ')
    {
      Attributes(_text, color, fontsize, filler, 0xFFFFFFFF);
    }

    void Attributes(char * _text, word color, bool fontsize, long bgc = 0xFFFFFFFF)
    {
      Attributes(_text, color, fontsize, ' ', bgc);
    }
    */

    void Progress(byte percent)
    {
      unsigned int centerBar, Leftside, length = strlen(text);
      _Disp->setTextColor(Props.Color, Props.BackColor);

      if (Props.x2 == AUTO)
        Props.x2 = Props.x + length * (Props.FontSize ? 16 : 8) + 10;

      if (Props.y2 == AUTO)
        Props.y2 = Props.y + (Props.FontSize ? 16 : 12) + 10;

      if (Props.Type == '#')
      {
        centerBar = (Props.x + Props.x2) / 2;

        Leftside = centerBar - length * (Props.FontSize ? 8 : 4);
      }

      //Overwrite Leftside from above
      if (Props.x2 == AUTO)
        Leftside = Props.x;

      for (unsigned int L = Props.x, i = 0; L < Props.x2 ; L++)
      {
        switch (Props.Type)
        {
          case '/':
            _Disp->drawLine(L, Props.y, L - 10, Props.y2, Props.Color);
            break;

          case '\\':
            _Disp->drawLine(L - 10, Props.y, L, Props.y2, Props.Color);
            break;

          case '>':
            _Disp->drawLine(L, Props.y, L + 10, (Props.y + Props.y2) / 2, Props.Color);
            _Disp->drawLine(L + 10, (Props.y + Props.y2) / 2, L, Props.y2, Props.Color);
            break;

          case '|':
            _Disp->drawFastVLine(L, Props.y, Props.y2 - Props.y, Props.Color);
            break;

          case '#':
            if (L >= Leftside ) // && i < length
            {
              if (i < length)
              {
                _Disp->setCursor(L, Props.y);
                _Disp->print(*text++);
                i++;
              }
              else
              {
                _Disp->setCursor(L, Props.y);
                _Disp->print(Filler);
              }
            }
            else
            {
              _Disp->setCursor(L, Props.y);
              _Disp->print(Filler);
            }

            L += (Props.FontSize ? 16 : 12) - 1;
            break;

          default:
            _Disp->setCursor(L, Props.y);
            _Disp->print(Props.Type);
            L += (Props.FontSize ? 16 : 12) - 1;
            break;
        }

        delay(50); // some delay
      }
    }

    Properties getButtonProperties()
    {
      return Props;
    }

  private:
    char *text, Filler;

};

class Meter : public Base
{
    struct Properties
    {
      int x;
      int y;
      int width;
      int height;
      int x2;
      int y2;
      word C1, C2, C3, BC, PadColor, TouchColor, PercentageTouchColor;
      long TouchBackColor, PercentageTouchBackColor;
      bool Round, Fill, Direction;
      byte PaddingSize, FontSize, PercentageFontSize;
    } Props;

  protected:
    bool orient;
    TFT_Display *_Disp;
    Base *_base;

  public:

    Meter(Base * B, bool _o) : _base(B), orient(_o)
    {
      _Disp = _base->getDisplay();

      locked = 0;
      SetRange();
      SetValue(-1);
      SetDirection(LTR);
      Padding(2, WHITE);
      Text(" ", BLACK, 0xffffffff, Small, MIDDLE);
      HeaderText(" ", BLACK, 0xffffffff, Small);
      FooterText(" ", BLACK, 0xffffffff, Small);
      T = -1;
    }

    ~Meter() { }

    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
    }

    void Attributes(bool round, bool fill)
    {
      Props.Round = round;
      Props.Fill = fill;
    }

    void Colors(word c1, word c2, word c3, word bc)
    {
      Props.C1 = c1;
      Props.C2 = c2;
      Props.C3 = c3;
      Props.BC = bc;
    }

    void Percentages(byte hi, byte mid, byte low = 0)
    {
      if (orient == VERTICAL)
      {
        int T_height = ((Props.y2 - Props.y) - (Props.PaddingSize * 2));
        if (Props.Direction == BTT)
        {
          Hi =  Props.y2 - (T_height * (float(hi) / 100.0));
          Mid = Props.y2 - (T_height * (float(mid) / 100.0));
          Low = Props.y2 - (T_height * (float(low) / 100.0));
        }
        else
        {
          Hi = (T_height * (float(hi) / 100.0)) + Props.y;
          Mid = (T_height * (float(mid) / 100.0)) + Props.y;
          Low = (T_height * (float(low) / 100.0)) + Props.y;
        }
      }
      else
      {
        int T_width = ((Props.x2 - Props.x) - (Props.PaddingSize * 2));
        if (Props.Direction == RTL)
        {
          Hi = Props.x2 - (T_width * (float(hi) / 100.0));
          Mid = Props.x2 - (T_width * (float(mid) / 100.0));
          Low = Props.x2 - (T_width * (float(low) / 100.0));
        }
        else
        {
          Hi = (T_width * (float(hi) / 100.0)) - Props.x;
          Mid = (T_width * (float(mid) / 100.0)) - Props.x;
          Low = (T_width * (float(low) / 100.0)) - Props.x;
        }
      }
    }

    void SetDirection(bool Dir) // Not used at this time
    {
      Props.Direction = Dir;
    }

    void Draw()
    {
      //Save_MainColor;
      if (Props.PaddingSize > 0)
      {
        if (!locked)
        {
          if (Props.Fill)
            (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, 2, Props.PadColor) : _Disp->fillRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, Props.PadColor));
          else
            (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, 2, Props.PadColor) : _Disp->drawRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, Props.PadColor));
          locked = true;
        }

        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.x - Props.PaddingSize * 2, Props.y2 - Props.y - Props.PaddingSize * 2, 2, Props.BC) : _Disp->fillRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.x - Props.PaddingSize * 2, Props.y2 - Props.y - Props.PaddingSize * 2, Props.BC));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.x - Props.PaddingSize * 2, Props.y2 - Props.y - Props.PaddingSize * 2, 2, Props.BC) : _Disp->drawRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.x - Props.PaddingSize * 2, Props.y2 - Props.y - Props.PaddingSize * 2, Props.BC));
      }
      else
      {
        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, 2, Props.BC) : _Disp->fillRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, Props.BC));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, 2, Props.BC) : _Disp->drawRect(Props.x, Props.y, Props.x2 - Props.x, Props.y2 - Props.y, Props.BC));
      }

      _Disp->setTextColor(Props.TouchColor, Props.TouchBackColor);
      _Disp->setTextSize(Props.FontSize ? 2 : 1);
      if (strcmp(text, " "))
      {
        _Disp->setCursor(TX, TY);
        _Disp->print(text);
      }

      //Restore_MainColor;
    }

    void Text(char * T, word color, byte size, int pos)
    {
      Text( T, color, 0xffffffff, size, pos);
    }

    void Text(char * T, word color, long Bcolor, byte size, int pos)
    {
      Props.FontSize = size;
      int Tsize = strlen(T) * (size ? 8 : 4);
      Props.TouchColor = color;
      Props.TouchBackColor = Bcolor;

      TX = ((Props.x + Props.x2) / 2) - Tsize;
      switch (pos)
      {
        case TOP:
          TY = Props.y - 20;
          break;
        case MIDDLE:
          TY = ((Props.y + Props.y2) / 2) - 8;
          break;
        case BOTTOM:
          TY = Props.y2 + 20;
          break;
        default:
          TY = pos;
      };

      text = T;
    }

    void HeaderText(char * T, word color, long Bcolor = 0xffffffff, byte size = Big)
    {
      Text(T, color, Bcolor, size, TOP);
    }

    void HeaderText(char * T, word color, byte size = Big)
    {
      Text(T, color, (byte)size, TOP);
    }

    void FooterText(char * T, word color, long Bcolor = 0xffffffff, byte size = Big)
    {
      Text(T, color, Bcolor, (byte)size, BOTTOM);
    }

    void FooterText(char * T, word color, byte size = Big)
    {
      Text(T, color, size, BOTTOM);
    }

    void ProgressText(word color, byte size, int pos = BOTTOM)
    {
      ProgressText(color, 0xffffffff, size, pos);
    }

    void ProgressText(word color, long Bcolor, byte size, int pos = BOTTOM)
    {
      Props.PercentageFontSize = size;
      int Tsize = 6 * (size ? 8 : 4);
      Props.PercentageTouchColor = color;
      Props.PercentageTouchBackColor = Bcolor;

      P_TX = ((Props.x + Props.x2) / 2) - Tsize;
      switch (pos)
      {
        case TOP:
          P_TY = Props.y - 20;
          break;
        case MIDDLE:
          P_TY = ((Props.y + Props.y2) / 2) - 8;
          break;
        case BOTTOM:
          P_TY = Props.y2 + 5;
          break;
        default:
          P_TY = pos;
      };
    }

    void Progress(bool textEnable = true)
    {
      //Save_MainColor;
      int Out = -1;
      int tmpGV = -1;

      if ((tmpGV = GetValue()) != -1)
        T = tmpGV;

      if (orient == VERTICAL)
      {
        if (Props.Direction == BTT)
        {
          //BTT
          for (int Y = Props.y2 - Props.PaddingSize; Y >= Props.y + Props.PaddingSize; Y--)
          {
            if (T <= Y)
            {
              if ((Y <= Low) && (Y > Mid))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C3);
              else if ((Y <= Mid) && (Y > Hi))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C2);
              else if ((Y <= Hi) && (Y > Props.y))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C1);
            }
            else
              _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.BC);
          }
        }
        else
        {
          for (int Y = Props.y + Props.PaddingSize; Y < Props.y2 - Props.PaddingSize; Y++)
          {
            if (T >= Y)
            {
              if ((Y > Low) && (Y <= Mid))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C3);
              else if ((Y > Mid) && (Y <= Hi))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C2);
              else if ((Y > Hi) && (Y <= Props.y2))
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.C1);
            }
            else
              _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, Props.BC);
          }
        }
      }
      else
      {
        if (Props.Direction == LTR)
        {
          //LTR
          for (int X = Props.x + Props.PaddingSize; X <= Props.x2 - Props.PaddingSize; X++)
          {
            if (T >= X)
            {
              if ((X >= Low) && (X < Mid))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C3);
              else if ((X >= Mid) && (X < Hi))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C2);
              else if ((X >= Hi) && (X < Props.x2))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C1);
            }
            else
              _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.BC);
          }
        }
        else
        {
          for (int X = Props.x2 - Props.PaddingSize; X > Props.x + Props.PaddingSize; X--)
          {
            if (T <= X)
            {
              if ((X < Low) && (X >= Mid))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C3);
              else if ((X < Mid) && (X >= Hi))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C2);
              else if ((X < Hi) && (X >= Props.x))
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.C1);
            }
            else
              _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, Props.BC);
          }
        }
      }
      _Disp->setTextColor(Props.TouchColor);
      _Disp->setTextSize(Props.FontSize ? 2 : 1);
      if (strcmp(text, " "))
      {
        _Disp->setCursor(TX, TY);
        _Disp->print(text);
      }

      if (textEnable)
      {
        _Disp->setTextColor(Props.PercentageTouchColor, Props.PercentageTouchBackColor);
        //Restore_MainColor;
        _Disp->setTextSize(Props.PercentageFontSize ? 2 : 1);
        char buf[7];
        byte val = TFTE_Map(tmpGV, (orient ? Props.y2 : Props.x2), (orient ? Props.y : Props.x), 0, 100);

        if (orient == VERTICAL)
        {
          if (Props.Direction == TTB)
            val = 100 - val;
        }
        else
        {
          if (Props.Direction == LTR)
            val = 100 - val;
        }

        buf[0] = ' ';
        buf[1] = val < 100 ? ' ' : (val / 100) + '0';
        buf[2] = val < 10 ? ' ' : ((val / 10) % 10) + '0';
        buf[3] = (val % 10) + '0';
        buf[4] = '%';
        buf[5] = ' ';
        buf[6] = 0;

        _Disp->setCursor(P_TX, P_TY);
        _Disp->print(buf);
      }
      //Restore_MainColor;
    }


    void SetStartingValue(int SV)
    {
      if (SV == -1)
        T = (orient ? Props.y2 : Props.x2);
      else
      {
        if (SV > H) SV = H;
        if (SV < L) SV = L;
        T = TFTE_Map(SV, L, H, (orient ? Props.y2 : Props.x2), (orient ? Props.y : Props.x));
      }
    }

    void Padding(byte padding = 0, word padColor = WHITE)
    {
      Props.PaddingSize = padding;
      Props.PadColor = padColor;
    }

    void SetRange(long l = 0, long h = 10)
    {
      L = l;
      H = h;
    }

    void SetValue(int Val = -1)
    {
      if (Val == -1)
        Value = Val;
      else
      {
        int SL = (orient ? Props.y : Props.x);
        int SH = (orient ? Props.y2 : Props.x2);
        if (orient == VERTICAL)
        {
          if (Props.Direction == BTT)
            Swap(SL, SH);
        }
        else
        {
          if (Props.Direction == RTL)
            Swap(SL, SH);
        }

        Value = TFTE_Map(Val, L, H, SL, SH);
      }
    }

    int GetValue()
    {
      return Value;
    }

    void Unlock()
    {
      locked = false;
    }

    Properties getButtonProperties()
    {
      return Props;
    }

  private:
    int TX, TY, P_TX, P_TY;
    char * text;
    int Hi, Mid, Low, Off;
    bool locked;//1
    long L, H;//8
    int Value;//2
    int T;

};

class Arc
{
    struct Properties
    {
      int x;
      int y;
      int X_Radius;
      int Y_Radius;
      int X_Thickness, Y_Thickness;
      word Color;
      int StartAngle, StopAngle;
      bool Direction;
    } Props;

  public:
    Arc(Base * B) : _base(B)
    {
      _Disp = _base->getDisplay();
    }

    void Coords(int X, int Y, int Rad, int Thick)
    {
      Coords(X, Y, Rad, Rad, Thick, Thick);
    }

    void Coords(int X, int Y, int XRad, int YRad, int XThick, int YThick)
    {
      Props.x = X;
      Props.y = Y;
      Props.X_Radius = XRad;
      Props.X_Thickness = XThick;
      Props.Y_Radius = YRad;
      Props.Y_Thickness = YThick;
    }

    void Range(int Start, int Stop, int increments)
    {
      Props.StartAngle = Start + 4;
      Props.StopAngle = Stop;
      Props.Direction = true;

      if (Props.StartAngle < Props.StopAngle)
        Props.Direction = false;
    }

    void Colors(word Color)
    {
      Props.Color = Color;
    }

    void drawArc()
    {
      //Save_MainColor;

      for (int rotate = Props.StartAngle; rotate != Props.StopAngle; rotate += (Props.Direction ? -1 : 1))
      {
        float _cos1 = -cos(rotate * Deg_to_rad);
        float _sin1 = sin(rotate * Deg_to_rad);

        int Xo1 = Props.x + (_cos1 * Props.X_Radius);
        int Xi1 = Props.x + (_cos1 * (Props.X_Radius - Props.X_Thickness));

        int Yo1 = Props.y + (_sin1 * Props.Y_Radius);
        int Yi1 = Props.y + (_sin1 * (Props.Y_Radius - Props.Y_Thickness));
        //===============================================
        int fix;
        if (Props.Direction)
          fix = (rotate - 4 <= Props.StopAngle ? rotate : rotate - 4);
        else
          fix = (rotate + 4 > Props.StopAngle ? rotate : rotate + 4);

        float _cos2 = -cos(fix * Deg_to_rad);
        float _sin2 =  sin(fix * Deg_to_rad);

        int Xo2 = Props.x + (_cos2 * Props.X_Radius);
        int Xi2 = Props.x + (_cos2 * (Props.X_Radius - Props.X_Thickness));

        int Yo2 = Props.y + (_sin2 * Props.Y_Radius);
        int Yi2 = Props.y + (_sin2 * (Props.Y_Radius - Props.Y_Thickness));

        fillTriangle(Xo2, Yo2, Xo1, Yo1, Xi2, Yi2, Props.Color);
        fillTriangle(Xo1, Yo1, Xi1, Yi1, Xi2, Yi2, Props.Color);
      }
      //Restore_MainColor;
    }

    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int Color)
    {
      float A, B, C;
      int XL, XR;

      if (y1 > y2)
      {
        Swap(x1, x2);
        Swap(y1, y2);
      }

      if (y2 > y3)
      {
        Swap(x2, x3);
        Swap(y2, y3);
      }

      if (y1 > y2)
      {
        Swap(x1, x2);
        Swap(y1, y2);
      }

      A = (y2 - y1) ? (float(x2 - x1) / float(y2 - y1)) : (x2 - x1);
      B = (y3 - y1) ? (float(x3 - x1) / float(y3 - y1)) : (x3 - x1);
      C = (y3 - y2) ? (float(x3 - x2) / float(y3 - y2)) : (x3 - x2);

      for (int Dy = y1; Dy <= y3; Dy++)
      {
        if (Dy <= y2)
          XL = x1 + A * (Dy - y1);
        else
          XL = x2 + C * (Dy - y2);

        XR = x1 + B * (Dy - y1);

        if (XL > XR) // the drawFastHLine method does not like negative widths.
          Swap(XL, XR);

        _Disp->drawFastHLine(XL, Dy, (XR - XL), Color);
      }
    }

    Properties getButtonProperties()
    {
      return Props;
    }

  protected:
    bool orient;
    TFT_Display *_Disp;
    Base *_base;
};

class Gauge
{
    struct Properties
    {
      int x;
      int y;
      int Radius;
      int Thickness;
      word TouchColor, BackColor;
      int StartAngle, StopAngle, TextStartAngle, TextStopAngle, TextIncrements;
      bool Direction, Where;
      int Xoff, Yoff, TS;
      int TXoff, TYoff;
    } Props;
  public:

    Gauge(Base * B) : _base(B)
    {
      _Disp = _base->getDisplay();
      TextColor(WHITE, BLACK);
      HeaderText("", 0);
      FooterText("", 0);
    }

    void Coords(int X, int Y, int Radius, int Thickness)
    {
      Props.x = X;
      Props.y = Y;
      Props.Radius = Radius;
      Props.Thickness = Thickness;
    }

    void ArcDegrees(int Start, int Stop)
    {
      Props.StartAngle = Start;
      Props.StopAngle = Stop;
      Props.Direction = true;

      if (Props.StartAngle < Props.StopAngle)
        Props.Direction = false;

      GPosition = Props.StartAngle;
      ChangePercentage();
    }

    void TextColor(word C = WHITE, word BackColor = BLACK)
    {
      Props.TouchColor = C;
      Props.BackColor = BackColor;
    }

    void TextRange(int Start, int Stop, int inc, bool where, byte textSize)
    {
      Props.TextStartAngle = Start;
      Props.TextStopAngle = Stop;
      Props.TextIncrements = inc;
      Props.Where = where;
      numLen = max(numSize(Start), numSize(Stop));
      Props.TS = textSize;

      //Props.Xoff = (Props.x - ((5*Props.TS)*numLen)/2);
      //Props.Yoff = (Props.y - 7*Props.TS);
      TextOffset();
    }

    void TextOffset(int X = 0, int Y = 0)
    {
      Props.TXoff = X;
      Props.TYoff = Y;

      Props.Xoff = (Props.x - ((5 * Props.TS) * numLen) / 2) + Props.TXoff;
      Props.Yoff = (Props.y - 7 * Props.TS) + Props.TYoff;
    }

    void Colors(word Cfg, word Cbg)
    {
      cfg = Cfg;
      cbg = Cbg;
    }

    void ChangePercentage(int L = 0, int H = 100)
    {
      lP = L;
      hP = H;
    }

    void HeaderText(char * HT, byte size)
    {
      Htext = HT;
      HTsize = size;
    }

    void FooterText(char * FT, byte size)
    {
      Ftext = FT;
      FTsize = size;
    }

    void Draw(const byte TE = true)
    {
      _Disp->setTextColor(Props.TouchColor);
      _Disp->setTextSize(HTsize);
      _Disp->setCursor(Props.x - (strlen(Htext) * (3 * HTsize)) + Props.TXoff, Props.Yoff - (HTsize * HTsize) - HTsize + Props.TYoff);
      _Disp->print(Htext);
      _Disp->setTextSize(FTsize);
      _Disp->setCursor( Props.x - (strlen(Ftext) * (3 * FTsize)) + Props.TXoff, Props.y + (7 * FTsize) + Props.TS + Props.TYoff);
      _Disp->print(Ftext);

      if (TE)
        DrawText();

      EmptyGauge();
      lastPos = Props.StartAngle + (Props.Direction ? 1 : -1);
    }

    void Progress(byte val, byte TE = true)
    {
      char buf[4];
      unsigned int color;
      static int XoL, XiL, YoL, YiL;
      int output = 0;

      int rotate = TFTE_Map(val, lP, hP, Props.StartAngle, Props.StopAngle);
      if (TE)
        output = TFTE_Map(val, lP, hP, Props.TextStartAngle, Props.TextStopAngle);

      float _cos1 = -cos(rotate * Deg_to_rad);
      float _sin1 = sin(rotate * Deg_to_rad);

      int Xo = Props.x + (_cos1 * Props.Radius);
      int Xi = Props.x + (_cos1 * (Props.Radius - Props.Thickness));

      int Yo = Props.y + (_sin1 * Props.Radius);
      int Yi = Props.y + (_sin1 * (Props.Radius - Props.Thickness));
      //===============================================

      if (Props.Direction)
        color = (rotate > lastPos ? cbg : cfg);
      else
        color = (rotate < lastPos ? cbg : cfg);

      if (rotate < Props.StartAngle)
      {
        fillTriangle(XoL, YoL, Xi, Yi, XiL, YiL, color);
        fillTriangle(Xo, Yo, Xi, Yi, XoL, YoL, color);
      }

      XoL = Xo;
      XiL = Xi;
      YoL = Yo;
      YiL = Yi;

      lastPos = rotate;

      _Disp->setTextColor(Props.TouchColor, Props.BackColor);

      buf[0] =  output < 100 ? '0' : (output / 100) + '0';
      buf[1] = output < 10 ? '0' : ((output / 10) % 10) + '0';
      buf[2] = (output % 10) + '0';
      buf[3] = '\0';

      _Disp->setCursor(Props.x - (3 * Props.TS)*numLen + Props.TXoff, Props.y - 2 * Props.TS + Props.TYoff);
      _Disp->print(buf);
      GPosition = rotate;
    }

    void EmptyGauge()
    {
      static int XoL, XiL, YoL, YiL;
      for (long r = lP; r <= hP; r++)
      {
        int rotate = TFTE_Map(r, lP, hP, Props.StartAngle, Props.StopAngle);

        float _cos1 = -cos(rotate * Deg_to_rad);
        float _sin1 = sin(rotate * Deg_to_rad);

        int Xo = Props.x + (_cos1 * Props.Radius);
        int Xi = Props.x + (_cos1 * (Props.Radius - Props.Thickness));

        int Yo = Props.y + (_sin1 * Props.Radius);
        int Yi = Props.y + (_sin1 * (Props.Radius - Props.Thickness));
        //===============================================

        if (rotate < Props.StartAngle)
        {
          fillTriangle(XoL, YoL, Xo, Yo, XiL, YiL, (rotate > GPosition ? cfg : cbg));
          fillTriangle(Xo, Yo, Xi, Yi, XiL, YiL, (rotate > GPosition ? cfg : cbg));
        }

        XoL = Xo;
        XiL = Xi;
        YoL = Yo;
        YiL = Yi;
      }
    }

    void DrawText()
    {
      int X, Y;
      _Disp->setTextColor(Props.TouchColor);
      _Disp->setTextSize(Props.TS);
      for (int rotate = Props.TextStartAngle; rotate <= Props.TextStopAngle; rotate += (Props.TextIncrements == 0 ? 1 : Props.TextIncrements))
      {
        int output = TFTE_Map(rotate, Props.TextStartAngle, Props.TextStopAngle, Props.StartAngle, Props.StopAngle);
        float _cos = -cos(output * Deg_to_rad);
        float _sin = sin(output * Deg_to_rad);

        if (Props.Where)
        {
          X = Props.x - ((Props.TS * Props.TS) * numLen) + (_cos * (Props.Radius + (5 * Props.TS) * (numLen - 1)));
          Y = Props.y + (_sin * (Props.Radius + (5 * Props.TS)) - 7);
        }
        else
        {
          X = Props.x - ((4 * Props.TS) * (numLen - 1)) + (_cos * (Props.Radius - Props.Thickness - ((3 * Props.TS) * (numLen + 1))));
          Y = Props.y + (_sin * (Props.Radius - Props.Thickness - 3 * Props.TS));
        }
        _Disp->setCursor(X, Y);
        _Disp->print(rotate);
      }
    }

    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
    {
      float A, B, C;
      int XL, XR;

      if (y1 > y2)
      {
        Swap(x1, x2);
        Swap(y1, y2);
      }

      if (y2 > y3)
      {
        Swap(x2, x3);
        Swap(y2, y3);
      }

      if (y1 > y2)
      {
        Swap(x1, x2);
        Swap(y1, y2);
      }

      A = (y2 - y1) ? (float(x2 - x1) / float(y2 - y1)) : (x2 - x1);
      B = (y3 - y1) ? (float(x3 - x1) / float(y3 - y1)) : (x3 - x1);
      C = (y3 - y2) ? (float(x3 - x2) / float(y3 - y2)) : (x3 - x2);

      for (int Dy = y1; Dy <= y3; Dy++)
      {
        if (Dy <= y2)
          XL = x1 + A * (Dy - y1);
        else
          XL = x2 + C * (Dy - y2);

        XR = x1 + B * (Dy - y1);

        if (XL > XR)
          Swap(XL, XR);

        _Disp->drawFastHLine(XL, Dy, (XR - XL), color);
      }
    }

    Properties getButtonProperties()
    {
      return Props;
    }

  private:
    word cfg, cbg;
    int numSize(unsigned long x) {
      byte count = 0;
      while (x != 0) {
        x *= 0.1;
        count++;
      } return count;
    }
    int numLen;
    char * Htext, *Ftext;
    byte HTsize, FTsize;
    int lastPos, GPosition;
    int lP, hP;

  protected:
    TFT_Display *_Disp;
    Base *_base;
};

#endif