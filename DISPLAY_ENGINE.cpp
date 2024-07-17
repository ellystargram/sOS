/*
* DISPLAY_ENGINE.cpp
*
* Created: 7/26/2023 1:43:57 AM
*  Author: 6gioc
*/
#include "TOOLBOX.h"

//MAX7219 PIN
#define CS PB0
#define CLK PB1
#define DIN PB2

//MAX7219 CONTROL
#define MAX7219_NOP     0x00
#define MAX7219_DIGIT0  0x01
#define MAX7219_DIGIT1  0x02
#define MAX7219_DIGIT2  0x03
#define MAX7219_DIGIT3  0x04
#define MAX7219_DIGIT4  0x05
#define MAX7219_DIGIT5  0x06
#define MAX7219_DIGIT6  0x07
#define MAX7219_DIGIT7  0x08
#define MAX7219_DECODE  0x09
#define MAX7219_INTEN   0x0A
#define MAX7219_SCANLIM 0x0B
#define MAX7219_SHUTDOWN 0x0C
#define MAX7219_TEST    0x0F

//TEXTDISPLAY SPACING
#define CHARSPACE 6

enum DOTMATRIXUPDATESEQUENCE{
	_dimming,
	_line0,
	_line1,
	_line2,
	_line3,
	_line4,
	_line5,
	_line6,
	_line7,
};

void spitrans_(unsigned char _data);
void display_commander_(unsigned char _adress, unsigned char _value);
void cwrotate_(unsigned char* __origin);
void ccwrotate_(unsigned char* __origin);
unsigned char mirror_(unsigned char _origin);
void contentupdate_(unsigned char _mind[], unsigned char _line);
unsigned char getlineofchar_(unsigned char _char, unsigned char _linenumber, unsigned char _bold);
unsigned long int unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(unsigned char _value, unsigned char _rightbitshiftvalue);

unsigned char _light_alphabet[52][8]={//0-25 uppercase / 26-51 lowercase
	{0x00, 0xfc, 0x12, 0x12, 0x12, 0xfc, 0x00, 0x00},//A
	{0x00, 0xfe, 0x92, 0x92, 0x92, 0x7c, 0x00, 0x00},//B
	{0x00, 0x7c, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00},//C
	{0x00, 0xfe, 0x82, 0x82, 0x82, 0x7c, 0x00, 0x00},//D
	{0x00, 0xfe, 0x92, 0x92, 0x92, 0x92, 0x00, 0x00},//E
	{0x00, 0xfe, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00},//F
	{0x00, 0x7c, 0x82, 0x82, 0x92, 0x72, 0x00, 0x00},//G
	{0x00, 0xfe, 0x10, 0x10, 0x10, 0xfe, 0x00, 0x00},//H
	{0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00},//I
	{0x00, 0x40, 0x80, 0x82, 0x82, 0x7e, 0x00, 0x00},//J
	{0x00, 0xfe, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00},//K
	{0x00, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00},//L
	{0x00, 0xfe, 0x08, 0x10, 0x08, 0xfe, 0x00, 0x00},//M
	{0x00, 0xfe, 0x08, 0x10, 0x20, 0xfe, 0x00, 0x00},//N
	{0x00, 0x7c, 0x82, 0x82, 0x82, 0x7c, 0x00, 0x00},//O
	{0x00, 0xfe, 0x12, 0x12, 0x12, 0x0c, 0x00, 0x00},//P
	{0x00, 0x7c, 0x82, 0x82, 0x42, 0xbc, 0x00, 0x00},//Q
	{0x00, 0xfe, 0x12, 0x12, 0x12, 0xec, 0x00, 0x00},//R
	{0x00, 0x8c, 0x92, 0x92, 0x92, 0x62, 0x00, 0x00},//S
	{0x00, 0x02, 0x02, 0xfe, 0x02, 0x02, 0x00, 0x00},//T
	{0x00, 0x7e, 0x80, 0x80, 0x80, 0x7e, 0x00, 0x00},//U
	{0x00, 0x0e, 0x30, 0xc0, 0x30, 0x0e, 0x00, 0x00},//V
	{0x00, 0xfe, 0x40, 0x38, 0x40, 0xfe, 0x00, 0x00},//W
	{0x00, 0xc6, 0x28, 0x10, 0x28, 0xc6, 0x00, 0x00},//X
	{0x00, 0x06, 0x08, 0xf0, 0x08, 0x06, 0x00, 0x00},//Y
	{0x00, 0xc2, 0xa2, 0x92, 0x8a, 0x86, 0x00, 0x00},//Z
	{0x00, 0x40, 0xa8, 0xa8, 0xa8, 0xf0, 0x00, 0x00},//a
	{0x00, 0xfe, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00},//b
	{0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00},//c
	{0x00, 0x70, 0x88, 0x88, 0x88, 0xfe, 0x00, 0x00},//d
	{0x00, 0x70, 0xa8, 0xa8, 0xa8, 0x30, 0x00, 0x00},//e
	{0x00, 0x88, 0x88, 0x7c, 0x0a, 0x0a, 0x00, 0x00},//f
	{0x00, 0x18, 0xa4, 0xa4, 0xa4, 0x78, 0x00, 0x00},//g
	{0x00, 0xfe, 0x08, 0x08, 0x08, 0xf0, 0x00, 0x00},//h
	{0x00, 0x00, 0x00, 0xfa, 0x00, 0x00, 0x00, 0x00},//i
	{0x00, 0x00, 0x80, 0x80, 0x7a, 0x00, 0x00, 0x00},//j
	{0x00, 0xfe, 0x20, 0x20, 0x50, 0x88, 0x00, 0x00},//k
	{0x00, 0x00, 0x00, 0x7e, 0x80, 0x00, 0x00, 0x00},//l
	{0x00, 0xf0, 0x08, 0xf0, 0x08, 0xf0, 0x00, 0x00},//m
	{0x00, 0xf0, 0x08, 0x08, 0x08, 0xf0, 0x00, 0x00},//n
	{0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00},//o
	{0x00, 0xfc, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00},//p
	{0x00, 0x18, 0x24, 0x24, 0x24, 0xfc, 0x00, 0x00},//q
	{0x00, 0xf0, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},//r
	{0x00, 0x90, 0xa8, 0xa8, 0xa8, 0x48, 0x00, 0x00},//s
	{0x00, 0x00, 0x08, 0x7e, 0x88, 0x80, 0x00, 0x00},//t
	{0x00, 0x78, 0x80, 0x80, 0x80, 0x78, 0x00, 0x00},//u
	{0x00, 0x18, 0x60, 0x80, 0x60, 0x18, 0x00, 0x00},//v
	{0x00, 0x78, 0x80, 0x78, 0x80, 0x78, 0x00, 0x00},//w
	{0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00},//x
	{0x00, 0x1c, 0xa0, 0xa0, 0xa0, 0x7c, 0x00, 0x00},//y
	{0x00, 0x88, 0xc8, 0xa8, 0x98, 0x88, 0x00, 0x00},//z
};
unsigned char _bold_alphabet[52][8]={//0-25 uppercase / 26-51 lowercase
	{0x00, 0xfc, 0xfe, 0x12, 0xfe, 0xfc, 0x00, 0x00},//A
	{0x00, 0xfe, 0xfe, 0x92, 0xfe, 0x6c, 0x00, 0x00},//B
	{0x00, 0x7c, 0xfe, 0x82, 0xc6, 0x44, 0x00, 0x00},//C
	{0x00, 0xfe, 0xfe, 0x82, 0xfe, 0x7c, 0x00, 0x00},//D
	{0x00, 0xfe, 0xfe, 0x92, 0x92, 0x82, 0x00, 0x00},//E
	{0x00, 0xfe, 0xfe, 0x12, 0x12, 0x02, 0x00, 0x00},//F
	{0x00, 0x7c, 0xfe, 0x82, 0xf6, 0xf4, 0x00, 0x00},//G
	{0x00, 0xfe, 0xfe, 0x10, 0xfe, 0xfe, 0x00, 0x00},//H
	{0x00, 0x82, 0xfe, 0xfe, 0x82, 0x00, 0x00, 0x00},//I
	{0x00, 0x40, 0xc0, 0x82, 0xfe, 0x7e, 0x02, 0x00},//J
	{0x00, 0xfe, 0xfe, 0x38, 0xee, 0xc6, 0x00, 0x00},//K
	{0x00, 0xfe, 0xfe, 0x80, 0x80, 0x80, 0x00, 0x00},//L
	{0x00, 0xfe, 0xfe, 0x0c, 0x0c, 0xfe, 0xfe, 0x00},//M
	{0x00, 0xfe, 0xfe, 0x18, 0x60, 0xfe, 0xfe, 0x00},//N
	{0x00, 0x7c, 0xfe, 0x82, 0xfe, 0x7c, 0x00, 0x00},//O
	{0x00, 0xfe, 0xfe, 0x12, 0x1e, 0x0c, 0x00, 0x00},//P
	{0x00, 0x3e, 0x7f, 0x41, 0xff, 0xbe, 0x00, 0x00},//Q
	{0x00, 0xfe, 0xfe, 0x12, 0xfe, 0xec, 0x00, 0x00},//R
	{0x00, 0x8c, 0x9e, 0x92, 0xf2, 0x62, 0x00, 0x00},//S
	{0x00, 0x02, 0x02, 0xfe, 0xfe, 0x02, 0x02, 0x00},//T
	{0x00, 0x7e, 0xfe, 0x80, 0xfe, 0x7e, 0x00, 0x00},//U
	{0x00, 0x1e, 0x7e, 0xe0, 0xe0, 0x7e, 0x1e, 0x00},//V
	{0x00, 0xfe, 0xfe, 0x60, 0x60, 0xfe, 0xfe, 0x00},//W
	{0x00, 0xc6, 0xee, 0x38, 0x38, 0xee, 0xc6, 0x00},//X
	{0x00, 0xc6, 0xee, 0x38, 0x18, 0x0e, 0x06, 0x00},//Y
	{0x00, 0xc2, 0xe2, 0xb2, 0x9e, 0x8e, 0x00, 0x00},//Z
	{0x00, 0x40, 0xe8, 0xa8, 0xf8, 0xf0, 0x00, 0x00},//a
	{0x00, 0xfe, 0xfe, 0x88, 0xf8, 0x70, 0x00, 0x00},//b
	{0x00, 0x70, 0xf8, 0x88, 0xd8, 0x50, 0x00, 0x00},//c
	{0x00, 0x70, 0xf8, 0x88, 0xfe, 0xfe, 0x00, 0x00},//d
	{0x00, 0x70, 0xf8, 0xa8, 0xb8, 0x30, 0x00, 0x00},//e
	{0x00, 0x10, 0xfc, 0xfe, 0x12, 0x00, 0x00, 0x00},//f
	{0x00, 0x9c, 0xbe, 0xa2, 0xfe, 0x7e, 0x00, 0x00},//g
	{0x00, 0xfe, 0xfe, 0x08, 0xf8, 0xf8, 0x00, 0x00},//h
	{0x00, 0x88, 0xfa, 0xfa, 0x80, 0x00, 0x00, 0x00},//i
	{0x00, 0x40, 0xc0, 0x88, 0xfa, 0x7a, 0x00, 0x00},//j
	{0x00, 0xfe, 0xfe, 0x70, 0xd8, 0x88, 0x00, 0x00},//k
	{0x00, 0x82, 0xfe, 0xfe, 0x80, 0x00, 0x00, 0x00},//l
	{0xf8, 0xf8, 0x08, 0xf0, 0x08, 0xf8, 0xf0, 0x00},//m
	{0x00, 0xf8, 0xf8, 0x08, 0xf8, 0xf0, 0x00, 0x00},//n
	{0x00, 0x70, 0xf8, 0x88, 0xf8, 0x70, 0x00, 0x00},//o
	{0x00, 0xfe, 0xfe, 0x22, 0x3e, 0x1c, 0x00, 0x00},//p
	{0x00, 0x1c, 0x3e, 0x22, 0xfe, 0xfe, 0x00, 0x00},//q
	{0x00, 0xf8, 0xf8, 0x18, 0x08, 0x08, 0x00, 0x00},//r
	{0x00, 0x90, 0xb8, 0xa8, 0xe8, 0x48, 0x00, 0x00},//s
	{0x00, 0x10, 0x7e, 0xfe, 0x90, 0x00, 0x00, 0x00},//t
	{0x00, 0x78, 0xf8, 0x80, 0xf8, 0xf8, 0x00, 0x00},//u
	{0x00, 0x38, 0x78, 0xc0, 0xc0, 0x78, 0x38, 0x00},//v
	{0x00, 0xf8, 0xf8, 0x60, 0x60, 0xf8, 0xf8, 0x00},//w
	{0x00, 0x88, 0xd8, 0x70, 0x70, 0xd8, 0x88, 0x00},//x
	{0x00, 0x9e, 0xbe, 0xa0, 0xfe, 0x7e, 0x00, 0x00},//y
	{0x00, 0xc8, 0xe8, 0xb8, 0x98, 0x00, 0x00, 0x00},//z
};
unsigned char _light_number[10][8]={
	{0x00, 0x7c, 0xa2, 0x92, 0x8a, 0x7c, 0x00, 0x00},//0
	{0x00, 0x00, 0x04, 0xfe, 0x00, 0x00, 0x00, 0x00},//1
	{0x00, 0x82, 0xc2, 0xa2, 0x92, 0x8c, 0x00, 0x00},//2
	{0x00, 0x92, 0x92, 0x92, 0x92, 0x6c, 0x00, 0x00},//3
	{0x00, 0x1e, 0x10, 0x10, 0x10, 0xfe, 0x00, 0x00},//4
	{0x00, 0x9e, 0x92, 0x92, 0x92, 0x62, 0x00, 0x00},//5
	{0x00, 0x7c, 0x92, 0x92, 0x92, 0x60, 0x00, 0x00},//6
	{0x00, 0x02, 0x02, 0xc2, 0x32, 0x0e, 0x00, 0x00},//7
	{0x00, 0x6c, 0x92, 0x92, 0x92, 0x6c, 0x00, 0x00},//8
	{0x00, 0x0c, 0x92, 0x92, 0x92, 0x7c, 0x00, 0x00},//9
};
unsigned char _bold_number[10][8]={
	{0x00, 0x7c, 0x7c, 0x82, 0x7c, 0x7c, 0x00, 0x00},//0
	{0x00, 0x84, 0xfe, 0xfe, 0x80, 0x00, 0x00, 0x00},//1
	{0x00, 0xc4, 0xe6, 0xb2, 0x9e, 0x8c, 0x00, 0x00},//2
	{0x00, 0x44, 0xc6, 0x92, 0xfe, 0x6c, 0x00, 0x00},//3
	{0x00, 0x18, 0x1c, 0x16, 0xfe, 0xfe, 0x00, 0x00},//4
	{0x00, 0x4e, 0xce, 0x8a, 0xfa, 0x72, 0x00, 0x00},//5
	{0x00, 0x7c, 0xfe, 0x8a, 0xfa, 0x70, 0x00, 0x00},//6
	{0x00, 0xc2, 0xe2, 0x32, 0x1e, 0x0e, 0x00, 0x00},//7
	{0x00, 0x6c, 0xfe, 0x92, 0xfe, 0x6c, 0x00, 0x00},//8
	{0x00, 0x6c, 0xfe, 0x92, 0xfe, 0x6c, 0x00, 0x00},//9
};

unsigned long long int _dotmatrix_screen[8];
unsigned char _dotmatrix_update_phase=0;
unsigned char _dotmatrix_brightness=0;
unsigned char _animation_shift=0;

void display_setup_(void){
	for(unsigned char _rotate=0;_rotate<52;_rotate++){
		cwrotate_(_light_alphabet[_rotate]);
		cwrotate_(_bold_alphabet[_rotate]);
		for(unsigned char _mirror=0;_mirror<8;_mirror++){
			_light_alphabet[_rotate][_mirror]=mirror_(_light_alphabet[_rotate][_mirror]);
			_bold_alphabet[_rotate][_mirror]=mirror_(_bold_alphabet[_rotate][_mirror]);
		}
	}
	for(unsigned char _rotate=0;_rotate<10;_rotate++){
		cwrotate_(_light_number[_rotate]);
		cwrotate_(_bold_number[_rotate]);
		for(unsigned char _mirror=0;_mirror<8;_mirror++){
			_light_number[_rotate][_mirror]=mirror_(_light_number[_rotate][_mirror]);
			_bold_number[_rotate][_mirror]=mirror_(_bold_number[_rotate][_mirror]);
		}
	}
	DDRB=DDRB|(TRUE<<CS)|(TRUE<<DIN)|(TRUE<<CLK);
	SPCR=(TRUE<<SPE)|(TRUE<<MSTR)|(TRUE<<SPR0);
	unsigned char _max7219_startup_sequence[6][2]={
		{MAX7219_NOP, 0x00},
		{MAX7219_TEST, 0x00},
		{MAX7219_SCANLIM, 0x07},
		{MAX7219_DECODE, 0x00},
		{MAX7219_INTEN, 0x00},
		{MAX7219_SHUTDOWN, 0x01},
	};
	for(unsigned char _phase=0;_phase<6;_phase++){
		portbcontroler_(CS,FALSE);
		for(unsigned char _module=0;_module<4;_module++){
			display_commander_(_max7219_startup_sequence[_phase][0], _max7219_startup_sequence[_phase][1]);
		}
		portbcontroler_(CS,TRUE);
	}
	for(unsigned char _eraser=0;_eraser<8;_eraser++){
		_dotmatrix_screen[_eraser]=0;
	}
}
void display_update_(void){
	contentupdate_(_mind, _dotmatrix_update_phase-1);
	portbcontroler_(CS,FALSE);
	for(unsigned char _digit=0;_digit<4;_digit++){
		if(_dotmatrix_update_phase==_dimming){
			display_commander_(MAX7219_INTEN, _dotmatrix_brightness);
		}
		else{
			display_commander_(_dotmatrix_update_phase, mirror_((unsigned char)(_dotmatrix_screen[_dotmatrix_update_phase-1]>>(8*_digit))));
		}
	}
	portbcontroler_(CS,TRUE);
	_dotmatrix_update_phase=(_dotmatrix_update_phase+1)%9;
	
}
void spitrans_(unsigned char _data){
	SPDR=_data;
	while(!(SPSR&(TRUE<<SPIF)));
}
void display_commander_(unsigned char _adress, unsigned char _value){
	spitrans_(_adress);
	spitrans_(_value);
}

void cwrotate_(unsigned char* __origin){
	unsigned char _drawboard[8]={0,};
	for(unsigned char _y=0;_y<8;_y++){
		for(unsigned char _x=0;_x<8;_x++){
			_drawboard[_y]=_drawboard[_y]|(getbitofbyte_(__origin[_x],(7-_y))<<_x);
		}
	}
	for(unsigned char _duplicate=0;_duplicate<8;_duplicate++){
		__origin[_duplicate]=_drawboard[_duplicate];
	}
}
void ccwrotate_(unsigned char* __origin){
	unsigned char _drawboard[8]={0,};
	for(unsigned char _y=0;_y<8;_y++){
		for(unsigned char _x=0;_x<8;_x++){
			_drawboard[_y]=_drawboard[_y]|(getbitofbyte_(__origin[7-_x], _y)<<_x);
		}
	}
	for(unsigned char _duplicate=0;_duplicate<8;_duplicate++){
		__origin[_duplicate]=_drawboard[_duplicate];
	}
}
unsigned char mirror_(unsigned char _origin){
	unsigned char _clipboard=_origin;
	_origin=0;
	for(unsigned char _bit=0;_bit<8;_bit++){
		_origin=_origin|(((_clipboard&(TRUE<<_bit))>>_bit)<<(7-_bit));
	}
	return _origin;
}
void contentupdate_(unsigned char _mind[], unsigned char _line){
	_dotmatrix_screen[_line]=0;
	if(_tutorial_stage == _joystick_tutorial){
		for(unsigned char _wordx=0;_wordx<strlen((char*)_mode_list[_tutorial_sentence_1]);_wordx++){
			_dotmatrix_screen[_line]=_dotmatrix_screen[_line] | unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(getlineofchar_(_mode_list[_tutorial_sentence_1][_wordx],_line, TRUE), (CHARSPACE*_wordx)-(_dotmatrix_animation_move));
		}
	}
	else if(_tutorial_stage == _button_tutorial){
		for(unsigned char _wordx=0;_wordx<strlen((char*)_mode_list[_tutorial_sentence_2]);_wordx++){
			_dotmatrix_screen[_line]=_dotmatrix_screen[_line] | unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(getlineofchar_(_mode_list[_tutorial_sentence_2][_wordx],_line, TRUE), (CHARSPACE*_wordx)-(_dotmatrix_animation_move));
		}
	}
	else if(strlen((char*)_mode_list[_mode])<=4){
		for(unsigned char _wordx=0;_wordx<4;_wordx++){
			_dotmatrix_screen[_line]=_dotmatrix_screen[_line] | unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(getlineofchar_(_mode_list[_mode][_wordx],_line, TRUE), (CHARSPACE*_wordx));
		}
		_dotmatrix_animation_move=0;
	}
	else{
		for(unsigned char _wordx=0;_wordx<strlen((char*)_mode_list[_mode]);_wordx++){
			_dotmatrix_screen[_line]=_dotmatrix_screen[_line] | unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(getlineofchar_(_mode_list[_mode][_wordx],_line, TRUE), (CHARSPACE*_wordx)-(_dotmatrix_animation_move));
		}
	}
	_dotmatrix_screen[_line]=(_dotmatrix_screen[_line]&0xffffff00)|_mind[_line];
	/*
	for(unsigned char _wordx=0;_wordx<3;_wordx++){
	for(unsigned char _wordy=0;_wordy<8;_wordy++){
	_dotmatrix_screen[_wordy]=_dotmatrix_screen[_wordy]|(((unsigned long long int)_testalpha[_wordx][_wordy])<<(8*(3-_wordx)));
	}
	}
	*/
}
unsigned char getlineofchar_(unsigned char _char, unsigned char _linenumber, unsigned char _bold){
	if(_char>='A' && _char<='Z'){//upper case
		_char = _char - 'A';
		if(_bold){
			return _bold_alphabet[_char][_linenumber];
		}
		else{
			return _light_alphabet[_char][_linenumber];
		}
	}
	else if(_char>='a' && _char<='z'){//lower case
		_char = _char - 71;
		if(_bold){
			return _bold_alphabet[_char][_linenumber];
		}
		else{
			return _light_alphabet[_char][_linenumber];
		}
	}
	else if(_char>='0' && _char<='9'){//number
		_char = _char - '0';
		if(_bold){
			return _bold_number[_char][_linenumber];
		}
		else{
			return _light_number[_char][_linenumber];
		}
	}
	else{
		return FALSE;
	}
}
unsigned long int unsignedlongintbitshiftleftadjustcreatorviaunsignedchar_(unsigned char _value, unsigned char _rightbitshiftvalue){
	if(_value==0){
		return FALSE;
	}
	unsigned long int _simpleunsignedlongint = _value;
	_simpleunsignedlongint=_simpleunsignedlongint<<(24-_rightbitshiftvalue);
	return _simpleunsignedlongint;
}