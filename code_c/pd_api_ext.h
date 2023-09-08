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
// |                           Drawing                            |
// +--------------------------------------------------------------+
void PdDrawText(MyStr_t text, v2i position)
{
	pd->graphics->drawText(text.chars, text.length, kUTF8Encoding, position.x, position.y);
}

#endif //  _PD_API_EXT_H
