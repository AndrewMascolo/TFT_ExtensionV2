#ifndef TFTE_Visuals_h
#define TFTE_Visuals_h

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
  protected:
    TFT_Display *_Disp;
    Base *_base;

  public:
    ProgressBar(Base * B) : _base(B)
    {
      _Disp = _base->getDisplay();
    }
	
	~ProgressBar(){ }
	
	struct Properties
	{
	  int x;
	  int y;
	  int width;
	  int height;
	  int x2;
	  int y2;
	  word fColor;
	  long bColor;
	  char type;
	  bool Fs;
	}Props;
	
    void Coords(int x1, int y1, int x2, int y2)
	{
	  Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
	  Props.height = y2 - y1;
	  Props.width = x2 - x1;
	}
	
	void Attributes(word color, bool fontsize, char type, long bgc = 0xFFFFFFFF)
	{
	  Props.fColor = color;
	  Props.bColor = bgc;
	  Props.type = type;
	  Props.Fs = fontsize;
	}
	
	void Attributes(char * _text, word color, bool fontsize, char filler = ' ', long bgc = 0xFFFFFFFF)
	{
	  Props.fColor = color;
	  Props.bColor = bgc;
	  text = _text;
	  Props.type = '#';
	  Filler = filler;
	  Props.Fs = fontsize;
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
	
	void Progress()
	{
	  unsigned int centerBar, Leftside, length = strlen(text);
	  _Disp->setColor(Props.fColor);
	  _Disp->setBackColor(Props.bColor);
	  
	  if(Props.x2 == AUTO)
	    Props.x2 = Props.x + length*(Props.Fs? 16 : 8) + 10;
		
	  if(Props.y2 == AUTO)
	    Props.y2 = Props.y + (Props.Fs? 16 : 12) + 10;
	  
	  if(Props.type == '#')
	  {
	    centerBar = (Props.x + Props.x2)/2;
		
		Leftside = centerBar - length*(Props.Fs? 8 : 4);
	  }
	  
	  //Overwrite Leftside from above
	  if(Props.x2 == AUTO)
	    Leftside = Props.x;

	  for (unsigned int L = Props.x, i = 0; L < Props.x2 ; L++)
	  {
		switch (Props.type)
		{
		  case '/':
			_Disp->drawLine(L, Props.y, L - 10, Props.y2);
			break;
		  
		  case '\\':
			_Disp->drawLine(L - 10, Props.y, L, Props.y2);
			break;
			
		  case '>':
			_Disp->drawLine(L, Props.y, L + 10, (Props.y + Props.y2)/2);
			_Disp->drawLine(L + 10, (Props.y + Props.y2)/2, L, Props.y2);
			break;
		  
		  case '|':
			_Disp->drawVLine(L, Props.y, Props.y2 - Props.y);
			break;
          
		  case '#':	
		    if(L >= Leftside ) // && i < length
			{
			  if(i < length)
			  {
		        _Disp->printChar(*text++, L, Props.y);
				i++;
			  }
			  else 
			    _Disp->printChar(Filler, L, Props.y);
			}
			else 
			  _Disp->printChar(Filler, L, Props.y);
			
			L += (Props.Fs? 16 : 12)-1;
			break;
			
          default:
            _Disp->printChar(Props.type, L, Props.y);
			L += (Props.Fs? 16 : 12)-1;
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
	  word C1, C2, C3, BC, PadColor, TouchColor, HFcolor, PercentageTouchColor;
	  long TouchBackColor, PercentageTouchBackColor;
	  bool Round, Fill, Direction;
	  byte PaddingSize, FontSize, PercentageFontSize;
	}Props;
	
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

	~Meter(){ }
	
    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
	  Props.width = x2 - x1;
	  Props.height = y2 - y1;
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
	  if(orient == VERTICAL)
	  {
		int T_height = ((Props.y2 - Props.y) - (Props.PaddingSize * 2));
		if(Props.Direction == BTT)
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
		if(Props.Direction == RTL)
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
      extern uint8_t SmallFont[];
      extern uint8_t BigFont[];
      Save_MainColor;
      if (Props.PaddingSize > 0)
      {
        if (!locked)
        {
          _Disp->setColor(Props.PadColor);
          if (Props.Fill)
            (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2, Props.y2) : _Disp->fillRect(Props.x, Props.y, Props.x2, Props.y2));
          else
            (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2, Props.y2) : _Disp->drawRect(Props.x, Props.y, Props.x2, Props.y2));
          locked = true;
        }

        _Disp->setColor(Props.BC);
        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.PaddingSize, Props.y2 - Props.PaddingSize) : _Disp->fillRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.PaddingSize, Props.y2 - Props.PaddingSize));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.PaddingSize, Props.y2 - Props.PaddingSize) : _Disp->drawRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2 - Props.PaddingSize, Props.y2 - Props.PaddingSize));
      }
      else
      {
        _Disp->setColor(Props.BC);
        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2, Props.y2) : _Disp->fillRect(Props.x, Props.y, Props.x2, Props.y2));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2, Props.y2) : _Disp->drawRect(Props.x, Props.y, Props.x2, Props.y2));
      }

      _Disp->setColor(Props.TouchColor);
	  _Disp->setBackColor(Props.TouchBackColor);
      _Disp->setFont(Props.FontSize ? BigFont : SmallFont);
	  if(strcmp(text, " "))
        _Disp->print(text, TX, TY);

      Restore_MainColor;
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
	  Save_MainColor;
      int Out = -1;
      int tmpGV = -1;

      if ((tmpGV = GetValue()) != -1)
        T = tmpGV;
		
      if(orient == VERTICAL)
	  {
		  if(Props.Direction == BTT)
		  {
			//BTT
			for (int Y = Props.y2 - Props.PaddingSize; Y >= Props.y + Props.PaddingSize; Y--)
			{
			  if (T <= Y)
			  {
				if((Y <= Low) && (Y > Mid)) 
				  _Disp -> setColor(Props.C3);
				else if((Y <= Mid) && (Y > Hi))
				  _Disp -> setColor(Props.C2);
				else if((Y <= Hi) && (Y > Props.y))
				  _Disp -> setColor(Props.C1);
			  }
			  else
				_Disp->setColor(Props.BC);

			  _Disp->drawHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2);
			}
		  }
		  else
		  {
			  for (int Y = Props.y + Props.PaddingSize; Y < Props.y2 - Props.PaddingSize; Y++)
			  {
				if (T >= Y)
				{
				  if((Y > Low) && (Y <= Mid)) 
					_Disp -> setColor(Props.C3);
				  else if((Y > Mid) && (Y <= Hi))
					_Disp -> setColor(Props.C2);
				  else if((Y > Hi) && (Y <= Props.y2))
					_Disp -> setColor(Props.C1);
				}
				else
				  _Disp->setColor(Props.BC);

				_Disp->drawHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2);
			  }
		  }
	  }
	  else
	  {
	    if(Props.Direction == LTR)
		  {
			//LTR
			for (int X = Props.x + Props.PaddingSize; X <= Props.x2 - Props.PaddingSize; X++)
			{
			  if (T >= X)
			  {
				if((X >= Low) && (X < Mid)) 
				  _Disp -> setColor(Props.C3);
				else if((X >= Mid) && (X < Hi))
				  _Disp -> setColor(Props.C2);
				else if((X >= Hi) && (X < Props.x2))
				  _Disp -> setColor(Props.C1);
			  }
			  else
				_Disp->setColor(Props.BC);

			  _Disp->drawVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2);
			}
		  }
		  else
		  {
			  for (int X = Props.x2 - Props.PaddingSize; X > Props.x + Props.PaddingSize; X--)
			  {
				if (T <= X)
				{
				  if((X < Low) && (X >= Mid)) 
					_Disp -> setColor(Props.C3);
				  else if((X < Mid) && (X >= Hi))
					_Disp -> setColor(Props.C2);
				  else if((X < Hi) && (X >= Props.x))
					_Disp -> setColor(Props.C1);
				}
				else
				  _Disp->setColor(Props.BC);

				_Disp->drawVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2);
			  }
		  }
	  }
      _Disp->setColor(Props.TouchColor);
      _Disp->setBackColor(0xFFFFFFFF);
      _Disp->setFont(Props.FontSize ? BigFont : SmallFont);
	  if(strcmp(text, " "))
        _Disp->print(text, TX, TY);
    
	  if(textEnable)
	  {
        _Disp->setColor(Props.PercentageTouchColor);
        _Disp->setBackColor(Props.PercentageTouchBackColor);
        _Disp->setFont(Props.PercentageFontSize ? BigFont : SmallFont);
        char buf[7];
		byte val = TFTE_Map(tmpGV, (orient ? Props.y2 : Props.x2), (orient ? Props.y : Props.x), 0, 100);
		
		if(orient == VERTICAL)
		{
		  if(Props.Direction == TTB)
		    val = 100 - val;
		}
		else
		{
		  if(Props.Direction == LTR)
		    val = 100 - val;
		}
			
		buf[0] = ' ';
		buf[1] = val < 100? ' ' : (val / 100) + '0';
		buf[2] = val < 10? ' ' : ((val / 10) % 10) + '0';
		buf[3] = (val % 10) + '0';
		buf[4] = '%';
		buf[5] = ' ';
		buf[6] = 0;

        _Disp->print(buf, P_TX, P_TY);
	  }
	  Restore_MainColor;
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
        if(orient == VERTICAL)
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
	  long BackColor;
      int StartAngle, StopAngle, Increments;
      bool Direction, Lines;
    } Props;
	
	public:
		Arc(Base * B) : _base(B)
		{ 
			_Disp = _base->getDisplay();
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

		void Coords(int X, int Y, int Rad, int Thick)
		{
		  Coords(X, Y, Rad, Rad, Thick, Thick);
		}
		
		void Range(int Start, int Stop, int increments)
		{
		  Props.StartAngle = Start;
		  Props.StopAngle = Stop;
		  Props.Increments = increments;
		  Props.Direction = true;
		  if(Props.StartAngle < Props.StopAngle)
		    Props.Direction = false;
		}

		void Colors(word Color, long BackColor = 0xffffffff)
		{
		  Props.Color = Color;
		  Props.BackColor = BackColor;
		}

		void drawArc()
		{
		  Save_MainColor;
	
		  for(int rotate = Props.StartAngle ; rotate != Props.StopAngle; rotate+=(Props.Direction? -1 : 1))
		  {
			_Disp->setColor(Props.Color); 
			_Disp->setBackColor(Props.BackColor);
			float _cos1 = -cos(rotate * Deg_to_rad);
			float _sin1 = sin(rotate * Deg_to_rad);

			int Xo1 = Props.x + (_cos1 * Props.X_Radius);
			int Xi1 = Props.x + (_cos1 * (Props.X_Radius - Props.X_Thickness));

			int Yo1 = Props.y + (_sin1 * Props.Y_Radius);
			int Yi1 = Props.y + (_sin1 * (Props.Y_Radius - Props.Y_Thickness));
		//===============================================
			int fix;
			if(Props.Direction)
			  fix = (rotate-4 <= Props.StopAngle? rotate : rotate-4);
			else 
			  fix = (rotate+4 > Props.StopAngle? rotate : rotate+4);
			
			float _cos2 = -cos(fix * Deg_to_rad);
			float _sin2 =  sin(fix * Deg_to_rad);

			int Xo2 = Props.x + (_cos2 * Props.X_Radius);
			int Xi2 = Props.x + (_cos2 * (Props.X_Radius - Props.X_Thickness));

			int Yo2 = Props.y + (_sin2 * Props.Y_Radius);
			int Yi2 = Props.y + (_sin2 * (Props.Y_Radius - Props.Y_Thickness));

			fillTriangle(Xo2, Yo2, Xo1, Yo1, Xi2, Yi2);
			fillTriangle(Xo1, Yo1, Xi1, Yi1, Xi2, Yi2);
		  }
		  Restore_MainColor;
		}

		void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
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

			_Disp->drawHLine(XL, Dy, (XR - XL));
		  }
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
    
	#if defined(TFT_ScaleFonts_h)
	Gauge(Base * B, TFT_ScaleFonts * TFTfs) : _base(B), Sfonts(TFTfs)
	{ 
	  _Disp = _base->getDisplay();
	  TextColor(WHITE);
	  HeaderText("", 0);
	  FooterText("", 0);
	}
    #else
    Gauge(Base * B) : _base(B)
	{ 
	  _Disp = _base->getDisplay();
	  TextColor(WHITE);
	  HeaderText("", 0);
	  FooterText("", 0);
	}
    #endif
	
	void Coords(int X,int Y, int Radius, int Thickness)
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
	  
	  if(Props.StartAngle < Props.StopAngle)
	    Props.Direction = false;
	  
	  GPosition = Props.StartAngle;
	  ChangePercentage();
	}
	
	void TextColor(word C = WHITE)
	{
	  Props.TouchColor = C;
	}

	void TextRange(int Start, int Stop, int inc, bool where = 0, byte textSize=1)
	{
	  Props.TextStartAngle = Start;
	  Props.TextStopAngle = Stop;		
	  Props.TextIncrements = inc;
		
	  Props.Where = where;
	  numLen = max(numSize(Start), numSize(Stop));
	
	#if defined TFT_ScaleFonts_h
	  Props.TS = textSize;
	#else 
	  Props.TS = 1;
	#endif
	
	  TextOffset(0, 0);
	}
	
	void TextOffset(int X = 0, int Y = 0)
	{
	  Props.TXoff = X;
	  Props.TYoff = Y; 
	  	  
	  Props.Xoff = (Props.x - ((_Disp->getFontXsize()*Props.TS)*numLen)/2) + Props.TXoff;
	  Props.Yoff = (Props.y - ((_Disp->getFontYsize()/2)*Props.TS)) + Props.TYoff;
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
	
	void HeaderText(char * HT= "", byte size = 0)
	{
	  Htext = HT;
	  HTsize = size;
	}
	
    void FooterText(char * FT = "", byte size = 0)
	{
	  Ftext = FT;
	  FTsize = size;
	}
	
	void Draw(const byte TE = true)
	{
	  Save_MainColor;
	  _Disp->setColor(Props.TouchColor);
	  _Disp->setBackColor((_Disp->_transparent? 0xffffffff : _Disp->getBackColor()));
	  #if defined TFT_ScaleFonts_h
	  Sfonts->print(Htext, Props.x - (strlen(Htext)*(_Disp->getFontXsize()/2)*HTsize) + Props.TXoff, Props.Yoff - (_Disp->getFontYsize()*HTsize) + Props.TYoff, HTsize);
	  Sfonts->print(Ftext, Props.x - (strlen(Ftext)*(_Disp->getFontXsize()/2)*FTsize) + Props.TXoff, Props.y + ((_Disp->getFontYsize()/2)*Props.TS)+ Props.TYoff, FTsize);
	  #else
	  _Disp->print(Htext, Props.x - (strlen(Htext)*_Disp->getFontXsize()/2) + Props.TXoff, Props.Yoff - _Disp->getFontYsize() + Props.TYoff);
	  _Disp->print(Ftext, Props.x - (strlen(Ftext)*_Disp->getFontXsize()/2) + Props.TXoff, Props.y + (_Disp->getFontYsize()/2) + Props.TYoff);
	  #endif
	  
	  if(TE)
	    DrawText();

	  EmptyGauge();
	  lastPos = Props.StartAngle + (Props.Direction? 1: -1);
	  Restore_MainColor;
	}
		
	void Progress(byte val, byte TE = true)
	{
	  char buf[4];
      unsigned int color;
	  static int XoL, XiL, YoL,YiL;
	  int output = 0;

	  int rotate = TFTE_Map(val, lP, hP, Props.StartAngle, Props.StopAngle);
	  if(TE)
	    output = TFTE_Map(val, lP, hP, Props.TextStartAngle, Props.TextStopAngle);

	  float _cos1 = -cos(rotate * Deg_to_rad);
	  float _sin1 = sin(rotate * Deg_to_rad);

	  int Xo = Props.x + (_cos1 * Props.Radius);
	  int Xi = Props.x + (_cos1 * (Props.Radius - Props.Thickness));

	  int Yo = Props.y + (_sin1 * Props.Radius);
	  int Yi = Props.y + (_sin1 * (Props.Radius - Props.Thickness));
	//===============================================
	  
	  if(Props.Direction)
	    color = (rotate > lastPos ? cbg : cfg);
	  else
	    color = (rotate < lastPos ? cbg : cfg);
      
	  _Disp->setColor(color);
	  if(rotate < Props.StartAngle)
	  {
		fillTriangle(XoL, YoL, Xi, Yi, XiL, YiL);
		fillTriangle(Xo, Yo, Xi, Yi, XoL, YoL);
	  }
	  XoL = Xo;
	  XiL = Xi;
	  YoL = Yo;
	  YiL = Yi;

	  lastPos = rotate;
	  if(TE)
	  {
	    _Disp->setColor(Props.TouchColor);
	    _Disp->setBackColor((_Disp->_transparent? 0xffffffff : _Disp->getBackColor()));
	    #if defined(TFT_ScaleFonts_h)
	    Sfonts->printNumI(output, Props.Xoff, Props.Yoff , numLen, Props.TS, '0');
	    #else
	    _Disp->printNumI(output, Props.x - 8*numLen + Props.TXoff, Props.y - (_Disp->getFontXsize()/2) + Props.TYoff, numLen,'0');
	    #endif
	  }
	  GPosition = rotate;
	  Restore_MainColor;
	}
	
	void EmptyGauge()
	{
	  Save_MainColor;
	  static int XoL, XiL, YoL,YiL;
	  for(byte r = lP; r <= hP; r++)
	  {
	      int rotate = TFTE_Map(r, lP, hP, Props.StartAngle, Props.StopAngle);
          _Disp->setColor((rotate >= GPosition? cfg:cbg));
		  float _cos1 = -cos(rotate * Deg_to_rad);
		  float _sin1 = sin(rotate * Deg_to_rad);

		  int Xo = Props.x + (_cos1 * Props.Radius);
		  int Xi = Props.x + (_cos1 * (Props.Radius - Props.Thickness));

		  int Yo = Props.y + (_sin1 * Props.Radius);
		  int Yi = Props.y + (_sin1 * (Props.Radius - Props.Thickness));
		//===============================================
		  if(rotate < Props.StartAngle)
		  {
			fillTriangle(XoL, YoL, Xi, Yi, XiL, YiL);
			fillTriangle(Xo, Yo, Xi, Yi, XoL, YoL);
		  }
		  XoL = Xo;
		  XiL = Xi;
		  YoL = Yo;
		  YiL = Yi;

	  }
	  Restore_MainColor;
	}
	
	void DrawText()
	{
	  int X, Y;
      _Disp->setBackColor((_Disp->_transparent? 0xffffffff : _Disp->getBackColor()));
	  _Disp->setColor(Props.TouchColor);
	  for (int rotate = Props.TextStartAngle; rotate <= Props.TextStopAngle; rotate += (Props.TextIncrements == 0 ? 1 : Props.TextIncrements))
	  {
        int output = TFTE_Map(rotate, Props.TextStartAngle, Props.TextStopAngle, Props.StartAngle, Props.StopAngle);
		float _cos = -cos(output * Deg_to_rad);
		float _sin = sin(output * Deg_to_rad);

		if(Props.Where)
		{
		  X = Props.x-(8*numLen) + (_cos * (Props.Radius + 16*(numLen-1)));
		  Y = Props.y + (_sin * (Props.Radius + 20 - 16*(((output < 0) || (output > 180))? sin(output * Deg_to_rad) : 0)));
		}
		else
		{
		  X = Props.x-(8*(numLen+1)) + (_cos * (Props.Radius - Props.Thickness - (8*(numLen))));
		  Y = Props.y + (_sin * (Props.Radius - Props.Thickness - 5));
		}
		  _Disp->printNumI(rotate, X, Y, numLen);
	  }
	}

	void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
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

		_Disp->drawHLine(XL, Dy, (XR - XL));
	  }
	}
	
	Properties getButtonProperties()
	{
	  return Props;
	}
	
	private:
		word cfg, cbg;
        int numSize(unsigned long x) {byte count = 0; while(x != 0){ x*=0.1; count++;} return count;}
		int numLen;
		char * Htext, *Ftext;
		byte HTsize, FTsize;
		int lastPos, GPosition;
		int lP, hP;
			
	protected:
	    TFT_Display *_Disp;
        Base *_base;
		#if defined(TFT_ScaleFonts_h)
		TFT_ScaleFonts * Sfonts;
		#endif
};

#endif