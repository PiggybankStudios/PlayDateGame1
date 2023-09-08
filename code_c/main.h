/*
File:   main.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _MAIN_H
#define _MAIN_H

//PlayDate has 16 MB of memory, we should be able to make 64x 256kB pages. Reserve one for FIXED_HEAP
#define FIXED_HEAP_SIZE         Kilobytes(256)
#define MAIN_HEAP_PAGE_SIZE     Kilobytes(256)
#define MAIN_HEAP_MAX_NUM_PAGES 63

struct AppState_t
{
	bool initialized;
	bool firstUpdateCalled;
	
	MemArena_t stdHeap;
	MemArena_t fixedHeap;
	MemArena_t mainHeap;
	
	PDMenuItem* debugConsoleMenuItem;
	bool debugConsoleEnabled;
	
	GameState_t* gameStatePntr;

};

#endif //  _MAIN_H
