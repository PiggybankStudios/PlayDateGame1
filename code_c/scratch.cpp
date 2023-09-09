/*
File:   scratch.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** None 
*/

void InitScratchArenas(MemArena_t* sourceArena, u64 perArenaSize, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(app);
	for (u64 aIndex = 0; aIndex < ArrayCount(app->scratchArenas); aIndex++)
	{
		if (perArenaSize > 0)
		{
			void* arenaMem = AllocMem(sourceArena, perArenaSize);
			NotNull(arenaMem);
			InitMemArena_MarkedStack(&app->scratchArenas[aIndex], perArenaSize, arenaMem, maxNumMarks, alignment);
		}
		else
		{
			ClearStruct(app->scratchArenas[aIndex]);
		}
	}
}

// Pass in 1 or 2 arenas that you DON'T want to get back from this function. i.e. you are going to use those arenas for memory that outlives your scratch memory
inline MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1, MemArena_t* avoidConflictWith2) //pre-declared in gy_scratch_arenas.h
{
	u64 scratchIndex = 0;
	while ((avoidConflictWith1 != nullptr && avoidConflictWith1 == &app->scratchArenas[scratchIndex]) ||
	       (avoidConflictWith2 != nullptr && avoidConflictWith2 == &app->scratchArenas[scratchIndex]))
	{
		scratchIndex++;
	}
	Assert(scratchIndex < ArrayCount(app->scratchArenas));
	MemArena_t* result = &app->scratchArenas[scratchIndex];
	if (!IsInitialized(result)) { return nullptr; }
	PushMemMark(result);
	return result;
}

inline void FreeScratchArena(MemArena_t* scratchArena) //pre-declared in gy_scratch_arenas.h
{
	NotNull(scratchArena);
	Assert(IsPntrInsideRange(scratchArena, &app->scratchArenas[0], sizeof(MemArena_t) * ArrayCount(app->scratchArenas)));
	if (scratchArena->size == 0) { return; }
	PopMemMark(scratchArena);
}
