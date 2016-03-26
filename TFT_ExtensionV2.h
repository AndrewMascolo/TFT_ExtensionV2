#ifndef TFT_ExtensionV2_h
#define TFT_ExtensionV2_h

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

//Version 2.12
// Adafruit extension now allows for buttons to be used for any orientation
// Until Henning Karlsen allows for reversed landscape and portrait orientations, I will hold off on adding that functionality to the UTFT extension.
// The color PINK was changed to REG_PINK to try and avoid issues with the SD card library.
// I changed the structure of the buttons attributes by wrapping everything inside a structure that can then be returned from a function. Ex. myButton.getButtonProperties().width;

//Version 2.11
// Added in the following functions:
// getBackColor
// getState
// getButtonPressedColor		(All button types)
// getButtonReleasedColor 		(All button types)	
// getButtonTextColor			(Box and Circle buttons)
// getButtonPaddingColor		(Box and Circle buttons)
// getButtonHeadFootColor 		(Box and Circle buttons)
// getText						(Box and Circle buttons)

// Version 2.10
// Added new DoubleClick() method to all button types. 
// Fixed the ReDraw() method to not only reset the button's appearance state, but to redraw it too (makes sense). 
// Gauge class is now finished and is usable with my TFT_ScaleFonts library for bigger center text.
// Making necessary conversions to allow the library to be compatible with the Adafruit_ILI9341 library and possibly others in the future.

// Version 2.09
// added XY limits to Swipe class so now it is more precise in either direction
// added background color option to Slider class Text functions
// split library into two separate libraries to reduce memory consumption when not using the touch screen.
// Gauge class is still unfinished and will most be part of the TFTE_Visuals library in the near future

// Version 2.08
// Custombuttons can now be used with the Radio class
// The defines for the triangle set degrees (left, right, up, down) have been changed
// to Tri_up, Tri_down... etc.
// Cycle class now has the option to allow rollover (Roll and NoRoll) for the two button function. Default is NoRoll.

// Version 2.07
// Added Cycle class
// took out text option from triangle class
// Redid the Delay functions for each button type to non-blocking.

// Version 2.06
// fixed the background text in all the buttons. (only shaves off a few microseconds)
// Fixed the keyboard to now allow colors and fill.
// Fixed the triangle class
// All buttons can now auto size themselves with the argument "AUTO" in place of the X2,Y2 coordinates or Radius.
// ** AUTO sizes the button based on the text with the most characters **
// took out sprintf from Meter class (reduced memory usage by 7,242 bytes!)

// Version 2.05
// Changed the class name from ProgressBar to Meter.
// Made a "new" class called Progressbar 
// added new feature that allows all buttons to have header and footer text.
// added save and restore main color macro to all classes that change the foreground and background colors
// change the function name "Latch" to "Toggle"

// Version 2.04
// Added Keyboard class

// Version 2.03
// Added custom button class and swipe class

// Version 2.02 
// minor bug fix with all buttons

// Version 2.01
// introducing the Box, Circle and Triangle classes

int FrontColor, BackColor;
byte * Font;

#if defined UTFT_h
#define TFT_Display UTFT
#define Save_MainColor 		do{ FrontColor = _Disp->getColor(); BackColor = _Disp->getBackColor(); Font = _Disp->getFont();}while(0)
#define Restore_MainColor	do{ _Disp->setColor(FrontColor); _Disp->setBackColor(BackColor); _Disp->setFont(Font);}while(0)
#elif defined _ADAFRUIT_ILI9341H_
#define TFT_Display Adafruit_ILI9341
#endif

#if defined Core_Ext_h
#define TFTE_Map custmap<long>
#else
#define TFTE_Map map
#endif


#ifdef UTouch_h
#define TOUCH UTouch
#else
#define TOUCH ITDB02_Touch
#endif

#define IsWithin(x, a, b) ((x >= a) && (x <= b))

#define Tri_up 0
#define Tri_down 180
#define Tri_left 90
#define Tri_right 270
#define Deg_to_rad 0.01745 + 3.14159265

#define AUTO -1
#define HORIZONTAL 0
#define VERTICAL 1

#define Big 0x1
#define Small 0x0
#define FILL 1
#define NOFILL 0
#define ROUNDED 1
#define SQUARED 0
#define NoBars 1
#define Bars 0
#define NoRoll 0
#define Roll 1
#define INSIDE 0
#define OUTSIDE 1

//=====================COLOR_PALLET==========================
#define BLACK   		0x0
#define BROWN 			0x6180
#define LIGHT_RED		0xFD14
#define RED     		0xF800
#define DARK_RED   		0x6000
#define LIGHT_ORANGE 	0xFF52
#define ORANGE  		0xFD00
#define DARK_ORANGE 	0xFA80
#define LIGHT_YELLOW 	0xFFF4
#define YELLOW  		0xD7E0
#define DARK_YELLOW 	0xCE40
#define LIGHT_GREEN 	0xB7F6
#define GREEN   		0x07E0
#define DARK_GREEN 		0x0320
#define LIGHT_BLUE 		0xA51F
#define BLUE    		0x001F
#define DARK_BLUE 		0x000C
#define LIGHT_PURPLE 	0xC819
#define PURPLE  		0x9019 
#define DARK_PURPLE 	0x600C
#define REG_PINK 		0xF81F
#define CYAN    		0x07FF
#define GREY    		0x8410
#define SILVER 			0xC618
#define GOLD			0xD566
#define WHITE   		0xFFFF
#define TRANSPARENT 	0xFFFFFFFF
//==================END_OF_COLOR_PALLET======================

#define FILL 1
#define NOFILL 0

#define LTR 1
#define RTL 0
#define BTT 1
#define TTB 0

#define Free_Swipe 		0x00
#define Swipe_up 		0x01
#define Swipe_down 		0x02
#define Swipe_left 		0x04
#define Swipe_right 	0x08
#define Swipe_upLeft 	0x10
#define Swipe_downLeft 	0x20
#define Swipe_upRight 	0x40
#define Swipe_downRight 0x80

#include <avr/pgmspace.h>
#include <Arduino.h>

extern uint8_t SmallFont[];
extern uint8_t BigFont[];

enum positions {TOP = 0xfffd, MIDDLE, BOTTOM};

#if defined UTFT_h
#define kboffset 2
#elif defined _ADAFRUIT_ILI9341H_
#define kboffset 3
#endif

const char Mobile_KB[3][13] PROGMEM = {
  {0, 13, 10, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {1, 12, 9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
  {kboffset, 10, 7, 'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
};

const char Mobile_NumKeys[3][13] PROGMEM = {
  {0, 13, 10, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {0, 13, 10, '-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char Mobile_SymKeys[3][13] PROGMEM = {
  {0, 13, 10, '[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {4, 9, 6, '_', '\\', '|', '~', '<', '>'}, //4
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

template<typename T>
void Swap(T &First, T &Second)
{
  T temp = First;
  First  = Second;
  Second = temp;
}

#define RGB_to_565(R, G, B) ( (int(R & 0xF8) << 8) | (int(G & 0xFC) << 3) | (B >> 3) )

class Base
{
  public:
    Base() {} // Needed for the other class to work properly

#if (defined UTFT_h || defined _ADAFRUIT_ILI9341H_) && defined UTouch_h
    Base(TFT_Display *Disp, TOUCH *Touch): _Disp(Disp), _Touch(Touch) {}
	
	TFT_Display * getDisplay() {
      return _Disp;
    }
	
	TOUCH * getTouch() {
      return _Touch;
    }
#else
	Base(TFT_Display *Disp): _Disp(Disp){}
	
	TFT_Display * getDisplay() {
      return _Disp;
    }
#endif
  protected:
    TFT_Display *_Disp;
	
#if (defined UTFT_h || defined _ADAFRUIT_ILI9341H_) && defined UTouch_h
    TOUCH *_Touch;
#endif
};

//==================Call the libraries=====================
#if defined UTFT_h
#include "Utilities/TFTE_Visuals.h"
#elif defined _ADAFRUIT_ILI9341H_
#include "Utilities/AdaFruit_TFTE_Visuals.h"
#endif

#if defined UTFT_h && defined UTouch_h
#include "Utilities/TFTE_Touchables.h"
#elif defined _ADAFRUIT_ILI9341H_ && defined UTouch_h
#include "Utilities/AdaFruit_TFTE_Touchables.h"
#endif
//=========================================================

#endif