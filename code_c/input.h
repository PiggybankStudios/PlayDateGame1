/*
File:   input.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _INPUT_H
#define _INPUT_H

enum Btn_t
{
	Btn_None = 0,
	Btn_A,
	Btn_B,
	Btn_Right,
	Btn_Down,
	Btn_Left,
	Btn_Up,
	Btn_NumButtons,
};
const char* GetBtnStr(Btn_t enumValue)
{
	switch (enumValue)
	{
		case Btn_None:  return "None";
		case Btn_A:     return "A";
		case Btn_B:     return "B";
		case Btn_Right: return "Right";
		case Btn_Down:  return "Down";
		case Btn_Left:  return "Left";
		case Btn_Up:    return "Up";
		default: return "Unknown";
	}
}

struct BtnState_t
{
	bool isDown;
	bool wasDown;
	bool wasPressed;
	bool wasReleased;
	u8 numTransitions;
	bool handled;
	bool handledExtended;
};
struct AppInput_t
{
	BtnState_t btnStates[Btn_NumButtons];
};

#endif //  _INPUT_H
