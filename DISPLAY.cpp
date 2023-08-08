#include <stdio.h>
//#include <conio.h>
//#include <dos.h>
#include <string.h>
#include <Arduino.h>

#include "ex.h"
#include "DISPLAY.h"

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF 0x00

// flags for function set
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08


//#define BitDisplStrobe 0x80	       // бит 7 (P2.7)


int _displaycontrol;
int _displaymode;
int _rs_pin = 2;
int _rw_pin = 255;
int _enable_pin = 3;
int _row_offsets[4];
int _data_pins[4];
int _displayfunction;
int _numlines = 4;

static void EXdisplay(int value, int mode);

//DISPL_PARAM	DisplParam;

/***************************************************************************\
  <ConfigDisplay> DP-4x20 VF, нет курсора
\***************************************************************************/
void ConfigDisplay(void)
{

    _data_pins[0] = 4;
   _data_pins[1] = 5;
   _data_pins[2] = 6;
   _data_pins[3] = 7; 
  
  _displayfunction = LCD_4BITMODE;

  _displayfunction |= LCD_2LINE;

  //set rows
   _row_offsets[0] = 0x00;
   _row_offsets[1] = 0x40;
   _row_offsets[2] = 0x00 + 20;
   _row_offsets[3] = 0x40 + 20;

  pinMode (_rs_pin, OUTPUT);
  if (_rw_pin != 255) { 
    pinMode (_rw_pin, OUTPUT);
  }
  pinMode (_enable_pin, OUTPUT);

  for (int i=0; i< 4; ++i)
  {
    pinMode (_data_pins[i], OUTPUT);
  } 

  delayMicroseconds(50000); 

  digitalWrite (_rs_pin, LOW);
  digitalWrite (_enable_pin, LOW);
  if (_rw_pin != 255) { 
    digitalWrite (_rw_pin, LOW);
  }

  write4bits (0x03);
  delayMicroseconds (4500); // wait min 4.1ms

  write4bits (0x03);
  delayMicroseconds (4500); // wait min 4.1ms
    
  write4bits(0x03); 
  delayMicroseconds(150);

  write4bits(0x02); 
  
  //set # lines, font size, etc.
  EXdisplay(LCD_FUNCTIONSET |_displayfunction,LOW);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  

  _displaycontrol |= LCD_DISPLAYON;
  EXdisplay(LCD_DISPLAYCONTROL | _displaycontrol,LOW);

  // clear 
  ClsDisplay();
  //leftToRight();

  // set the entry mode
  EXdisplay(LCD_ENTRYMODESET | _displaymode, LOW);

 _displaymode |= LCD_ENTRYLEFT;
 EXdisplay(LCD_ENTRYMODESET | _displaymode,LOW);

};

/***************************************************************************\
  <ClsDisplay>
\***************************************************************************/
void ClsDisplay(void)
{
  EXdisplay(0x01, LOW);
  delayMicroseconds(2000);  // this command takes a long time!
};

int x1,y1;
char str1;
int as(){
return str1;
}

/***************************************************************************\
  <DisplayYX>
\***************************************************************************/
void DisplayYX(int y, int x, char *str)//x,y-установить курсор
{
    str1=str;
    //x-row
int i;

 const int max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( x >= max_lines ) {
    x = max_lines - 1;    // we count rows starting w/0
  }
  if ( x >= _numlines ) {
    x = _numlines - 1;    // we count rows starting w/0
  }
EXdisplay(LCD_SETDDRAMADDR | (y + _row_offsets[x]), LOW);

if (str == NULL) return 0;  //проверка на пустой указатель
  else{
    const uint8_t *buffer=(const uint8_t *)str;
    long size=strlen(str);
    while (size--) {//берёт указатель, размер. Отправляет на печать пока размер не = 0
      if (write(*buffer++));
      else break;
    }
  }
}


long write(int value) {
  EXdisplay(value, HIGH);
  return value; // assume sucess
}

/***************************************************************************\
  <Display>
\***************************************************************************/
// void Display(char *str)
// {
// int i;
//   for (i=0; i<strlen(str); i++)
//        EXdisplay(*(str+i), 1);
// }

/* *************************************************************
*	процедура вывода 7-ми битного кода в дисплей           *
** ************************************************************/
static void EXdisplay(int value, int mode)
{
		//!!!!! 29.06.07 ConfigPIO();

	digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, HIGH);
  }
    write4bits(value>>4);
    write4bits(value);
}

void write4bits(int value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}
