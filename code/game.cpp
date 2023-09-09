/*
File:   game.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds the main game logic (aka GameInitialize and GameUpdate)
*/

// +--------------------------------------------------------------+
// |                           Helpers                            |
// +--------------------------------------------------------------+
void BackgroundToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(game->backgroundMenuItem) != 0);
	if (game->backgroundEnabled != newValue)
	{
		game->backgroundEnabled = newValue;
		PrintLine_I("Background %s", game->backgroundEnabled ? "Enabled" : "Disabled");
	}
}

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
void GameInitialize()
{
	game->backgroundEnabled = true;
	game->backgroundMenuItem = pd->system->addCheckmarkMenuItem("Bg", 1, BackgroundToggledCallback, nullptr);
	NotNull(game->backgroundMenuItem);
	pd->system->setMenuItemValue(game->backgroundMenuItem, game->backgroundEnabled ? 1 : 0);
	
	game->mainFont = LoadFont(NewStr(MAIN_FONT_PATH), "mainFont");
	Assert(game->mainFont.isValid);
	game->smallFont = LoadFont(NewStr(SMALL_FONT_PATH), "smallFont");
	Assert(game->smallFont.isValid);
	game->debugFont = LoadFont(NewStr(DEBUG_FONT_PATH), "debugFont");
	Assert(game->debugFont.isValid);
	game->gameFont = LoadFont(NewStr(GAME_FONT_PATH), "gameFont");
	Assert(game->gameFont.isValid);
	
	game->testSheet = LoadSpriteSheet(NewStr("Resources/Sheets/test"), 5);
	Assert(game->testSheet.isValid);
	PrintLine_D("testSheet: (%d, %d) frames, each %dx%d", game->testSheet.numFramesX, game->testSheet.numFramesY, game->testSheet.frameSize.width, game->testSheet.frameSize.height);
	game->testSheetFrame = NewVec2i(game->testSheet.numFramesX-1, game->testSheet.numFramesY-1);
	
	game->pieSheet = LoadSpriteSheet(NewStr("Resources/Sheets/pie_badge_small"), 6);
	Assert(game->pieSheet.isValid);
	
	game->backgroundTexture = LoadTexture(NewStr("Resources/Textures/background"));
	Assert(game->backgroundTexture.isValid);
	
	game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
	Assert(game->pigTexture.isValid);
	
	game->pigPos.x = (ScreenSize.width - game->pigTexture.width) / 2.0f;
	game->pigPos.y = (ScreenSize.height - game->pigTexture.height) / 2.0f;
	game->pigVel.x = 1;
	game->pigVel.y = 2;
	
	game->initialized = true;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void GameUpdate()
{
	MemArena_t* scratch = GetScratchArena();
	
	MyStr_t pigEngineText = NewStr("Pig Engine");
	v2i pigEngineTextSize = MeasureText(game->mainFont.font, pigEngineText);
	v2i totalWidgetSize = NewVec2i(
		MaxI32(game->pigTexture.width, pigEngineTextSize.width),
		game->pigTexture.height + pigEngineTextSize.height
	);
	
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		game->isInverted = !game->isInverted;
		game->testSheetFrame.x++;
		if (game->testSheetFrame.x >= game->testSheet.numFramesX)
		{
			game->testSheetFrame.x = 0;
			game->testSheetFrame.y++;
			if (game->testSheetFrame.y >= game->testSheet.numFramesY)
			{
				game->testSheetFrame.y = 0;
			}
		}
	}
	if (BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		game->pigVel.x += SignOfR32(game->pigVel.x);
		game->pigVel.y += SignOfR32(game->pigVel.y);
	}
	
	if (!IsCrankDocked())
	{
		if (!game->followingCrank)
		{
			game->pigOldVel = game->pigVel;
			game->followingCrank = true;
		}
		game->pigVel = Vec2FromAngle(input->crankAngleRadians + ThreeHalfsPi32, 3);
	}
	else if (game->followingCrank)
	{
		game->pigVel = game->pigOldVel;
		game->followingCrank = false;
	}
	
	game->pigPos += game->pigVel * TimeScale;
	
	if (game->pigPos.x < 0)
	{
		game->pigPos.x = 0;
		game->pigVel.x = AbsR32(game->pigVel.x);
	}
	if (game->pigPos.x > ScreenSize.width - totalWidgetSize.width)
	{
		game->pigPos.x = (r32)(ScreenSize.width - totalWidgetSize.width);
		game->pigVel.x = -AbsR32(game->pigVel.x);
	}
	if (game->pigPos.y < 0)
	{
		game->pigPos.y = 0;
		game->pigVel.y = AbsR32(game->pigVel.y);
	}
	if (game->pigPos.y > ScreenSize.height - totalWidgetSize.height)
	{
		game->pigPos.y = (r32)(ScreenSize.height - totalWidgetSize.height);
		game->pigVel.y = -AbsR32(game->pigVel.y);
	}
	
	// +==============================+
	// |            Render            |
	// +==============================+
	pd->graphics->clear(game->isInverted ? kColorBlack : kColorWhite);
	PdSetDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	if (game->backgroundEnabled)
	{
		PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, ScreenSize));
	}
	
	{
		reci pigIconRec = NewReci(Vec2Roundi(game->pigPos), game->pigTexture.size);
		v2i pigEngineTextPos = pigIconRec.topLeft + NewVec2i(0, pigIconRec.height);
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdBindFont(&game->mainFont);
		PdDrawText(pigEngineText, pigEngineTextPos);
		pd->graphics->setDrawMode(oldDrawMode);
		PdDrawTexturedRec(game->pigTexture, pigIconRec);
	}
	
	v2i testFramePos = NewVec2i(
		ScreenSize.width/2 - game->pieSheet.frameSize.width/2,
		ScreenSize.height/2 - game->pieSheet.frameSize.height/2
	);
	PdDrawSheetFrame(game->testSheet, game->testSheetFrame, testFramePos);
	
	if (IsCrankUndocked())
	{
		r32 crankPercent = input->crankAngleRadians / TwoPi32;
		i32 pieFrameIndex = RoundR32i(24 * (1.0f - crankPercent));
		v2i pieFrame = NewVec2i(pieFrameIndex % game->pieSheet.numFramesX, pieFrameIndex / game->pieSheet.numFramesX);
		v2i pieSize = Vec2Roundi(ToVec2(game->pieSheet.frameSize) * LerpR32(1, 5, crankPercent));
		PdDrawSheetFrame(game->pieSheet, pieFrame, NewReci(ScreenSize.width - pieSize.width, 0, pieSize));
	}
	
	if (app->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (app->perfGraph.enabled) { textPos.y += app->perfGraph.mainRec.y + app->perfGraph.mainRec.height + 1; }
		PdBindFont(&game->debugFont);
		i32 stepY = game->debugFont.lineHeight + 1;
		
		// PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "ElapsedMs: %u (%u)", input->elapsedMsU32, input->realElapsedMsU32);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "TimeScale: %.2f (%.2f)", TimeScale, input->realTimeScale);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "TimeSinceEpoch: %llu", input->timeSinceEpoch);
		// textPos.y += stepY;
		
		PdDrawTextPrint(textPos, "main: %llu chars Height:%d %s", game->mainFont.numChars, game->mainFont.lineHeight, GetFontCapsStr(game->mainFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "small: %llu chars Height:%d %s", game->smallFont.numChars, game->smallFont.lineHeight, GetFontCapsStr(game->smallFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "debug: %llu chars Height:%d %s", game->debugFont.numChars, game->debugFont.lineHeight, GetFontCapsStr(game->debugFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "game: %llu chars Height:%d %s", game->gameFont.numChars, game->gameFont.lineHeight, GetFontCapsStr(game->gameFont));
		textPos.y += stepY;
		
		for (u8 rangeIndex = 0; rangeIndex < FontRange_NumRanges; rangeIndex++)
		{
			FontRange_t range = FontRangeByIndex(rangeIndex);
			if (FontHasRange(game->gameFont, range, true))
			{
				u8 numCharsInRange = GetNumCharsInFontRange(range);
				for (u8 charIndex = 0; charIndex < numCharsInRange; charIndex++)
				{
					u32 codepoint = GetFontRangeChar(range, charIndex);
					LCDFontPage* fontPage = pd->graphics->getFontPage(game->gameFont.font, codepoint);
					if (fontPage != nullptr)
					{
						LCDBitmap* glyphBitmap = nullptr;
						i32 glyphAdvance = 0;
						LCDFontGlyph* fontGlyph = pd->graphics->getPageGlyph(fontPage, codepoint, &glyphBitmap, &glyphAdvance);
						if (fontGlyph != nullptr && glyphBitmap != nullptr && glyphAdvance > 0)
						{
							v2i glyphBitmapSize = GetBitmapSize(glyphBitmap);
							if (textPos.x + glyphAdvance > ScreenSize.width)
							{
								textPos.x = 1;
								textPos.y += stepY;
							}
							PdDrawTexturedRec(glyphBitmap, glyphBitmapSize, NewReci(textPos, glyphBitmapSize));
							textPos.x += glyphAdvance;
						}
					}
				}
				textPos.x = 1;
				textPos.y += stepY;
			}
		}
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}
