/*
File:   font.cpp
Author: Taylor Robbins
Date:   09\09\2023
Description: 
	** Functions for Font_t structure (which wraps LCDFont*)
*/

#if 0
bool IsSameCharBitmap(BitmapData_t char1, BitmapData_t char2)
{
	if (char1.size.width != char2.size.width) { return false; }
	if (char1.size.height != char2.size.height) { return false; }
	if (char1.rowWidth != char2.rowWidth) { return false; }
	if (char1.size.width * char1.size.height > 0)
	{
		if ((char1.data == nullptr) != (char2.data == nullptr)) { return false; }
		if ((char1.mask == nullptr) != (char2.mask == nullptr)) { return false; }
		if (char1.mask != nullptr && MyMemCompare(char1.mask, char2.mask, char1.rowWidth * char1.size.height) != 0) { return false; }
		if (char1.data != nullptr && MyMemCompare(char1.data, char2.data, char1.rowWidth * char1.size.height) != 0) { return false; }
	}
	return true;
}
#endif

Font_t LoadFont(MyStr_t path, const char* fontName)
{
	MemArena_t* scratch = GetScratchArena();
	const char* loadFontErrorStr = nullptr;
	Font_t result = {};
	
	MyStr_t pathNullTerm = AllocString(scratch, &path);
	result.font = pd->graphics->loadFont(pathNullTerm.chars, &loadFontErrorStr);
	if (result.font != nullptr)
	{
		result.lineHeight = pd->graphics->getFontHeight(result.font);
		
		result.numChars = 0;
		result.ranges = FontRange_None;
		result.partialRanges = FontRange_None;
		result.numRanges = 0;
		result.numPartialRanges = 0;
		for (u8 rangeIndex = 0; rangeIndex < FontRange_NumRanges; rangeIndex++)
		{
			FontRange_t range = FontRangeByIndex(rangeIndex);
			u8 numCharsInRange = GetNumCharsInFontRange(range);
			bool allCharsFound = true;
			bool anyCharsFound = false;
			for (u8 charIndex = 0; charIndex < numCharsInRange; charIndex++)
			{
				u32 codepoint = GetFontRangeChar(range, charIndex);
				if (codepoint == 0) { PrintLine_E("0 codepoint at %s[%u]", GetFontRangeStr(range), charIndex); }
				DebugAssert(codepoint != 0);
				LCDFontPage* fontPage = pd->graphics->getFontPage(result.font, codepoint);
				if (fontPage != nullptr)
				{
					LCDBitmap* glyphBitmap = nullptr;
					i32 glyphAdvance = 0;
					LCDFontGlyph* fontGlyph = pd->graphics->getPageGlyph(fontPage, codepoint, &glyphBitmap, &glyphAdvance);
					if (fontGlyph != nullptr)// && glyphBitmap != nullptr && glyphAdvance > 0)
					{
						result.numChars++;
						anyCharsFound = true;
					}
					else { allCharsFound = false; }
				}
				else { allCharsFound = false; }
			}
			
			if (allCharsFound) { result.ranges |= range; result.numRanges++; }
			if (anyCharsFound) { result.partialRanges |= range; result.numPartialRanges++; }
		}
		
		result.isValid = true;
	}
	else
	{
		PrintLine_E("Couldn't load font %s: %s", MAIN_FONT_PATH, loadFontErrorStr);
	}
	
	FreeScratchArena(scratch);
	return result;
}
