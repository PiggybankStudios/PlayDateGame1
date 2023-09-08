/*
File:   input.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Handles taking the raw input from the PlayDate's input API and converting it
	** into a highly usable format for the game to query.
*/

void InitializeAppInput()
{
	NotNull(input);
	ClearPointer(input);
}

void UpdateAppInputBtnState(Btn_t btn, bool isDown)
{
	NotNull(input);
	BtnState_t* state = &input->btnStates[btn];
	if (state->isDown != isDown)
	{
		state->isDown = isDown;
		if (isDown) { state->wasPressed = true; }
		else { state->wasReleased = true; }
		state->numTransitions++;
		PrintLine_D("Btn_%s %s", GetBtnStr(btn), isDown ? "Pressed" : "Released");
	}
}

void UpdateAppInput()
{
	for (u64 bIndex = 0; bIndex < ArrayCount(input->btnStates); bIndex++)
	{
		BtnState_t* state = &input->btnStates[bIndex];
		state->wasDown = state->isDown;
		state->wasPressed = false;
		state->wasReleased = false;
		state->numTransitions = 0;
		state->handled = false;
	}
	
	PDButtons currentBtnBits = (PDButtons)0;
	PDButtons pressedBtnBits = (PDButtons)0;
	PDButtons releasedBtnBits = (PDButtons)0;
	pd->system->getButtonState(&currentBtnBits, &pressedBtnBits, &releasedBtnBits);
	
	UpdateAppInputBtnState(Btn_A,     IsFlagSet(currentBtnBits, kButtonA));
	UpdateAppInputBtnState(Btn_B,     IsFlagSet(currentBtnBits, kButtonB));
	UpdateAppInputBtnState(Btn_Right, IsFlagSet(currentBtnBits, kButtonRight));
	UpdateAppInputBtnState(Btn_Down,  IsFlagSet(currentBtnBits, kButtonDown));
	UpdateAppInputBtnState(Btn_Left,  IsFlagSet(currentBtnBits, kButtonLeft));
	UpdateAppInputBtnState(Btn_Up,    IsFlagSet(currentBtnBits, kButtonUp));
	
	for (u64 bIndex = 0; bIndex < ArrayCount(input->btnStates); bIndex++)
	{
		BtnState_t* state = &input->btnStates[bIndex];
		if (state->handledExtended)
		{
			if (state->isDown) { state->handled = true; }
			else
			{
				if (state->wasDown) { state->handled = true; }
				state->handledExtended = false;
			}
		}
	}
}

bool WasBtnHandled(Btn_t btn)
{
	return input->btnStates[btn].handled;
}
void HandleBtn(Btn_t btn)
{
	input->btnStates[btn].handled = true;
}
void HandleBtnExtended(Btn_t btn)
{
	input->btnStates[btn].handled = true;
	input->btnStates[btn].handledExtended = true;
}

bool BtnDownRaw(Btn_t btn)
{
	return input->btnStates[btn].isDown;
}
bool BtnPressedRaw(Btn_t btn)
{
	return input->btnStates[btn].wasPressed;
}
bool BtnReleasedRaw(Btn_t btn)
{
	return input->btnStates[btn].wasReleased;
}

bool BtnDown(Btn_t btn)
{
	return (!WasBtnHandled(btn) && BtnDownRaw(btn));
}
bool BtnPressed(Btn_t btn)
{
	return (!WasBtnHandled(btn) && BtnPressedRaw(btn));
}
bool BtnReleased(Btn_t btn)
{
	return (!WasBtnHandled(btn) && BtnReleasedRaw(btn));
}