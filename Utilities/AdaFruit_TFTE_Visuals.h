#ifndef AdaFruit_TFTE_Visuals_h
#define AdaFruit_TFTE_Visuals_h

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
	
    void Coords(int x1, int y1, int x2, int y2)
	{
	  X1 = x1;
      Y1 = y1;
      X2 = x2;
      Y2 = y2;
	}
	
	void Attributes(word color, bool fontsize, char type, long bgc = 0xFFFFFFFF)
	{
	  Color = color;
	  BGC = bgc;
	  Type = type;
	  FS = fontsize;
	}
	
	void Attributes(char * _text, word color, bool fontsize, char filler = ' ', long bgc = 0xFFFFFFFF)
	{
	  Color = color;
	  BGC = bgc;
	  text = _text;
	  Type = '#';
	  Filler = filler;
	  FS = fontsize;
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
	  _Disp->setTextColor(Color, BGC);
	  
	  if(X2 == AUTO)
	    X2 = X1 + length*(FS? 16 : 8) + 10;
		
	  if(Y2 == AUTO)
	    Y2 = Y1 + (FS? 16 : 12) + 10;
	  
	  if(Type == '#')
	  {
	    centerBar = (X1 + X2)/2;
		
		Leftside = centerBar - length*(FS? 8 : 4);
	  }
	  
	  //Overwrite Leftside from above
	  if(X2 == AUTO)
	    Leftside = X1;

	  for (unsigned int L = X1, i = 0; L < X2 ; L++)
	  {
		switch (Type)
		{
		  case '/':
			_Disp->drawLine(L, Y1, L - 10, Y2, Color);
			break;
		  
		  case '\\':
			_Disp->drawLine(L - 10, Y1, L, Y2, Color);
			break;
			
		  case '>':
			_Disp->drawLine(L, Y1, L + 10, (Y1 + Y2)/2, Color);
			_Disp->drawLine(L + 10, (Y1 + Y2)/2, L, Y2, Color);
			break;
		  
		  case '|':
			_Disp->drawFastVLine(L, Y1, Y2 - Y1, Color);
			break;
          
		  case '#':	
		    if(L >= Leftside ) // && i < length
			{
			  if(i < length)
			  {
			    _Disp->setCursor(L, Y1);
		        _Disp->print(*text++);
				i++;
			  }
			  else 
			  {
			    _Disp->setCursor(L, Y1);
			    _Disp->print(Filler);
			  }
			}
			else 
			{
			  _Disp->setCursor(L, Y1);
			  _Disp->print(Filler);
			}
			
			L += (FS? 16 : 12)-1;
			break;
			
          default:
		    _Disp->setCursor(L, Y1);
            _Disp->print(Type);
			L += (FS? 16 : 12)-1;
			break;			
		}
		
		delay(50); // some delay
	  }
	}
	
	private:
      int X1, Y1, X2, Y2; //16
	  word Color;
	  long BGC;
	  char Type, *text, Filler;
	  bool FS;
};

class Meter : public Base
{
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
      X1 = x1;
      Y1 = y1;
      X2 = x2;
      Y2 = y2;
    }

    void Attributes(bool round, bool fill)
    {
      Round = round;
      Fill = fill;
    }

    void Colors(word c1, word c2, word c3, word bc)
    {
      C1 = c1;
      C2 = c2;
      C3 = c3;
      BC = bc;
    }

    void Percentages(byte hi, byte mid, byte low = 0)
    {
	  if(orient == VERTICAL)
	  {
		int T_height = ((Y2 - Y1) - (_padding * 2));
		if(Direction == BTT)
		{
		  Hi =  Y2 - (T_height * (float(hi) / 100.0));
		  Mid = Y2 - (T_height * (float(mid) / 100.0));
		  Low = Y2 - (T_height * (float(low) / 100.0));
		}
		else
		{
		  Hi = (T_height * (float(hi) / 100.0)) + Y1;
		  Mid = (T_height * (float(mid) / 100.0)) + Y1;
		  Low = (T_height * (float(low) / 100.0)) + Y1;
		}
      }
	  else
	  {
	    int T_width = ((X2 - X1) - (_padding * 2));
		if(Direction == RTL)
		{
		  Hi = X2 - (T_width * (float(hi) / 100.0));
		  Mid = X2 - (T_width * (float(mid) / 100.0));
		  Low = X2 - (T_width * (float(low) / 100.0));
		}
		else
		{
		  Hi = (T_width * (float(hi) / 100.0)) - X1;
		  Mid = (T_width * (float(mid) / 100.0)) - X1;
		  Low = (T_width * (float(low) / 100.0)) - X1;
		}
	  }
    }

    void SetDirection(bool Dir) // Not used at this time
    {
      Direction = Dir;
    }

    void Draw()
    {
      //Save_MainColor;
      if (_padding > 0)
      {
        if (!locked)
        {
          if (Fill)
            (Round ? _Disp->fillRoundRect(X1, Y1, X2-X1, Y2-Y1, 2, PadColor) : _Disp->fillRect(X1, Y1, X2-X1, Y2-Y1, PadColor));
          else
            (Round ? _Disp->drawRoundRect(X1, Y1, X2-X1, Y2-Y1, 2, PadColor) : _Disp->drawRect(X1, Y1, X2-X1, Y2-Y1, PadColor));
          locked = true;
        }

        if (Fill)
          (Round ? _Disp->fillRoundRect(X1 + _padding, Y1 + _padding, X2-X1 - _padding*2, Y2-Y1 - _padding*2, 2, BC) : _Disp->fillRect(X1 + _padding, Y1 + _padding, X2-X1 - _padding*2, Y2-Y1 - _padding*2, BC));
        else
          (Round ? _Disp->drawRoundRect(X1 + _padding, Y1 + _padding, X2-X1 - _padding*2, Y2-Y1 - _padding*2, 2, BC) : _Disp->drawRect(X1 + _padding, Y1 + _padding, X2-X1 - _padding*2, Y2-Y1 - _padding*2, BC));
      }
      else
      {
        if (Fill)
          (Round ? _Disp->fillRoundRect(X1, Y1, X2-X1, Y2-Y1, 2, BC) : _Disp->fillRect(X1, Y1, X2-X1, Y2-Y1, BC));
        else
          (Round ? _Disp->drawRoundRect(X1, Y1, X2-X1, Y2-Y1, 2, BC) : _Disp->drawRect(X1, Y1, X2-X1, Y2-Y1, BC));
      }

      _Disp->setTextColor(Tcolor, TBcolor);
      _Disp->setTextSize(textSize ? 2 : 1);
	  if(strcmp(text, " "))
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
      textSize = size;
      int Tsize = strlen(T) * (size ? 8 : 4);
      Tcolor = color;
      TBcolor = Bcolor;
	  
      TX = ((X1 + X2) / 2) - Tsize;
      switch (pos)
      {
        case TOP:
          TY = Y1 - 20;
          break;
        case MIDDLE:
          TY = ((Y1 + Y2) / 2) - 8;
          break;
        case BOTTOM:
          TY = Y2 + 20;
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
      P_textSize = size;
      int Tsize = 6 * (size ? 8 : 4);
      P_Tcolor = color;
      P_TBcolor = Bcolor;
	  
      P_TX = ((X1 + X2) / 2) - Tsize;
      switch (pos)
      {
        case TOP:
          P_TY = Y1 - 20;
          break;
        case MIDDLE:
          P_TY = ((Y1 + Y2) / 2) - 8;
          break;
        case BOTTOM:
          P_TY = Y2 + 5;
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
		
      if(orient == VERTICAL)
	  {
		  if(Direction == BTT)
		  {
			//BTT
			for (int Y = Y2 - _padding; Y >= Y1 + _padding; Y--)
			{
			  if (T <= Y)
			  {
				if((Y <= Low) && (Y > Mid))
				  _Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C3);				
				else if((Y <= Mid) && (Y > Hi))
				  _Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C2);
				else if((Y <= Hi) && (Y > Y1))
				  _Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C1);
			  }
			  else
				_Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, BC);	  
			}
		  }
		  else
		  {
			  for (int Y = Y1 + _padding; Y < Y2 - _padding; Y++)
			  {
				if (T >= Y)
				{
				  if((Y > Low) && (Y <= Mid)) 
					_Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C3);	
				  else if((Y > Mid) && (Y <= Hi))
					_Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C2);	
				  else if((Y > Hi) && (Y <= Y2))
					_Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, C1);	
				}
				else
				  _Disp->drawFastHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2, BC);	
			  }
		  }
	  }
	  else
	  {
	    if(Direction == LTR)
		  {
			//LTR
			for (int X = X1 + _padding; X <= X2 - _padding; X++)
			{
			  if (T >= X)
			  {
				if((X >= Low) && (X < Mid)) 
				  _Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C3);
				else if((X >= Mid) && (X < Hi))
				  _Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C2);
				else if((X >= Hi) && (X < X2))
				  _Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C1);
			  }
			  else
				_Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, BC);
			}
		  }
		  else
		  {
			  for (int X = X2 - _padding; X > X1 + _padding; X--)
			  {
				if (T <= X)
				{
				  if((X < Low) && (X >= Mid)) 
					_Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C3);
				  else if((X < Mid) && (X >= Hi))
					_Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C2);
				  else if((X < Hi) && (X >= X1))
					_Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, C1);
				}
				else
				  _Disp->drawFastVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2, BC);
			  }
		  }
	  }
      _Disp->setTextColor(Tcolor);
      _Disp->setTextSize(textSize ? 2 : 1);
	  if(strcmp(text, " "))
	  {
	    _Disp->setCursor(TX, TY);
        _Disp->print(text);
	  }
    
	  if(textEnable)
	  {
        _Disp->setTextColor(P_Tcolor, P_TBcolor);
		//Restore_MainColor;
        _Disp->setTextSize(P_textSize ? 2 : 1);
        char buf[7];
		byte val = Map(tmpGV, (orient ? Y2 : X2), (orient ? Y1 : X1), 0, 100);
		
		if(orient == VERTICAL)
		{
		  if(Direction == TTB)
		    val = 100 - val;
		}
		else
		{
		  if(Direction == LTR)
		    val = 100 - val;
		}
			
		buf[0] = ' ';
		buf[1] = val < 100? ' ' : (val / 100) + '0';
		buf[2] = val < 10? ' ' : ((val / 10) % 10) + '0';
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
        T = (orient ? Y2 : X2);
      else
      {
        if (SV > H) SV = H;
        if (SV < L) SV = L;
        T = Map(SV, L, H, (orient ? Y2 : X2), (orient ? Y1 : X1));
      }
    }

    void Padding(byte padding = 0, word padColor = WHITE)
    {
      _padding = padding;
      PadColor = padColor;
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
        int SL = (orient ? Y1 : X1);
        int SH = (orient ? Y2 : X2);
        if(orient == VERTICAL)
		{
          if (Direction == BTT)
            Swap(SL, SH);
		}
		else
		{
		  if (Direction == RTL)
            Swap(SL, SH);
		}

        Value = Map(Val, L, H, SL, SH);
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

  private:
    int X1, Y1, X2, Y2; //16
    byte _padding;//1
    word C1, C2, C3, BC, PadColor, Tcolor, P_Tcolor;//12
	long TBcolor, P_TBcolor;
    int TX, TY, P_TX, P_TY;
    char * text;
    int Hi, Mid, Low, Off;
    bool Round, Fill, Direction;//3
    bool locked;//1
    long L, H;//8
    int Value;//2
    int T;
    byte textSize, P_textSize;
};

class Arc
{
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
		  cx = X; 
		  cy = Y; 
		  Xradius = XRad; 
		  Xthickness = XThick;
		  Yradius = YRad; 
		  Ythickness = YThick;
		}

		void Range(int Start, int Stop, int increments)
		{
		  start = Start+4;
		  stop = Stop;
		  dir = true;
	  
	      if(start < stop)
	        dir = false;
		}

		void Colors(word Color)
		{
		  color = Color;
		}

		void drawArc()
		{
		  //Save_MainColor;
	
		  for(int rotate = start; rotate != stop; rotate+=(dir? -1 : 1))
		  {
			float _cos1 = -cos(rotate * Deg_to_rad);
			float _sin1 = sin(rotate * Deg_to_rad);

			int Xo1 = cx + (_cos1 * Xradius);
			int Xi1 = cx + (_cos1 * (Xradius - Xthickness));

			int Yo1 = cy + (_sin1 * Yradius);
			int Yi1 = cy + (_sin1 * (Yradius - Ythickness));
		//===============================================
			int fix;
			if(dir)
			  fix = (rotate-4 <= stop? rotate : rotate-4);
			else 
			  fix = (rotate+4 > stop? rotate : rotate+4);
			
			float _cos2 = -cos(fix * Deg_to_rad);
			float _sin2 =  sin(fix * Deg_to_rad);

			int Xo2 = cx + (_cos2 * Xradius);
			int Xi2 = cx + (_cos2 * (Xradius - Xthickness));

			int Yo2 = cy + (_sin2 * Yradius);
			int Yi2 = cy + (_sin2 * (Yradius - Ythickness));

			fillTriangle(Xo2, Yo2, Xo1, Yo1, Xi2, Yi2, color);
			fillTriangle(Xo1, Yo1, Xi1, Yi1, Xi2, Yi2, color);
		  }
		  //Restore_MainColor;
		}

		void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color)
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
			
            if(XL > XR) // the drawFastHLine method does not like negative widths.
		      Swap(XL,XR);
			  
			_Disp->drawFastHLine(XL, Dy, (XR - XL), color);
		  }
		}
		
	private:
		int cx; 
		int cy; 
		int Xradius, Yradius;
		int Xthickness, Ythickness;
		int start;
		int stop;
		word color;
		bool dir;
		
	
	protected:
	    bool orient;
		TFT_Display *_Disp;
		Base *_base;
};

class Gauge
{
  public:

    Gauge(Base * B) : _base(B)
	{ 
	  _Disp = _base->getDisplay();
	  TextColor(WHITE, BLACK);
	  HeaderText("", 0);
	  FooterText("", 0);
	}
	
	void Coords(int X,int Y, int Radius, int Thickness)
	{
	  cx = X;
	  cy = Y;
	  radius = Radius;
	  thick = Thickness;
	}
	
	void ArcDegrees(int Start, int Stop)
	{
	  start = Start;
	  stop = Stop;
	  dir = true;
	  
	  if(start < stop)
	    dir = false;
	  
	  GPosition = start;
	  ChangePercentage();
	}
    
	void TextColor(word C = WHITE, word BC = BLACK)
	{
	  TC = C;
	  BC = BC;
	}
	
	void TextRange(int Start, int Stop, int inc, bool where, byte textSize)
	{
	  Tstart = Start;
	  Tstop = Stop;
	  Tinc = inc;
	  Where = where;
	  numLen = max(numSize(Start), numSize(Stop));
	  TS = textSize;
	  
	  //Xoff = (cx - ((5*TS)*numLen)/2);
	  //Yoff = (cy - 7*TS);
	  TextOffset();
	}

	void TextOffset(int X = 0, int Y = 0)
	{
	  TXoff = X;
	  TYoff = Y; 
	  	  
	  Xoff = (cx - ((5*TS)*numLen)/2) + TXoff;
	  Yoff = (cy - 7*TS) + TYoff;
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
	  _Disp->setTextColor(TC);
	  _Disp->setTextSize(HTsize);
	  _Disp->setCursor(cx - (strlen(Htext)*(3*HTsize)) + TXoff, Yoff - (HTsize*HTsize) - HTsize + TYoff);
	  _Disp->print(Htext);
	  _Disp->setTextSize(FTsize);
	  _Disp->setCursor( cx - (strlen(Ftext)*(3*FTsize)) + TXoff, cy + (7*FTsize) + TS + TYoff);
	  _Disp->print(Ftext);
	  
	  if(TE)
	    DrawText();

	  EmptyGauge();
	  lastPos = start + (dir? 1: -1);
	}
	
	void Progress(byte val, byte TE = true)
	{
	  char buf[4];
      unsigned int color;
	  static int XoL, XiL, YoL,YiL;
	  int output = 0;

	  int rotate = Map(val, lP, hP, start, stop);
	  if(TE)
	    output = Map(val, lP, hP, Tstart, Tstop);

	  float _cos1 = -cos(rotate * Deg_to_rad);
	  float _sin1 = sin(rotate * Deg_to_rad);

	  int Xo = cx + (_cos1 * radius);
	  int Xi = cx + (_cos1 * (radius - thick));

	  int Yo = cy + (_sin1 * radius);
	  int Yi = cy + (_sin1 * (radius - thick));
	//===============================================
	  
	  if(dir)
	    color = (rotate > lastPos ? cbg : cfg);
	  else
	    color = (rotate < lastPos ? cbg : cfg);
      
	  if(rotate < start)
	  {
	    fillTriangle(XoL, YoL, Xi, Yi, XiL, YiL, color);
	    fillTriangle(Xo, Yo, Xi, Yi, XoL, YoL, color);
	  }
	  
	  XoL = Xo;
	  XiL = Xi;
	  YoL = Yo;
	  YiL = Yi;

	  lastPos = rotate;
	  
	  _Disp->setTextColor(TC, BC);
	  
	  buf[0] =  output < 100? '0' : (output / 100) + '0';
	  buf[1] = output < 10? '0' : ((output / 10) % 10) + '0';
	  buf[2] = (output % 10) + '0';
	  buf[3] = '\0';
      
	  _Disp->setCursor(cx - (3*TS)*numLen + TXoff, cy - 2*TS + TYoff);
	  _Disp->print(buf);
	  GPosition = rotate;
	}

	void EmptyGauge()
	{
	  static int XoL, XiL, YoL,YiL;
	  for(long r = lP; r <= hP; r++)
	  {
	    int rotate = Map(r, lP, hP, start, stop);
	
	    float _cos1 = -cos(rotate * Deg_to_rad);
	    float _sin1 = sin(rotate * Deg_to_rad);

	    int Xo = cx + (_cos1 * radius);
	    int Xi = cx + (_cos1 * (radius - thick));

   	    int Yo = cy + (_sin1 * radius);
	    int Yi = cy + (_sin1 * (radius - thick));
		//===============================================

		if(rotate < start)
		{
		  fillTriangle(XoL, YoL, Xo, Yo, XiL, YiL, (rotate > GPosition? cfg:cbg));
		  fillTriangle(Xo, Yo, Xi, Yi, XiL, YiL, (rotate > GPosition? cfg:cbg));
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
	  _Disp->setTextColor(TC);
	  _Disp->setTextSize(TS);
	  for (int rotate = Tstart; rotate <= Tstop; rotate += (Tinc == 0 ? 1 : Tinc))
	  {
        int output = Map(rotate, Tstart, Tstop, start, stop);
		float _cos = -cos(output * Deg_to_rad);
		float _sin = sin(output * Deg_to_rad);

		if(Where)
		{
		  X = cx-((TS*TS)*numLen) + (_cos * (radius + (5*TS)*(numLen-1)));
		  Y = cy + (_sin * (radius + (5*TS))-7);
		}
		else
		{
		  X = cx-((4*TS)*(numLen-1)) + (_cos * (radius - thick - ((3*TS)*(numLen+1))));
		  Y = cy + (_sin * (radius - thick - 3*TS));
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

		if(XL > XR)
		  Swap(XL,XR);
		  
		_Disp->drawFastHLine(XL, Dy, (XR - XL), color);
	  }
	}
	private:
		int cx; 
		int cy; 
		int radius;
		int thick;
		int start;
		int stop;
		bool dir;
		word cfg, cbg, TC, BC;
		int Tstart;
		int Tstop;
		int Tinc;
		bool Where;
        int numSize(unsigned long x) {byte count = 0; while(x != 0){ x*=0.1; count++;} return count;}
		int numLen;
		int Xoff, Yoff, TS;
		char * Htext, *Ftext;
		byte HTsize, FTsize;
		int lastPos, GPosition;
		int TXoff,TYoff;
		int lP, hP;
		
			
	protected:
	    TFT_Display *_Disp;
        Base *_base;
};

#endif