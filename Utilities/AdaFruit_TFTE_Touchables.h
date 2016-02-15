#ifndef AdaFruit_TFTE_Touchables_h
#define AdaFruit_TFTE_Touchables_h

class Box : public Base
{
	struct Properties
	{
	  int x;
	  int y;
	  int width;
	  int height;
	  int x2;
	  int y2;
	  word C1, C2, PadColor, tColor, HFcolor;
	  bool Round, Fill;
	  byte PaddingSize, FontSize, HFFontSize;
	}Props;
	
  protected:
    byte State, lastState;
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    Box(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
      locked = 0;
      SetState(false);
      Padding(2, WHITE);
      lastState = 255;
      Text(" ", BLACK, Small);
      HeadFootText(0, 0, BLACK, Small);
	  lastTouch = 0, gotfirst = false, released = false;
	  lockout = false;
	  setBackColor(0x0);
    }
	
	~Box(){ }
	
	void setBackColor(uint32_t color)
	{
	  BGCOLOR = color;
	}
	
	uint32_t getBackColor()
	{
	  return BGCOLOR;
	}

    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
	  Props.width = x2-x1;
	  Props.height = y2-y1;
    }

    void Colors(uint16_t c1, uint16_t c2, bool round, bool fill)
    {
      Props.C1 = c1;
      Props.C2 = c2;
      Props.Round = round;
      Props.Fill = fill;
    }

    void Colors(uint16_t color, bool round, bool fill)
    {
      Colors(color, color, round, fill);
    }

    void Padding(byte padding = 0, uint16_t padColor = WHITE)
    {
      Props.PaddingSize = padding;
      Props.PadColor = padColor;
    }

    void Text(char * PText, char * NPText, uint16_t Tcolor, byte fontSize)
    {
      pText = PText;
      npText = NPText;
      Props.FontSize = fontSize;
      Props.tColor = Tcolor;
    }

    void Text(char * PText, uint16_t Tcolor, byte fontSize)
    {
      Text(PText, PText, Tcolor, fontSize);
    }

    void HeadFootText(char * HText, char * FText, uint16_t color, byte fontSize)
    {
      hText = HText;
      fText = FText;
      Props.HFFontSize = fontSize;
      Props.HFcolor = color;
    }

    void HeaderText(char * HText, uint16_t color, byte fontSize)
    {
      HeadFootText(HText, " ", color, fontSize);
    }

    void FooterText(char * FText, uint16_t color, byte fontSize)
    {
      HeadFootText(" ", FText, color, fontSize);
    }

    void Draw(bool TE = true)
    {
	  //Save_MainColor;
	  
	  if(Props.x2 == AUTO)
	    Props.x2 = Props.x + max(strlen(pText),strlen(npText))*(6*(Props.FontSize+1)) + 10;//autoPadding;
	  if(Props.y2 == AUTO)
	    Props.y2 = Props.y + (8*(Props.FontSize+1)) + 8;//autoPadding;
		
      if (State != lastState)
      {
        if (Props.PaddingSize > 0) //Padding for the button
        {
          if (!locked)
          {
            if (Props.Fill)
              (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.PadColor) : _Disp->fillRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.PadColor));
            else
              (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.PadColor) : _Disp->drawRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.PadColor));

            locked = true;
          }

          if (Props.Fill)
            (Props.Round ? _Disp->fillRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize*2, Props.y2-Props.y - Props.PaddingSize*2,2,(State ? Props.C1 : Props.C2)) : _Disp->fillRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize*2, Props.y2-Props.y - Props.PaddingSize*2,(State ? Props.C1 : Props.C2)) );
          else
            (Props.Round ? _Disp->drawRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize*2, Props.y2-Props.y - Props.PaddingSize*2,2,(State ? Props.C1 : Props.C2))  : _Disp->drawRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize*2, Props.y2-Props.y - Props.PaddingSize*2,(State ? Props.C1 : Props.C2)) );
        }
        else
        {
          if (Props.Fill)
            (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2,(State ? Props.C1 : Props.C2)) : _Disp->fillRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,(State ? Props.C1 : Props.C2)));
          else
            (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2,(State ? Props.C1 : Props.C2)) : _Disp->drawRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,(State ? Props.C1 : Props.C2)));
        }
		if(Props.Fill)
		  setBackColor(State ? Props.C1 : Props.C2);
        drawText(TE, State);

        lastState = State;
      }
      //Restore_MainColor;
    }

    void ReDraw(bool TE = true)
    {
	  locked = false;
      lastState = 255;
	  this->Draw(TE);
    }

    void drawText(bool TxtEnable, bool Button)
    {
      if (TxtEnable) // If there is text for that button, show it.
      {
        byte strl = strlen(pText);
        byte strl_2 = strlen(npText);
        int Xpos = (Props.x2 + Props.x) / 2; // find the center of the button
        int Ypos = (Props.y2 + Props.y) / 2; // -----------------------------
		  
        if (Props.C1 == Props.C2)
		  _Disp->setTextColor(((Props.tColor != Props.C1) ? Props.tColor : ~Props.tColor), getBackColor());
          // Show the text color
        else
        {
          _Disp->setTextColor(((Props.tColor != (Button?Props.C1:Props.C2)) ? Props.tColor : ~Props.tColor), getBackColor());
        }
		
		_Disp->setTextSize(Props.FontSize + 1);
		if (Button)
		{
		  _Disp->setCursor(Xpos - (strl * (3*(Props.FontSize+1))), Ypos - (4*(Props.FontSize+1))+1);
		  _Disp->print(pText); // small font is 8x12
		}
		else
		{
		  _Disp->setCursor(Xpos - (strl_2 * (3*(Props.FontSize+1))), Ypos - (4*(Props.FontSize+1))+1);
		  _Disp->print(npText);
		}
		
        _Disp->setTextColor(Props.HFcolor, getBackColor());
        _Disp->setTextSize(Props.HFFontSize ? 2 : 1);
        if (strcmp(hText, " "))
		{
		  _Disp->setCursor(((Props.x + Props.x2) / 2) - strlen(hText) * (Props.HFFontSize ? 5 : 2.5f), Props.y - 20);
          _Disp->print(hText);
		}
        if (strcmp(fText, " "))
		{
		  _Disp->setCursor(((Props.x + Props.x2) / 2) - strlen(fText) * (Props.HFFontSize ? 5 : 2.5f), Props.y2 + 5);
          _Disp->print(fText);
		}
      }
    }

    bool getTouchState()
    {
	  if(_Touch->dataAvailable())
	  {
		_Touch->read();
	    int x, y;
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}
		return (IsWithin(x, Props.x, Props.x2) & IsWithin(y, Props.y, Props.y2));
	  }
	  else
	    return false;
    }

    bool Touch(bool draw = true)
    {
      State = this->getTouchState(); // If the buttons coords are touched, return true.
      if (draw)
        this->Draw();
      return State; // button coords were not touched, return false.
    }

    bool Toggle()
    {
      if ( this->getTouchState() )
      {
        if (Lock == true) // If this button is pressed, set the latch.
        {
          State = !State;
          Lock = false; // Only allow it to be pressed once and not held.
          this->Draw();
        }
      }
      else Lock = true; // Once released allow it to be pressed again.

      return State; // Return the buttons state
    }

    bool Delay(unsigned long T = 1000)
    {   
	  bool touch = this->getTouchState();
	  
      if (touch==true && lockout == false)
      {
        B_current_time = millis();
        lockout = true;
      }
      
      if (touch && lockout)
        State = ((millis() - B_current_time) >= T);
	  else 
	  {
	    State = false; 
	    lockout = false;
	  }
      
      this->Draw();

      return State;
    }
	
	bool DoubleClick(unsigned long timeout = 500000UL)
	{
	  static unsigned long time = micros();
	  
	  bool touch = this->getTouchState();
	  
      if (touch != lastTouch) // get first touch 
      {
	    if(touch)
		{
		  gotfirst = true;
		  time = micros();// record the time
		}
		lastTouch = touch;
      }
	  
	  if(gotfirst && !touch) // see if it was released
	    released = true;
		
	  if(gotfirst && ((micros() - time) >= timeout)) // check the timeout timer, if it sees that it was touched then\
	  then not touched again, reset released and gotfirst .
	  {
	    released = false;
		gotfirst = false;
	  }
	  
	  if(!touch && !gotfirst) // If not touched and it sees that it was not touched at all, set released to false.
	    released = false;
	  
	  if(touch & released) // if it is being touched and sees that it was previously released, then the user double clicked
	  {
	    gotfirst = false;
	    State = true;
	  }
	  else 
	    State = false; // show it is not being touched
		
	  this->Draw();
	  return State;
	}

    void SetState(bool S)
    {
      State = S;
    }
	
	byte getState()
	{
	  return this->State;
	}

    void Unlock()
    {
      locked = false;
    }
    
	uint16_t getButtonPressedColor()
	{
	  return Props.C1;
	}
	
	uint16_t getButtonReleasedColor()
	{
	  return Props.C2;
	}
	
	uint16_t getButtonTextColor()
	{
	  return Props.tColor;
	}
	
	uint16_t getButtonPaddingColor()
	{
	  return Props.PadColor;
	}
	
	uint16_t getButtonHeadFootColor()
	{
	  return Props.HFcolor;
	}
	
	void getText(char* text)
	{
	  strcpy(text, (getState()? pText : npText));
	}
	
	Properties getButtonProperties()
	{
	  return Props;
	}
	
  private:
    bool Lock;
    bool locked;
	byte lastTouch, gotfirst, released;
	//byte autoPadding;
    char *pText, *npText, *hText, *fText;
	unsigned long B_current_time;
    bool lockout;
	uint32_t BGCOLOR;
};

class Circle : public Base
{
  protected:
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    struct Properties
	{
	  int x;
	  int y;
	  int radius;
	  word C1, C2, PadColor, tColor, HFcolor;
      bool Fill;
	  byte PaddingSize, FontSize, HFFontSize, autoPadding;
	}Props;
	
    Circle(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
      SetState(false);
      lastState = 1;
      locked = 0;
      Text(" ", BLACK, Small);
      HeadFootText(" ", " ", BLACK, Small);
	  lastTouch = 0, gotfirst = false, released = false;
	  Props.autoPadding = 5;
	  lockout = false;
	  touch = 0;
	  setBackColor(0x0);
    }
	
    ~Circle(){ }
	
	void setBackColor(uint32_t color)
	{
	  BGCOLOR = color;
	}
	
	uint32_t getBackColor()
	{
	  return BGCOLOR;
	}
	
    void Coords(int x, int y, int radius)
    {
      Props.x = x;
      Props.y = y;
      Props.radius = radius;
    }

    void Colors(uint16_t c1, uint16_t c2, bool fill)
    {
      Props.C1 = c1;
      Props.C2 = c2;
      Props.Fill = fill;
    }

    void Colors(uint16_t color, bool fill)
    {
      Colors(color, color, fill);
    }

    void Padding(byte padding = 0, uint16_t padColor = WHITE)
    {
      Props.PaddingSize = padding;
      Props.PadColor = padColor;
    }

    void Text(char * PText, char * NPText, uint16_t Tcolor, byte fontSize)
    {
      pText  = PText;
      npText = NPText;
      Props.FontSize = fontSize;
      Props.tColor = Tcolor;
    }

    void Text(char * PText, uint16_t Tcolor, byte fontSize)
    {
      Text(PText, PText, Tcolor, fontSize);
    }

    void HeadFootText(char * HText, char * FText, uint16_t color, byte fontSize)
    {
      hText  = HText;
      fText = FText;
      Props.HFFontSize = fontSize;
      Props.HFcolor = color;
    }

    void HeaderText(char * HText, uint16_t color, byte fontSize)
    {
      HeadFootText(HText, " ", color, fontSize);
    }

    void FooterText(char * FText, uint16_t color, byte fontSize)
    {
      HeadFootText(" ", FText, color, fontSize);
    }

    void Draw(bool TE = true)
    {
	  //Save_MainColor;
      if (State != lastState)
      {
	    if(Props.radius == AUTO)
	      Props.radius = max(strlen(pText),strlen(npText))*(3*(Props.FontSize+1)) + Props.autoPadding;
	  
        if (Props.PaddingSize > 0)
        {
          if (!locked)
          {
            Props.Fill ? _Disp->fillCircle(Props.x, Props.y, Props.radius, Props.PadColor) : _Disp->drawCircle(Props.x, Props.y, Props.radius, Props.PadColor);

            locked = true;
          }

          Props.Fill ? _Disp->fillCircle(Props.x, Props.y, Props.radius - Props.PaddingSize, (State ? Props.C1 : Props.C2)) : _Disp->drawCircle(Props.x, Props.y, Props.radius - Props.PaddingSize, (State ? Props.C1 : Props.C2));
        }
        else
        {
          Props.Fill ? _Disp->fillCircle(Props.x, Props.y, Props.radius, (State ? Props.C1 : Props.C2)) : _Disp->drawCircle(Props.x, Props.y, Props.radius, (State ? Props.C1 : Props.C2));
        }

        lastState = State;
        
		if(Props.Fill)
		  setBackColor(State ? Props.C1 : Props.C2);
		  
        drawText(TE, State);
      }
	  //Restore_MainColor;
    }

    void ReDraw(bool TE = true)
    {
	  locked = false;
      lastState = 255;
	  this->Draw(TE);
    }

    void drawText(bool TxtEnable, bool Button)
    {
      if (TxtEnable) // If there is text for that button, show it.
      {
        byte strl = strlen(pText);
        byte strl_2 = strlen(npText);
		  
        if (Props.C1 == Props.C2)
		  _Disp->setTextColor(((Props.tColor != Props.C1) ? Props.tColor : ~Props.tColor), getBackColor());
          // Show the text color
        else
        {
          if (Button)
            _Disp->setTextColor(((Props.tColor != Props.C1) ? Props.tColor : ~Props.tColor), getBackColor());
          else
            _Disp->setTextColor(((Props.tColor != Props.C2) ? Props.tColor : ~Props.tColor), getBackColor());
        }
		if (Props.FontSize) //big font
		{
		  _Disp->setTextSize(2);
		  if (Button)
		  {
		    _Disp->setCursor(Props.x - (strl * 6), Props.y - 8);
			_Disp->print(pText); //print the string in the center of the button. Big font is 16x16
		  }
		  else
		  {
		    _Disp->setCursor(Props.x - (strl_2 * 6), Props.y - 8);
			_Disp->print(npText);
		  }
		}
		else
		{
		  _Disp->setTextSize(1);
		  if (Button)
		  {
		    _Disp->setCursor(Props.x - (strl * 3), Props.y - 3);
			_Disp->print(pText); 
		  }
		  else
		  {
		    _Disp->setCursor(Props.x - (strl_2 * 3), Props.y - 3);
			_Disp->print(npText);
		  }
		}
		
        _Disp->setTextColor(Props.HFcolor, getBackColor());
        _Disp->setTextSize(Props.HFFontSize ? 2 : 1);
        if (strcmp(hText, " "))
		{
		  _Disp->setCursor(Props.x - strlen(hText) * (Props.HFFontSize ? 5 : 2.5f), Props.y - 20);
          _Disp->print(hText);
		}
        if (strcmp(fText, " "))
		{
		  _Disp->setCursor(Props.x - strlen(fText) * (Props.HFFontSize ? 5 : 2.5f), Props.y+Props.radius + 5);
          _Disp->print(fText);
		}
      }
    }

    bool getTouchState()
    {
	  if(_Touch->dataAvailable())
	  {
		_Touch->read();
	    int x, y;
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}
		return (IsWithin(((x - Props.x)*(x - Props.x)) + ((y - Props.y)*(y - Props.y)), 0, (Props.radius * Props.radius)));
	  }
	  else 
	    return false;
    }

    bool Touch(bool draw = true)
    {
      State = this->getTouchState(); // If the buttons coords are touched, return true.
      if (draw)
        this->Draw();
      return State; // button coords were not touched, return false.
    }

    bool Toggle()
    {
      if ( this->getTouchState() )
      {
        if (Lock == true) // If this button is pressed, set the latch.
        {
          State = !State;
          Lock = false; // Only allow it to be pressed once and not held.
          this->Draw();
        }
      }
      else Lock = true; // Once released allow it to be pressed again.

      return State; // Return the buttons state
    }

    bool Delay(unsigned long T = 1000)
    { 
      touch = this->getTouchState();
	  
      if (touch==true && lockout == false)
      {
        B_current_time = millis();
        lockout = true;
      }

      if (touch && lockout)
        State = ((millis() - B_current_time) >= T);
	  else 
	  {
	    State = false; 
	    lockout = false;
	  }

      this->Draw();

      return State;
    }
	
	bool DoubleClick(unsigned long timeout = 500000UL)
	{
	  static unsigned long time = micros();
	  
	  bool touch = this->getTouchState();
	  
      if (touch != lastTouch) // get first touch 
      {
	    if(touch)
		{
		  gotfirst = true;
		  time = micros(); // record the time
		}
		lastTouch = touch;
      }
	  
	  if(gotfirst && !touch) // see if it was released
	    released = true;
		
	  if(gotfirst && ((micros() - time) >= timeout)) // check the timeout timer, if it sees that it was touched then\
	  then not touched again, reset released and gotfirst .
	  {
	    released = false;
		gotfirst = false;
	  }
	  
	  if(!touch && !gotfirst) // If not touched and it sees that it was not touched at all, set released to false.
	    released = false;
	  
	  if(touch & released) // if it is being touched and sees that it was previously released, then the user double clicked
	  {
	    gotfirst = false;
	    State = true;
	  }
	  else 
	    State = false; // show it is not being touched
		
	  this->Draw();
	  return State;
	}

    void SetState(bool S)
    {
      State = S;
    }
	
	byte getState()
	{
	  return this->State;
	}

    void Unlock()
    {
      locked = false;
    }

	uint16_t getButtonPressedColor()
	{
	  return Props.C1;
	}
	
	uint16_t getButtonReleasedColor()
	{
	  return Props.C2;
	}
	
	uint16_t getButtonTextColor()
	{
	  return Props.tColor;
	}
	
	uint16_t getButtonPaddingColor()
	{
	  return Props.PadColor;
	}
	
	uint16_t getButtonHeadFootColor()
	{
	  return Props.HFcolor;
	}
	
	void getText(char* text)
	{
	  strcpy(text, (getState()? pText : npText));
	}
	
	Properties getButtonProperties()
	{
	  return Props;
	}
	
  private:
	uint32_t BGCOLOR;
    bool Lock, State, lastState;
    bool locked;
    char *pText, *npText, *hText, *fText;
	byte lastTouch, gotfirst, released;
	unsigned long B_current_time;
    bool lockout, touch;
};

class Triangle : public Base
{
  protected:
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    struct Points
    {
      int x;
      int y;
    };

    struct T_ABC
    {
      Points A, B, C;
    } TriPoints;
    //----------------------------------------

    Triangle(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
      lastState = 0;
      SetState(true);
      Lock = 0;
      locked = false;
      //Text(" ", BLACK, Small);
	  //HeadFootText(" ", " ", BLACK, Small);
	  lastTouch = 0, gotfirst = false, released = false;
	  touch = 0;
	  lockout = false;
    }
    
	~Triangle(){ }
	
    void Coords(int x1, int y1, int x2, int y2, int x3, int y3)
    {
      TriPoints.A.x = x1;
      TriPoints.A.y = y1;
      TriPoints.B.x = x2;
      TriPoints.B.y = y2;
      TriPoints.C.x = x3;
      TriPoints.C.y = y3;
    }

    void Coords(int x, int y, int base, int deg = 0)
    {
      convert_xyDegtoABC(x, y, base, deg);
    }

    void getCoords(int *Cx, int *Cy, int *Cx1, int *Cy1, int *Cx2, int *Cy2)
    {
      *Cx = TriPoints.A.x;
      *Cy = TriPoints.A.y;
      *Cx1 = TriPoints.B.x;
      *Cy1 = TriPoints.B.y;
      *Cx2 = TriPoints.C.x;
      *Cy2 = TriPoints.C.y;
    }

    void Colors(uint16_t c1, uint16_t c2, bool fill)
    {
      C1 = c1;
      C2 = c2;
      Fill = fill;
    }

    void Colors(uint16_t color, bool fill)
    {
      Colors(color, color, fill);
    }

    /*
    void Padding(byte padding = 0, uint16_t padColor = WHITE)
    {
      _padding = padding;
      PadColor = padColor;
    }
    */

	//Text is difficult to place in a triangle, so I disabled it.
    /* void Text(char * PText, char * NPText, uint16_t Tcolor, byte fontSize)
    {
      pText  = PText;
      npText = NPText;
      FontSize = fontSize;
      tColor = Tcolor;
    }

    void Text(char * PText, uint16_t Tcolor, byte fontSize)
    {
      Text(PText, PText, Tcolor, fontSize);
    } */

    void HeadFootText(char * HText, char * FText, uint16_t color, byte fontSize)
    {
      hText  = HText;
      fText = FText;
      HFFontSize = fontSize;
      HFcolor = color;
    }

    void HeaderText(char * HText, uint16_t color, byte fontSize)
    {
      HeadFootText(HText, " ", color, fontSize);
    }

    void FooterText(char * FText, uint16_t color, byte fontSize)
    {
      HeadFootText(" ", FText, color, fontSize);
    }

    void Draw()
    {
	  //Save_MainColor;
      int Cx, Cx1, Cx2, Cy, Cy1, Cy2;
	  //delay(1);
      if (State != lastState)
      {
        this->getCoords(&Cx, &Cy, &Cx1, &Cy1, &Cx2, &Cy2);
        if (!locked)
        {
          if (Fill)
            fillTriangle(Cx, Cy, Cx1, Cy1, Cx2, Cy2, (State ? C1 : C2));
		  else  
		    drawTriangle(Cx, Cy, Cx1, Cy1, Cx2, Cy2, (State ? C1 : C2));
         
          locked = true;
        }

        if (Fill)
          fillTriangle(Cx, Cy, Cx1, Cy1, Cx2, Cy2, (State ? C1 : C2));
		else
		  drawTriangle(Cx, Cy, Cx1, Cy1, Cx2, Cy2, (State ? C1 : C2));
        
        lastState = State;
      }

	  _Disp->setTextColor(HFcolor);
      _Disp->setTextSize(HFFontSize ? 2 : 1);
	  
	  if(strcmp(hText, " "))
	  {
	    _Disp->setCursor(((Cx + Cx1) / 2) - strlen(hText) * (HFFontSize ? 8 : 4), ((Cy + Cy1) / 2) - 20);
        _Disp->print(hText);
	  }
	  if(strcmp(fText, " "))
	  {
	    _Disp->setCursor(((Cx + Cx1) / 2) - strlen(fText) * (HFFontSize ? 8 : 4), ((Cy + Cy1) / 2) + 5);
        _Disp->print(fText);
	  }
		
	  //Restore_MainColor;
    }

    void ReDraw()
    {
	  locked = false;
      lastState = 255;
	  this->Draw();
    }

    void drawTriangle(int Cx, int Cy, int Cx1, int Cy1, int Cx2, int Cy2, int color)
    {
      _Disp->drawLine(Cx, Cy, Cx1, Cy1, color);
      _Disp->drawLine(Cx1, Cy1, Cx2, Cy2, color);
      _Disp->drawLine(Cx2, Cy2, Cx, Cy, color);
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

	  A = (y2 - y1)? (float(x2 - x1) / float(y2 - y1)) : (x2 - x1);
	  B = (y3 - y1)? (float(x3 - x1) / float(y3 - y1)) : (x3 - x1);
	  C = (y3 - y2)? (float(x3 - x2) / float(y3 - y2)) : (x3 - x2);

      for (int Dy = y1; Dy <= y3; Dy++)
      {
        if (Dy <= y2)
          XL = x1 + A * (Dy - y1);
        else
          XL = x2 + C * (Dy - y2);

        XR = x1 + B * (Dy - y1);

        _Disp->drawFastHLine(XL, Dy, (XR - XL), color);
      }
    }

    bool getTouchState()
    {
	  if(_Touch->dataAvailable())
	  {
		int Cx, Cx1, Cx2, Cy, Cy1, Cy2;
        int x, y;
		
		_Touch->read();
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}

		this->getCoords(&Cx, &Cy, &Cx1, &Cy1, &Cx2, &Cy2);
		return PointInTriangle(x, y, Cx, Cy, Cx1, Cy1, Cx2, Cy2); // This is the ideal area of the triangle based on the 3 points.;
	  }
	  else 
	    return false;
    }

    bool Touch(bool draw = true)
    {
      State = this->getTouchState(); // If the buttons coords are touched, return true.
      if (draw)
        this->Draw();
      return State; // button coords were not touched, return false.
    }

    bool Toggle()
    {
      if (this->getTouchState() )
      {
        if (Lock == true) // If this button is pressed, set the latch.
        {
          State = !State;
          this->Draw();
          Lock = false; // Only allow it to be pressed once and not held.
        }
      }
      else Lock = true; // Once released allow it to be pressed again.

      return State; // Return the buttons state
    }

    bool Delay(unsigned long T = 1000)
    {   
      touch = this->getTouchState();
	  
      if (touch==true && lockout == false)
      {
        B_current_time = millis();
        lockout = true;
      }

      if (touch && lockout)
        State = ((millis() - B_current_time) >= T);
	  else 
	  {
	    State = false; 
	    lockout = false;
	  }

      this->Draw();

      return State;
    }

	bool DoubleClick(unsigned long timeout = 500000UL)
	{
	  static unsigned long time = micros();
	  
	  bool touch = this->getTouchState();
	  
      if (touch != lastTouch) // get first touch 
      {
	    if(touch)
		{
		  gotfirst = true;
		  time = micros();// record the time
		}
		lastTouch = touch;
      }
	  
	  if(gotfirst && !touch) // see if it was released
	    released = true;
		
	  if(gotfirst && ((micros() - time) >= timeout)) // check the timeout timer, if it sees that it was touched then\
	  then not touched again, reset released and gotfirst .
	  {
	    released = false;
		gotfirst = false;
	  }
	  
	  if(!touch && !gotfirst) // If not touched and it sees that it was not touched at all, set released to false.
	    released = false;
	  
	  if(touch & released) // if it is being touched and sees that it was previously released, then the user double clicked
	  {
	    gotfirst = false;
	    State = true;
	  }
	  else 
	    State = false; // show it is not being touched
		
	  this->Draw();
	  return State;
	}
	
    void SetState(bool S)
    {
      State = S;
      //this->Draw();
    }
    
	byte getState()
	{
	  return this->State;
	}
	
    void Polygon(int cx, int cy, int sides, int diameter, uint16_t color, bool fill = 0, float deg = 0)
    {
	  //Save_MainColor;
	  #define deg_to_rad	0.01745 + 3.1415
      diameter /= 2;

      if(sides < 3) sides = 3; // This ensures the minimum side number is 3.
      int Xpoints[sides], Ypoints[sides];// Set the arrays based on the number of sides entered
      int rads = 360 / sides; // This equally spaces the points.

      for (int idx = 0; idx < sides; idx++)
      {
        Xpoints[idx] = cx + sin(float(idx * rads + deg) * deg_to_rad) * diameter;
        Ypoints[idx] = cy + cos(float(idx * rads + deg) * deg_to_rad) * diameter;
      }

      for (int idx = 0; idx < sides; idx++) // draws the polygon on the screen.
      {
        if ( (idx + 1) < sides)
          _Disp->drawLine(Xpoints[idx], Ypoints[idx], Xpoints[idx + 1], Ypoints[idx + 1], color); // draw the lines
        else
          _Disp->drawLine(Xpoints[idx], Ypoints[idx], Xpoints[0], Ypoints[0], color); // finishes the last line to close up the polygon.
      }

      if (fill)
        for (int idx = 0; idx < sides; idx++)
        {
          if ((idx + 1) < sides)
            fillTriangle(cx, cy, Xpoints[idx], Ypoints[idx], Xpoints[idx + 1], Ypoints[idx + 1], color);
          else
            fillTriangle(cx, cy, Xpoints[idx], Ypoints[idx], Xpoints[0], Ypoints[0], color);
        }
	  //Restore_MainColor;
    }
	
	uint16_t getButtonPressedColor()
	{
	  return C1;
	}
	
	uint16_t getButtonReleasedColor()
	{
	  return C2;
	}
	
	uint16_t getButtonTextColor()
	{
	  return tColor;
	}
	
	uint16_t getButtonPaddingColor()
	{
	  return PadColor;
	}
	
	uint16_t getButtonHeadFootColor()
	{
	  return HFcolor;
	}

  private:
    int X, Y, _Base, Deg;
    int X2, Y2;
    int X3, Y3;
    uint16_t C1, C2, PadColor, tColor, HFcolor;
    bool Fill;
    bool Lock, State, lastState, locked;
    bool OPT;
    char *pText, *npText, *hText, *fText;
    byte _padding, FontSize, HFFontSize;
	byte lastTouch, gotfirst, released;
	unsigned long B_current_time;
    bool lockout, touch;

    long Area(long Ax, long Ay, long Bx, long By, long Cx, long Cy)
    {
      return abs(((Ax * (By - Cy)) + (Bx * (Cy - Ay)) + (Cx * (Ay - By))) / 2);
    }

    bool PointInTriangle(long tx, long ty, long Cx, long Cy, long Cx1, long Cy1, long Cx2, long Cy2)
    {
      long A = Area(Cx, Cy, Cx1, Cy1, Cx2, Cy2); // This is the ideal area of the triangle based on the 3 points.
      long A1 = Area(tx, ty, Cx1, Cy1, Cx2, Cy2);// this checks to see if the touch coords are within the created triangle.
      long A2 = Area(Cx, Cy, tx, ty, Cx2, Cy2);  // The reason three are needed is because each one is a section of the triangle.
      long A3 = Area(Cx, Cy, Cx1, Cy1, tx, ty);

      return (A >= (A1 + A2 + A3));
    }

    void convert_xyDegtoABC(int x, int y, int base, int Deg)
    {
      Points C, C1, C2;

      int dir;

      if (Deg < 90) dir = Tri_up;
      else if (Deg < 180) dir = Tri_left; 
      else if (Deg < 270) dir = Tri_down;
      else dir = Tri_right;

      C.x = x + sin(float(Deg) * Deg_to_rad) * base;
      C.y = y + cos(float(Deg) * Deg_to_rad) * base;
      C1.x = x + sin((float(Deg) + 119) * Deg_to_rad) * base;
      C1.y = y + cos((float(Deg) + 119) * Deg_to_rad) * base;
      C2.x = x + sin((float(Deg) + 241) * Deg_to_rad) * base;
      C2.y = y + cos((float(Deg) + 241) * Deg_to_rad) * base;

      TriPoints.A = C;
      TriPoints.B = C1;
      TriPoints.C = C2;
    }
};

class Radio
{	
  public:
    Radio() {}
    ~Radio() {}
	
    template<class T, size_t N>
    byte RadioButtons(T(&_buttons)[N])
    {
      static byte Output = 0;
      static bool lastState = 1, lastOutput = -1;
      bool state = 0;

      for (byte i = 0; i < N; i++)
      {
        state = _buttons[i]->Touch(false);
        if (state != lastState)
        {		
          if (state)
            Output = i + 1;
			  
          lastState = state;
        }
		
        _buttons[i]->SetState(i == (Output - 1));
        _buttons[i]->Draw();
      }

      return Output;
    }

    template<class T, size_t N>
    void UnlockRadioButtons(T(&_buttons)[N])
    {
      for (byte i = 0; i < N; i++)
        _buttons[i]->unlock();
    }
};

class Slider : public Base
{
  protected:
    bool orient;
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    struct Properties
	{
	  int x;
	  int y;
	  int width;
	  int height;
	  int x2;
	  int y2;
	  byte PaddingSize;//1
      word C1, C2, PadColor;//12
      bool Round, Fill, Direction;//3
	}Props;
	
    Slider(Base * B, bool _o) : _base(B), orient(_o)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();

      locked = 0;
      SetRange();
      SetValue(-1);
      SetDirection(LTR);
      Padding(2, WHITE);
      T = -1;
    }
	
	~Slider(){ }

    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
	  
	  Props.width = x2-x1;
	  Props.height = y2-y1;
    }

    void Colors(uint16_t c1, uint16_t c2, bool round, bool fill)
    {
      Props.C1 = c1;
      Props.C2 = c2;
      Props.Round = round;
      Props.Fill = fill;
    }

    void SetDirection(bool Dir)
    {
      Props.Direction = Dir;
    }

    int getTouchState()
    {
      	_Touch->read();
	    int x, y;
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}

      if ((x >= Props.x) && (x <= Props.x2) && (y >= Props.y) && (y <= Props.y2))
        return orient ? y : x;
      else
        return -1;
    }

    void Draw()
    {
	  //Save_MainColor;
      if (Props.PaddingSize > 0)
      {
        if (!locked)
        {
          if (Props.Fill)
            (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.PadColor) : _Disp->fillRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.PadColor));
          else
            (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.PadColor) : _Disp->drawRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.PadColor));
          locked = true;
        }

        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize, Props.y2-Props.y - Props.PaddingSize,2, Props.C2) : _Disp->fillRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize, Props.y2-Props.y - Props.PaddingSize, Props.C2));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize, Props.y2-Props.y - Props.PaddingSize, 2,Props.C2) : _Disp->drawRect(Props.x + Props.PaddingSize, Props.y + Props.PaddingSize, Props.x2-Props.x - Props.PaddingSize, Props.y2-Props.y - Props.PaddingSize, Props.C2));
      }
      else
      {
        if (Props.Fill)
          (Props.Round ? _Disp->fillRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.C2) : _Disp->fillRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.C2));
        else
          (Props.Round ? _Disp->drawRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y,2, Props.C2) : _Disp->drawRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, Props.C2));
      }
	  //Restore_MainColor;
    }

    int Touch()
    {
	  //Save_MainColor;
      int Out = -1;
      int tmpTS = -1, tmpGV = -1;

      if ((tmpTS = getTouchState()) != -1)
        T = tmpTS;

      if ((tmpGV = GetValue()) != -1)
        T = tmpGV;

      if (T != -1)
      {
        if (orient == HORIZONTAL)
        {
          if (Props.Direction == LTR)
          {
            for (int X = Props.x + Props.PaddingSize; X <= Props.x2 - Props.PaddingSize; X++)
            {
              if ((X % (((Props.x2 - Props.x) - Props.PaddingSize) / (abs(H - L) / Inc)) != 0) | BarsOff) // (X <= T) &&
              {
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, (X <= T) ? Props.C1 : Props.C2);
              }
            }
          }
          else
          {
            for (int X = Props.x2 - Props.PaddingSize; X >= Props.x + Props.PaddingSize; X--)
            {
              if ((X % (((Props.x2 - Props.x) - Props.PaddingSize) / (abs(H - L) / Inc)) != 0) | BarsOff) // (X <= T) &&
              {
                _Disp->drawFastVLine(X, Props.y + Props.PaddingSize, (Props.y2 - Props.y) - Props.PaddingSize * 2, (X >= T) ? Props.C1 : Props.C2);
              }
            }
          }
        }
        else
        {
          if (Props.Direction == BTT)
          {
            for (int Y = Props.y2 - Props.PaddingSize; Y >= Props.y + Props.PaddingSize; Y--)
            {
              if ((Y % (((Props.y2 - Props.y) - Props.PaddingSize) / (abs(H - L) / Inc)) != 0) | BarsOff) // (Y >= T) &&
              {
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, (Y >= T) ? Props.C1 : Props.C2);
              }
            }
          }
          else
          {
            for (int Y = Props.y + Props.PaddingSize; Y <= Props.y2 - Props.PaddingSize; Y++)
            {
              if ((Y % (((Props.y2 - Props.y) - Props.PaddingSize) / (abs(H - L) / Inc)) != 0) | BarsOff) // (Y >= T) &&
              {
                _Disp->drawFastHLine(Props.x + Props.PaddingSize, Y, (Props.x2 - Props.x) - Props.PaddingSize * 2, (Y <= T) ? Props.C1 : Props.C2);
              }
            }
          }
        }
        if (T > -1)
          Out = Map(T, (orient ? Props.y : Props.x), (orient ? Props.y2 : Props.x2), H, L - 1);
      }
	  //Restore_MainColor;
      return Out;
    }

    void SetStartingValue(long SV)
    {
      if (SV == -1)
        T = (orient ? Props.y2 : Props.x2);
      else
      {
        if (SV > H) SV = H;
        if (SV < L) SV = L;
        T = Map(SV, L, H, (orient ? Props.y2 : Props.x2), (orient ? Props.y : Props.x));
      }

    }

    void Padding(byte padding = 0, uint16_t padColor = WHITE, bool barsoff = 0)
    {
      Props.PaddingSize = padding;
      Props.PadColor = padColor;
      BarsOff = barsoff;
    }

    void SetRange(long l = 0, long h = 10, byte inc = 1)
    {
      L = l;
      H = h;
      Inc = ((inc < 1) ? 1 : inc);
    }

    void SetValue(long Val = -1)
    {
      if (Val == -1)
        Value = Val;
      else
      {
        int SL = (orient ? Props.y : Props.x);
        int SH = (orient ? Props.y2 : Props.x2);

        if (Props.Direction == RTL || Props.Direction == BTT)
          Swap(SL, SH);

        Value = Map(Val, L, H, SL, SH);
      }
    }

    long GetValue()
    {
      return Value;
    }

    void Unlock()
    {
      locked = false;
    }

	uint16_t getButtonPressedColor()
	{
	  return Props.C1;
	}
	
	uint16_t getButtonReleasedColor()
	{
	  return Props.C2;
	}
	
	uint16_t getButtonPaddingColor()
	{
	  return Props.PadColor;
	}
	
	Properties getButtonProperties()
	{
	  return Props;
	}
  private:
    bool locked;//1
    bool State;//1
    bool BarsOff;
    long L, H;//8
    byte Inc;//1
    long Value;//2
    int T;
};

class Swipe : public Base
{
  protected:
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
  
    struct Properties
	{
	  int x;
	  int y;
	  int width;
	  int height;
	  int x2;
	  int y2;
	}Props;
	
    Swipe(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
      ShowSwipeArea(false);
    }
    
	~Swipe(){ }
	
    void Coords(int x1, int y1, int x2, int y2)
    {
      Props.x = x1;
      Props.y = y1;
      Props.x2 = x2;
      Props.y2 = y2;
	  
	  Props.width = x2-x1;
	  Props.height = y2-y1;
    }
	
	void setLimits(byte X = 10, byte Y = 10)
	{
	  Xlimit = X;
	  Ylimit = Y;
	} 

    void ShowSwipeArea(bool En = false, uint16_t color = RED, byte type = SQUARED)
    {
	  //Save_MainColor;
	  if(En)
        (type? _Disp->drawRoundRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, 2, color) : _Disp->drawRect(Props.x, Props.y, Props.x2-Props.x, Props.y2-Props.y, color));
	  //Restore_MainColor;
    }

    bool getTouchState()
    {
	  if(_Touch->dataAvailable())
	  {
		_Touch->read();
	    int x, y;
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}
		return (IsWithin(x, Props.x, Props.x2) & IsWithin(y, Props.y, Props.y2));
	  }
	  else
	    return false;
    }

    byte SwipeFromArea(byte dir = 0x0, uint16_t lenght = 50)
    {
      static int FTouchX, FTouchY, STouchX, STouchY, touchX, touchY;
      static bool gotFirst = 0;
      byte Direction = 0;

      if (!_Touch->dataAvailable())
      {
        gotFirst = false;
        FTouchX = 0; FTouchY = 0; STouchX = 0; STouchY = 0;
      }

      if (!gotFirst && getTouchState())
      {
        _Touch->read();
        touchX = _Touch->getX();
        touchY = _Touch->getY();
        FTouchX = (touchX > _Disp->width() ? 0 : touchX);
        FTouchY = (touchY > _Disp->height() ? 0 : touchY);
        STouchY = FTouchY;
        STouchX = FTouchX;
        gotFirst = true;
      }

      while (_Touch->dataAvailable() && gotFirst == true)
      {
        _Touch->read();
        touchX = _Touch->getX();
        touchY = _Touch->getY();
        STouchX = (touchX > _Disp->width() ? 0 : touchX);
        STouchY = (touchY > _Disp->height() ? 0 : touchY);

        //look for a specific direction
        if ((abs(STouchY - FTouchY) >= lenght) && (abs(STouchX - FTouchX) >= lenght))
        {
          if (     (STouchY < FTouchY) && (STouchX < FTouchX)) Direction = Swipe_upLeft; // upleft
          else if ((STouchY > FTouchY) && (STouchX < FTouchX)) Direction = Swipe_downLeft; // downLeft
          else if ((STouchY < FTouchY) && (STouchX > FTouchX)) Direction = Swipe_upRight; // upRight
          else if ((STouchY > FTouchY) && (STouchX > FTouchX)) Direction = Swipe_downRight; // downRight
        }
        //get a swipe direction for the Y axis
        else if ((abs(STouchY - FTouchY) >= lenght) && IsWithin(FTouchX, Props.x + Xlimit, Props.x2 - Xlimit))
        {
          if (STouchY < FTouchY) Direction = Swipe_up; // up
          else if (STouchY > FTouchY) Direction = Swipe_down; // down
        }
        //get a swipe direction for X axis
        else if ((abs(STouchX - FTouchX) >= lenght) && IsWithin(FTouchY, Props.y + Ylimit, Props.y2 - Ylimit))
        {
          if (STouchX < FTouchX) Direction = Swipe_left; // left
          else if (STouchX > FTouchX) Direction = Swipe_right; // right
        }

        if (Direction > 0) break;
      }

      if (dir > 0)
        return (dir & Direction);
      else
        return Direction;
    }
    
	Properties getButtonProperties()
	{
	  return Props;
	}
	
  private:
	byte Xlimit, Ylimit;
};

//-------------------------------------------------
//     **This section is a work in progress**
//-------------------------------------------------

class CustomButton : public Base
{
  private:
    #define NumOfPoints 20
    byte CBsize;
	
  protected:
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    struct Points
    {
      long x;
      long y;
    };
	
	Points CustPoints[NumOfPoints];

    CustomButton(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
	  lastTouch = 0, gotfirst = false, released = false;
      lastState = 1;
      lockout = false;
	  touch = 0;
    }
	
	~CustomButton(){ }
	
    template<size_t N>
    void Coords(Points (&points)[N])
	{
	  for(byte i = 0; i < N; i++)
	    CustPoints[i] = points[i];
	  CBsize = N;
	}
	
    void Colors(uint16_t c1, uint16_t c2, bool fill)
    {
      C1 = c1;
      C2 = c2;
      Fill = fill;
    }

    void fillCB(int x1, int y1, int x2, int y2, int x3, int y3, int color)
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

	  A = (y2 - y1)? (float(x2 - x1) / float(y2 - y1)) : (x2 - x1);
	  B = (y3 - y1)? (float(x3 - x1) / float(y3 - y1)) : (x3 - x1);
	  C = (y3 - y2)? (float(x3 - x2) / float(y3 - y2)) : (x3 - x2);

      for (int Dy = y1; Dy <= y3; Dy++)
      {
        if (Dy <= y2)
          XL = x1 + A * (Dy - y1);
        else
          XL = x2 + C * (Dy - y2);

        XR = x1 + B * (Dy - y1);

        _Disp->drawFastHLine(XL, Dy, (XR - XL), color);
      }
    }

    long AreaCB(long Ax, long Ay, long Bx, long By, long Cx, long Cy)
    {
      return abs(((Ax * (By - Cy)) + (Bx * (Cy - Ay)) + (Cx * (Ay - By))) / 2);
    }

    bool PointInCustButton(long tx, long ty, long Cx, long Cy, long Cx1, long Cy1, long Cx2, long Cy2)
    {
      long A = AreaCB(Cx, Cy, Cx1, Cy1, Cx2, Cy2); // This is the ideal area of the triangle based on the 3 points.
      long A1 = AreaCB(tx, ty, Cx1, Cy1, Cx2, Cy2);// this checks to see if the touch coords are within the created triangle.
      long A2 = AreaCB(Cx, Cy, tx, ty, Cx2, Cy2);  // The reason three are needed is because each one is a section of the triangle.
      long A3 = AreaCB(Cx, Cy, Cx1, Cy1, tx, ty);

      return (A >= (A1 + A2 + A3));
    }

    bool getTouchState()
    {
	  if(_Touch->dataAvailable())
	  {
		for (byte i = 0; i < (CBsize - 1); ++i)
		{
			_Touch->read();
			int x, y;
			
			switch(_Disp->getRotation())
			{
			case 0:
				y = _Disp->height() - _Touch->getX();
				x =  _Touch->getY();
				break;
				
			case 1:
				x = _Disp->width() - _Touch->getX();
				y = _Disp->height() - _Touch->getY();
				break;
				
			case 2:
				y = _Touch->getX();
				x = _Disp->width() - _Touch->getY();
				break;
				
			case 3:
				x = _Touch->getX();
				y = _Touch->getY();	
				break;
			}
			if (PointInCustButton(x, y, CustPoints[i].x, CustPoints[i].y, CustPoints[i + 1].x, CustPoints[i + 1].y, CustPoints[CBsize - 1 - i].x, CustPoints[CBsize - 1 - i].y))
				return true;
		}
	  }
      return false;
    }

    bool Touch(bool draw = true)
    {
      State = this->getTouchState(); // If the buttons coords are touched, return true.
      if (draw)
        this->Draw();
      return State; // button coords were not touched, return false.
    }

    bool Toggle()
    {
      if ( this->getTouchState() )
      {
        if (Lock == true) // If this button is pressed, set the latch.
        {
          State = !State;
          Lock = false; // Only allow it to be pressed once and not held.
          this->Draw();
        }
      }
      else Lock = true; // Once released allow it to be pressed again.

      return State; // Return the buttons state
    }

    bool Delay(unsigned long T = 1000)
    { 
      touch = this->getTouchState();
	  
      if (touch==true && lockout == false)
      {
        B_current_time = millis();
        lockout = true;
      }

      if (touch && lockout)
        State = ((millis() - B_current_time) >= T);
	  else 
	  {
	    State = false; 
	    lockout = false;
	  }

      this->Draw();

      return State;
    }
    
	bool DoubleClick(unsigned long timeout = 500000UL)
	{
	  static unsigned long time = micros();
	  
	  bool touch = this->getTouchState();
	  
      if (touch != lastTouch) // get first touch 
      {
	    if(touch)
		{
		  gotfirst = true;
		  time = micros(); // record the time
		}
		lastTouch = touch;
      }
	  
	  if(gotfirst && !touch) // see if it was released
	    released = true;
		
	  if(gotfirst && ((micros() - time) >= timeout)) // check the timeout timer, if it sees that it was touched then\
	  then not touched again, reset released and gotfirst .
	  {
	    released = false;
		gotfirst = false;
	  }
	  
	  if(!touch && !gotfirst) // If not touched and it sees that it was not touched at all, set released to false.
	    released = false;
	  
	  if(touch & released) // if it is being touched and sees that it was previously released, then the user double clicked
	  {
	    gotfirst = false;
	    State = true;
	  }
	  else 
	    State = false; // show it is not being touched
		
	  this->Draw();
	  return State;
	}
	
    void Draw()
    {
	  //Save_MainColor;
      if (State != lastState)
      {
        for (int i = 0; i < (CBsize - 1); ++i)
          _Disp->drawLine(CustPoints[i].x, CustPoints[i].y, CustPoints[i + 1].x, CustPoints[i + 1].y, State ? C1 : C2);

        _Disp->drawLine(CustPoints[0].x, CustPoints[0].y, CustPoints[CBsize - 1].x, CustPoints[CBsize - 1].y, State ? C1 : C2);

        if (Fill)
        {
          for (byte i = 0; i < (CBsize - 1); ++i) {
            fillCB(CustPoints[i].x, CustPoints[i].y, CustPoints[i + 1].x, CustPoints[i + 1].y, CustPoints[CBsize - 1 - i].x, CustPoints[CBsize - 1 - i].y, State ? C1 : C2);
          }
        }
        lastState = State;
      }
	  //Restore_MainColor;
    }

    template<size_t N>
    long Area(Points (&point)[N])
    {
      byte numVertices = N;
      long area = 0;

      for (int i = 0; i < numVertices - 1; ++i)
        area += point[i].x * point[i + 1].y - point[i + 1].x * point[i].y;

      area += point[numVertices - 1].x * point[0].y - point[0].x * point[numVertices - 1].y;

      return abs(area) / 2;
    }
	
	void SetState(bool S)
    {
      State = S;
    }
	
	byte getState()
	{
	  return this->State;
	}
	
	int getButtonPressedColor()
	{
	  return C1;
	}
	
	int getButtonReleasedColor()
	{
	  return C2;
	}
	
  private:
    uint16_t C1, C2;
    bool Fill, State, lastState, Lock;
    unsigned long B_current_time;
    bool lockout, touch;
	byte lastTouch, gotfirst, released;
};

#if 1
class MobileKeyboard : public Base
{
  protected:
    TFT_Display *_Disp;
    TOUCH *_Touch;
    Base *_base;

  public:
    MobileKeyboard(Base * B): _base(B)
    {
      _Disp = _base->getDisplay();
      _Touch = _base->getTouch();
	  byte Dir = _Disp->getRotation();
	  if((Dir == 1) || (Dir == 3))
	    BUF = 27;
	  else
	    BUF = 18;
    }
	
	~MobileKeyboard(){ }
    
	void Colors(uint16_t text, uint16_t background, uint16_t box, bool fill = false)
	{
	  Textcolor = text;
	  Backgnd = (fill?box : background);
	  Boxcolor = box;
	  Fill = fill;
	}
	
    void SetupMobileKB(int X, int Y, float SX = -1, float SY = -1)
    {
	  // don't need this anymore
    }
    
	void make_Mobile_Keyboard(const char type[][13])
	{
	  byte Dir = _Disp->getRotation();
	  if((Dir == 1) || (Dir == 3))
	    make_Mobile_Keyboard_L(type);
      else  
	    make_Mobile_Keyboard_P(type);
	}
	
	void make_Mobile_Keyboard_L(const char type[][13])
	{
	  _Disp->setTextSize(2);
	  _Disp->setTextColor(Textcolor, Backgnd);
	  for (int y = 0; y < 3; y++)
	  {
		int ShiftRight = 15 * pgm_read_byte(&(type[y][0]));
		for (int x = 3; x < 13; x++)
		{
		  if (x >= pgm_read_byte(&(type[y][1]))) break;

		  drawButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25); // this will draw the button on the screen by so many pixels
		  _Disp->setCursor(20 + (30 * (x - 3)) + ShiftRight, 105 + (30 * y));
		  _Disp->print(char(pgm_read_byte(&(type[y][x]))));
		}
	  }
	  //ShiftKey
	  drawButton(15, 160, 35, 25);
	  _Disp->setCursor(27, 168);
	  _Disp->print('^');

	  //Special Characters
	  drawButton(15, 190, 35, 25);
	  _Disp->setCursor(21, 195);
	  _Disp->print(F("SP"));

	  //BackSpace
	  drawButton(270, 160, 35, 25);
	  _Disp->setCursor(276, 165);
	  _Disp->print(F("BS"));

	  //Return
	  drawButton(270, 190, 35, 25);
	  _Disp->setCursor(276, 195);
	  _Disp->print(F("RT"));

	  //Spacebar
	  drawButton(60, 190, 200, 25);
	  _Disp->setCursor(105, 195);
	  _Disp->print(F("SPACE BAR"));
	}
	
	void make_Mobile_Keyboard_P(const char type[][13])
	{
	  _Disp->setTextSize(2);
	  _Disp->setTextColor(Textcolor, Backgnd);
	  for (int y = 0; y < 3; y++)
	  {
		int ShiftRight = 10 * pgm_read_byte(&(type[y][0]));
		for (int x = 3; x < 13; x++)
		{
		  if (x >= pgm_read_byte(&(type[y][1]))) break;

		  drawButton(8 + (23 * (x - 3)) + ShiftRight, 200 + (30 * y), 20,25); // this will draw the button on the screen by so many pixels
		  _Disp->setCursor(12 + (23 * (x - 3)) + ShiftRight, 205 + (30 * y));
		  _Disp->print(char(pgm_read_byte(&(type[y][x]))));
		}
	  }
	  //ShiftKey
	  drawButton(5, 260, 30, 25);
	  _Disp->setCursor(15, 265);
	  _Disp->print(F("^"));

	  //Special Characters
	  drawButton(5, 290, 30, 25);
	  _Disp->setCursor(9, 200 + 95);
	  _Disp->print(F("SP"));

	  //BackSpace
	  drawButton(200, 260, 30, 25);
	  _Disp->setCursor(204, 200 + 65);
	  _Disp->print(F("BS"));

	  //Return
	  drawButton(200, 290, 30, 25);
	  _Disp->setCursor(204, 200 + 95);
	  _Disp->print(F("RT"));

	  //Spacebar
	  drawButton(47, 290, 140, 25);
	  _Disp->setCursor(65, 200 + 95);
	  _Disp->print(F("SPACE BAR"));
	}

	void drawButton(int x, int y, int w, int h)
	{
	  // grey
	  _Disp->fillRoundRect(x - 3, y + 3, w, h, 3, Backgnd/2); //Button Shading

	  // white
	  _Disp->fillRoundRect(x, y, w, h, 3, Boxcolor);// outter button color

	  //red
	  _Disp->fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, Backgnd); //inner button color
	}
	
    void clearMSG()
    {
      memset(MSG, ' ', BUF);
    }

	char * Mobile_KeyBoard(char * myMSG, uint16_t color = 0xFFFF)
	{
	  byte Dir = _Disp->getRotation();
	  if((Dir == 1) || (Dir == 3))
	    Mobile_KeyBoard_L(myMSG, color);
	  else 
	    Mobile_KeyBoard_P(myMSG, color);
	}
	
	char * Mobile_KeyBoard_L(char * myMSG, uint16_t color = 0xFFFF)
	{
	  char key = 0;
	  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
	  static char bufIndex = 0;

	  if(_Touch->dataAvailable())
	  {
		//ShiftKey
		if (TouchButton(15, 160, 35, 25))
		{
		  shift = !shift;
		  delay(200);
		}

		//Special Characters
		if (TouchButton(15, 190, 35, 25))
		{
		  special = !special;
		  delay(200);
		}

		if (special != lastSp || shift != lastSh)
		{
		  if (special)
		  {
			if (shift)
			{
			  _Disp->fillScreen(Backgnd);
			  make_Mobile_Keyboard_L(Mobile_SymKeys);
			}
			else
			{
			  _Disp->fillScreen(Backgnd);
			  make_Mobile_Keyboard_L(Mobile_NumKeys);
			}
		  }
		  else
		  {
			_Disp->fillScreen(Backgnd);
			make_Mobile_Keyboard_L(Mobile_KB);
			_Disp->setTextColor(Textcolor, Backgnd);
		  }

		  if (special)
			_Disp->setTextColor(0x0FF0, Backgnd);
		  else
			_Disp->setTextColor((Backgnd != 0xFFFF? 0xFFFF : 0), Backgnd);

		  _Disp->setCursor(21, 195);
		  _Disp->print(F("SP"));

		  if (shift)
			_Disp->setTextColor(0x0FF0, Backgnd);
		  else
			_Disp->setTextColor((Backgnd != 0xFFFF? 0xFFFF : 0), Backgnd);

		  _Disp->setCursor(27, 168);
		  _Disp->print(F("^"));

		  lastSh = shift;

		  lastSp = special;
		  lastSh = shift;
		}

		for (int y = 0; y < 3; y++)
		{
		  int ShiftRight;
		  if (special)
		  {
			if (shift)
			  ShiftRight = 15 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
			else
			  ShiftRight = 15 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
		  }
		  else
			ShiftRight = 15 * pgm_read_byte(&(Mobile_KB[y][0]));

		  for (int x = 3; x < 13; x++)
		  {
			if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) break;

			if (TouchButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25)) // this will draw the button on the screen by so many pixels
			{
			  if (bufIndex < (BUF - 1))
			  {
				delay(200);

				if (special)
				{
				  if (shift)
					MSG[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
				  else
					MSG[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
				}
				else
				  MSG[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

				bufIndex++;
			  }
			  break;
			}
		  }
		}

		//Spacebar
		if (TouchButton(60, 190, 200, 25))
		{
		  MSG[bufIndex++] = ' ';
		  delay(200);
		}

		//BackSpace
		if (TouchButton(270, 160, 35, 25))
		{
		  if ((bufIndex) > 0)
			bufIndex--;
		  MSG[bufIndex] = 0;
		  _Disp->setTextColor(1, Backgnd);
		  _Disp->setCursor(15, 80);
		  _Disp->print(F("                          "));
		  delay(200);
		}

		//Return
		if (TouchButton(270, 190, 35, 25))
		{
		  Serial.println(MSG);
		  MSG[bufIndex] = '\0';
		  
		  strcpy(myMSG, MSG);
		  clearMSG();
		  bufIndex = 0;

		  _Disp->setTextColor(1, Backgnd);
		  _Disp->setCursor(15, 80);
		  _Disp->print(F("                         "));
		}
	  }
	  _Disp->setTextColor(color, Backgnd);
	  _Disp->setCursor(15, 80);
	  MSG[bufIndex] = '\0';
	  _Disp->print(MSG);
	}
	
	char * Mobile_KeyBoard_P(char * myMSG, uint16_t color = 0xFFFF)
	{
	  char key = 0;
	  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
	  static char bufIndex = 0;

	  if (_Touch->dataAvailable())
	  {
		//ShiftKey
		if (TouchButton(5, 260, 30, 25))
		{
		  shift = !shift;
		  delay(200);
		}

		//Special Characters
		if (TouchButton(5, 290, 30, 25))
		{
		  special = !special;
		  delay(200);
		}

		if (special != lastSp || shift != lastSh)
		{
		  if (special)
		  {
			if (shift)
			{
			  _Disp->fillScreen(Backgnd);
			  make_Mobile_Keyboard_P(Mobile_SymKeys);
			}
			else
			{
			  _Disp->fillScreen(Backgnd);
			  make_Mobile_Keyboard_P(Mobile_NumKeys);
			}
		  }
		  else
		  {
			_Disp->fillScreen(Backgnd);
			make_Mobile_Keyboard_P(Mobile_KB);
			_Disp->setTextColor(0xffff, Backgnd);
		  }

		  if (special)
			_Disp->setTextColor(0x0FF0, Backgnd);
		  else
			_Disp->setTextColor(Textcolor, Backgnd);

		  _Disp->setCursor(9, 295);
		  _Disp->print(F("SP"));

		  if (shift)
			_Disp->setTextColor(0x0FF0, Backgnd);
		  else
			_Disp->setTextColor(Textcolor, Backgnd);

		  _Disp->setCursor(15, 265);
		  _Disp->print(F("^"));

		  lastSh = shift;

		  lastSp = special;
		  lastSh = shift;
		}

		for (int y = 0; y < 3; y++)
		{
		  int ShiftRight;
		  if (special)
		  {
			if (shift)
			  ShiftRight = 10 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
			else
			  ShiftRight = 10 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
		  }
		  else
			ShiftRight = 10 * pgm_read_byte(&(Mobile_KB[y][0]));

		  for (int x = 3; x < 13; x++)
		  {
			if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) 
			  break;

			if (TouchButton(8 + (23 * (x - 3)) + ShiftRight, 200 + (30 * y), 20,25)) // this will draw the button on the screen by so many pixels
			{
			  if (bufIndex < (BUF - 1))
			  {
				delay(200);

				if (special)
				{
				  if (shift)
					MSG[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
				  else
					MSG[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
				}
				else
				  MSG[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

				bufIndex++;
			  }
			  break;
			}
		  }
		}

		//Spacebar
		if (TouchButton(47, 290, 140, 25))
		{
		  MSG[bufIndex++] = ' ';
		  delay(200);
		}

		//BackSpace
		if (TouchButton(200, 260, 30, 25))
		{
		  if ((bufIndex) > 0)
			bufIndex--;
		  MSG[bufIndex] = 0;
		  _Disp->setTextColor(0, Backgnd);
		  _Disp->setCursor(15, 180);
		  _Disp->print(F("              "));
		  delay(200);
		}

		//Return
		if (TouchButton(200, 290, 30, 25))
		{
		  Serial.println(MSG);
		  MSG[bufIndex] = '\0'; 
		  strcpy(myMSG, MSG);
		  
		  clearMSG();
          bufIndex = 0;
		  
		  _Disp->setTextColor(0, Backgnd);
		  _Disp->setCursor(15, 180);
		  _Disp->print(F("              "));
		}
	  }
	  _Disp->setTextColor(color, Backgnd);
	  _Disp->setCursor(15, 180);
	  _Disp->print(MSG);
	}

    void ReceiveMsg(int X, int Y, uint16_t color)
    {
      //Save_MainColor;
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
      if (Serial1.available() > 0)
      {
        if (!RCV_locked)
        {
          clearRCVMSG();
		  _Disp->setTextColor(color, Backgnd);
		  _Disp->setCursor(X, Y);
          _Disp->print(ReceiveMSG);
          RCV_locked = true;
        }
        char R_MSG = Serial1.read();
        if ( R_MSG != '\n')
        {
          ReceiveMSG[RCV_cnt] = R_MSG;
          RCV_cnt++;
        }
        else
        {
          //Restore_MainColor;
		  _Disp->setTextColor(color, Backgnd);
		  _Disp->setCursor(X, Y);
          _Disp->print(ReceiveMSG);
          RCV_cnt = 0;
          RCV_locked = false;
        }
      }
#endif
      //Restore_MainColor;
    }

    void clearRCVMSG()
    {
      memset(ReceiveMSG, ' ', sizeof(ReceiveMSG));
    }

  private:
    int 	XoffSet, YoffSet, TxtoffSet;
    int		idx;
	byte    BUF;
    char    MSG[30];
    char    RET_MSG[30];
    byte 	RCV_cnt;
    bool	RCV_locked;
    char 	ReceiveMSG[30];
    byte 	_FONT;
    bool	_TYPE;
    bool	Type, _symbol;
    uint16_t	FrontColor, BackColor;
    float 	ScaleX, ScaleY;
    bool    Shift;
	uint16_t	Textcolor, Backgnd, Boxcolor;
	bool 	Fill;

    bool TouchButton(int x1, int y1, int w, int h)
    {
		_Touch->read();
	    int x, y;
		
		switch(_Disp->getRotation())
		{
		case 0:
			y = _Disp->height() - _Touch->getX();
			x =  _Touch->getY();
			break;
			
		case 1:
			x = _Disp->width() - _Touch->getX();
			y = _Disp->height() - _Touch->getY();
			break;
			
		case 2:
			y = _Touch->getX();
			x = _Disp->width() - _Touch->getY();
			break;
			
		case 3:
			x = _Touch->getX();
			y = _Touch->getY();	
			break;
		}
		
		return (IsWithin(x, x1, x1+w) & IsWithin(y, y1, y1+h));// If the buttons coords are touched, return true.
    }
};
#endif

class Cycle 
{
    public:
	Cycle() {lastStateUp = 0; lastStateDn = 0; counter = -1;}
	~Cycle() {}
	
    template<class B, class Func, size_t N>
    byte CycleButton(B button, Func(&_functions)[N])
    {	
	    byte state = button->Touch(true);
	    if(state != lastStateUp)
	    { 
	        if(state == true)
		    {
		      counter++;
			  if(counter > N-1)
			    counter = 0;
		    }
			lastStateUp = state;
		    _functions[counter]();
	    }
	}
	
	template<class B_1, class B_2, class Func, size_t N>
    byte CycleButton(B_1 buttonUp, B_2 buttonDn, Func(&_functions)[N], byte roll = false)
    {	
	    byte stateUp = buttonUp->Touch(true);
		byte stateDn = buttonDn->Touch(true);
		
	    if(stateUp != lastStateUp)
	    { 
	        if(stateUp == true)
		    {
		      counter++;
			  if(counter > N-1)
			    counter = roll? 0 : N-1;
		    }
			lastStateUp = stateUp;
		    _functions[counter]();
	    }
		
		if(stateDn != lastStateDn)
	    { 
	        if(stateDn == true)
		    {
		      counter--;
			  if(counter < 0)
			    counter = roll? N-1 : 0;
		    }
			lastStateDn = stateDn;
		    _functions[counter]();
	    }
	}
	private:
	   byte lastStateUp, lastStateDn;
	   int counter; // wont work correctly as type char
};
#endif