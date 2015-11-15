#ifndef TFTE_Visuals_h
#define TFTE_Visuals_h

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
	
	void Progress()
	{
	  unsigned int centerBar, Leftside, length = strlen(text);
	  _Disp->setColor(Color);
	  _Disp->setBackColor(BGC);
	  
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
			_Disp->drawLine(L, Y1, L - 10, Y2);
			break;
		  
		  case '\\':
			_Disp->drawLine(L - 10, Y1, L, Y2);
			break;
			
		  case '>':
			_Disp->drawLine(L, Y1, L + 10, (Y1 + Y2)/2);
			_Disp->drawLine(L + 10, (Y1 + Y2)/2, L, Y2);
			break;
		  
		  case '|':
			_Disp->drawVLine(L, Y1, Y2 - Y1);
			break;
          
		  case '#':	
		    if(L >= Leftside ) // && i < length
			{
			  if(i < length)
			  {
		        _Disp->printChar(*text++, L, Y1);
				i++;
			  }
			  else 
			    _Disp->printChar(Filler, L, Y1);
			}
			else 
			  _Disp->printChar(Filler, L, Y1);
			
			L += (FS? 16 : 12)-1;
			break;
			
          default:
            _Disp->printChar(Type, L, Y1);
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
      extern uint8_t SmallFont[];
      extern uint8_t BigFont[];
      Save_MainColor;
      if (_padding > 0)
      {
        if (!locked)
        {
          _Disp->setColor(PadColor);
          if (Fill)
            (Round ? _Disp->fillRoundRect(X1, Y1, X2, Y2) : _Disp->fillRect(X1, Y1, X2, Y2));
          else
            (Round ? _Disp->drawRoundRect(X1, Y1, X2, Y2) : _Disp->drawRect(X1, Y1, X2, Y2));
          locked = true;
        }

        _Disp->setColor(BC);
        if (Fill)
          (Round ? _Disp->fillRoundRect(X1 + _padding, Y1 + _padding, X2 - _padding, Y2 - _padding) : _Disp->fillRect(X1 + _padding, Y1 + _padding, X2 - _padding, Y2 - _padding));
        else
          (Round ? _Disp->drawRoundRect(X1 + _padding, Y1 + _padding, X2 - _padding, Y2 - _padding) : _Disp->drawRect(X1 + _padding, Y1 + _padding, X2 - _padding, Y2 - _padding));
      }
      else
      {
        _Disp->setColor(BC);
        if (Fill)
          (Round ? _Disp->fillRoundRect(X1, Y1, X2, Y2) : _Disp->fillRect(X1, Y1, X2, Y2));
        else
          (Round ? _Disp->drawRoundRect(X1, Y1, X2, Y2) : _Disp->drawRect(X1, Y1, X2, Y2));
      }

      _Disp->setColor(Tcolor);
	  _Disp->setBackColor(TBcolor);
      _Disp->setFont(textSize ? BigFont : SmallFont);
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
	  Save_MainColor;
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
				  _Disp -> setColor(C3);
				else if((Y <= Mid) && (Y > Hi))
				  _Disp -> setColor(C2);
				else if((Y <= Hi) && (Y > Y1))
				  _Disp -> setColor(C1);
			  }
			  else
				_Disp->setColor(BC);

			  _Disp->drawHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2);
			}
		  }
		  else
		  {
			  for (int Y = Y1 + _padding; Y < Y2 - _padding; Y++)
			  {
				if (T >= Y)
				{
				  if((Y > Low) && (Y <= Mid)) 
					_Disp -> setColor(C3);
				  else if((Y > Mid) && (Y <= Hi))
					_Disp -> setColor(C2);
				  else if((Y > Hi) && (Y <= Y2))
					_Disp -> setColor(C1);
				}
				else
				  _Disp->setColor(BC);

				_Disp->drawHLine(X1 + _padding, Y, (X2 - X1) - _padding * 2);
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
				  _Disp -> setColor(C3);
				else if((X >= Mid) && (X < Hi))
				  _Disp -> setColor(C2);
				else if((X >= Hi) && (X < X2))
				  _Disp -> setColor(C1);
			  }
			  else
				_Disp->setColor(BC);

			  _Disp->drawVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2);
			}
		  }
		  else
		  {
			  for (int X = X2 - _padding; X > X1 + _padding; X--)
			  {
				if (T <= X)
				{
				  if((X < Low) && (X >= Mid)) 
					_Disp -> setColor(C3);
				  else if((X < Mid) && (X >= Hi))
					_Disp -> setColor(C2);
				  else if((X < Hi) && (X >= X1))
					_Disp -> setColor(C1);
				}
				else
				  _Disp->setColor(BC);

				_Disp->drawVLine(X, Y1 + _padding, (Y2 - Y1) - _padding * 2);
			  }
		  }
	  }
      _Disp->setColor(Tcolor);
      _Disp->setBackColor(0xFFFFFFFF);
      _Disp->setFont(textSize ? BigFont : SmallFont);
	  if(strcmp(text, " "))
        _Disp->print(text, TX, TY);
    
	  if(textEnable)
	  {
        _Disp->setColor(P_Tcolor);
        _Disp->setBackColor(P_TBcolor);
        _Disp->setFont(P_textSize ? BigFont : SmallFont);
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

        _Disp->print(buf, P_TX, P_TY);
	  }
	  Restore_MainColor;
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

		void Coords(int X, int Y, int XRad, int YRad, int XThick, int YThick)
		{
		  cx = X; 
		  cy = Y; 
		  Xradius = XRad; 
		  Xthickness = XThick;
		  Yradius = YRad; 
		  Ythickness = YThick;
		}

		void Coords(int X, int Y, int Rad, int Thick)
		{
		  Coords(X, Y, Rad, Rad, Thick, Thick);
		}
		
		void Range(int Start, int Stop, int increments)
		{
		  start = Start;
		  stop = Stop;
		  inc = increments;
		  dir = true;
		  if(start < stop)
		    dir = false;
		}

		void Colors(word Color, long BC = 0xffffffff)
		{
		  color = Color;
		  BC = BC;
		}

		void drawArc()
		{
		  Save_MainColor;
	
		  for(int rotate = start ; rotate != stop; rotate+=(dir? -1 : 1))
		  {
			_Disp->setColor(color); 
			_Disp->setBackColor(BC);
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
		
	private:
		int cx; 
		int cy; 
		int Xradius, Yradius;
		int Xthickness, Ythickness;
		int start;
		int stop;
		word color;
		bool lines;
		int inc, dir;
		long BC;
	
	protected:
	    bool orient;
		TFT_Display *_Disp;
		Base *_base;
};

class Gauge
{
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
	
	void TextColor(word C = WHITE)
	{
	  TC = C;
	}

	void TextRange(int Start, int Stop, int inc, bool where = 0, byte textSize=1)
	{
	  Tstart = Start;
	  Tstop = Stop;		
	  Tinc = inc;
		
	  Where = where;
	  numLen = max(numSize(Start), numSize(Stop));
	
	#if defined TFT_ScaleFonts_h
	  TS = textSize;
	#else 
	  TS = 1;
	#endif
	
	  TextOffset(0, 0);
	}
	
	void TextOffset(int X = 0, int Y = 0)
	{
	  TXoff = X;
	  TYoff = Y; 
	  	  
	  Xoff = (cx - ((_Disp->getFontXsize()*TS)*numLen)/2) + TXoff;
	  Yoff = (cy - ((_Disp->getFontYsize()/2)*TS)) + TYoff;
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
	  _Disp->setColor(TC);
	  _Disp->setBackColor((_Disp->_transparent? 0xffffffff : _Disp->getBackColor()));
	  #if defined TFT_ScaleFonts_h
	  Sfonts->print(Htext, cx - (strlen(Htext)*(_Disp->getFontXsize()/2)*HTsize) + TXoff, Yoff - (_Disp->getFontYsize()*HTsize) + TYoff, HTsize);
	  Sfonts->print(Ftext, cx - (strlen(Ftext)*(_Disp->getFontXsize()/2)*FTsize) + TXoff, cy + ((_Disp->getFontYsize()/2)*TS)+ TYoff, FTsize);
	  #else
	  _Disp->print(Htext, cx - (strlen(Htext)*_Disp->getFontXsize()/2) + TXoff, Yoff - _Disp->getFontYsize() + TYoff);
	  _Disp->print(Ftext, cx - (strlen(Ftext)*_Disp->getFontXsize()/2) + TXoff, cy + (_Disp->getFontYsize()/2) + TYoff);
	  #endif
	  
	  if(TE)
	    DrawText();

	  EmptyGauge();
	  lastPos = start + (dir? 1: -1);
	  Restore_MainColor;
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
      
	  _Disp->setColor(color);
	  if(rotate < start)
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
	    _Disp->setColor(TC);
	    _Disp->setBackColor((_Disp->_transparent? 0xffffffff : _Disp->getBackColor()));
	    #if defined(TFT_ScaleFonts_h)
	    Sfonts->printNumI(output, Xoff, Yoff , numLen, TS, '0');
	    #else
	    _Disp->printNumI(output, cx - 8*numLen + TXoff, cy - (_Disp->getFontXsize()/2) + TYoff, numLen,'0');
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
	      int rotate = Map(r, lP, hP, start, stop);
          _Disp->setColor((rotate >= GPosition? cfg:cbg));
		  float _cos1 = -cos(rotate * Deg_to_rad);
		  float _sin1 = sin(rotate * Deg_to_rad);

		  int Xo = cx + (_cos1 * radius);
		  int Xi = cx + (_cos1 * (radius - thick));

		  int Yo = cy + (_sin1 * radius);
		  int Yi = cy + (_sin1 * (radius - thick));
		//===============================================
		  if(rotate < start)
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
	  _Disp->setColor(TC);
	  for (int rotate = Tstart; rotate <= Tstop; rotate += (Tinc == 0 ? 1 : Tinc))
	  {
        int output = Map(rotate, Tstart, Tstop, start, stop);
		float _cos = -cos(output * Deg_to_rad);
		float _sin = sin(output * Deg_to_rad);

		if(Where)
		{
		  X = cx-(8*numLen) + (_cos * (radius + 16*(numLen-1)));
		  Y = cy + (_sin * (radius + 20 - 16*(((output < 0) || (output > 180))? sin(output * Deg_to_rad) : 0)));
		}
		else
		{
		  X = cx-(8*(numLen+1)) + (_cos * (radius - thick - (8*(numLen))));
		  Y = cy + (_sin * (radius - thick - 5));
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
	private:
		int cx; 
		int cy; 
		int radius;
		int thick;
		int start;
		int stop;
		bool dir;
		word cfg, cbg, TC;
		int Tstart;
		int Tstop;
		int Tinc;
		bool Where;
        int numSize(unsigned long x) {byte count = 0; while(x != 0){ x*=0.1; count++;} return count;}
		int numLen;
		int Xoff, Yoff, TS, TXoff, TYoff;
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