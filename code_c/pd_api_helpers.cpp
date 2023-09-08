/*
File:   pd_api_helpers.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds some functions that we want to add on top of the default pd_api.h
*/

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
