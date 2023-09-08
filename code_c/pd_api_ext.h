/*
File:   pd_api_ext.h
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds some functions that we want to add on top of the default pd_api.h
*/

#ifndef _PD_API_EXT_H
#define _PD_API_EXT_H

// +--------------------------------------------------------------+
// |                         Enum Strings                         |
// +--------------------------------------------------------------+
const char* GetPDSystemEventStr(PDSystemEvent event)
{
	switch (event)
	{
		case kEventInit:        return "kEventInit";
		case kEventInitLua:     return "kEventInitLua";
		case kEventLock:        return "kEventLock";
		case kEventUnlock:      return "kEventUnlock";
		case kEventPause:       return "kEventPause";
		case kEventResume:      return "kEventResume";
		case kEventTerminate:   return "kEventTerminate";
		case kEventKeyPressed:  return "kEventKeyPressed";
		case kEventKeyReleased: return "kEventKeyReleased";
		case kEventLowPower:    return "kEventLowPower";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                             Font                             |
// +--------------------------------------------------------------+
v2i MeasureText(LCDFont* font, MyStr_t text, i32 tracking = 0)
{
	v2i result;
	result.width  = pd->graphics->getTextWidth(font, text.chars, text.length, kUTF8Encoding, tracking);
	result.height = pd->graphics->getFontHeight(font);
	return result;
}

// +--------------------------------------------------------------+
// |                           Drawing                            |
// +--------------------------------------------------------------+
void PdDrawText(MyStr_t text, v2i position)
{
	pd->graphics->drawText(text.chars, text.length, kUTF8Encoding, position.x, position.y);
}

void PdDrawTexturedRec(Texture_t texture, reci drawRec)
{
	pd->graphics->drawRotatedBitmap(
		texture.bitmap,
		drawRec.x, drawRec.y,
		0.0f, //rotation
		0.0f, 0.0f, //centerx/y
		(r32)drawRec.width / (r32)texture.width, //scalex
		(r32)drawRec.height / (r32)texture.height //scaley
	);
}

#endif //  _PD_API_EXT_H
