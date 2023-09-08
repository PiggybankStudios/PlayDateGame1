/*
File:   texture.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Functions for Texture_t structure (which wraps LCDBitmap*)
*/

Texture_t LoadTexture(MyStr_t path)
{
	Assert(IsStrNullTerminated(path)); //TODO: Allocate it somewhere if it's not!
	Texture_t result = {};
	
	const char* loadBitmapErrorStr = nullptr;
	result.bitmap = pd->graphics->loadBitmap(path.chars, &loadBitmapErrorStr);
	if (loadBitmapErrorStr == nullptr)
	{
		result.isValid = true;
	}
	else
	{
		PrintLine_E("Failed to load texture from \"%.*s\": %s", path.length, path.chars, loadBitmapErrorStr);
	}
	
	pd->graphics->getBitmapData(
		result.bitmap,
		&result.width,
		&result.height,
		nullptr, //rowbytes
		nullptr, //mask
		nullptr //data
	);
	
	return result;
}

