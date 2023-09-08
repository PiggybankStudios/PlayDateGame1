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
void FpsToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(app->fpsDisplayMenuItem) != 0);
	if (app->fpsDisplayEnabled != newValue)
	{
		app->fpsDisplayEnabled = newValue;
		PrintLine_I("FPS Display %s", app->fpsDisplayEnabled ? "Enabled" : "Disabled");
	}
}
void DebugConsoleToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(app->debugConsoleMenuItem) != 0);
	if (app->debugConsoleEnabled != newValue)
	{
		app->debugConsoleEnabled = newValue;
		PrintLine_I("Debug Console %s", app->debugConsoleEnabled ? "Enabled" : "Disabled");
	}
}

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
void GameInitialize()
{
	const char* loadFontErrorStr;
	game->font = pd->graphics->loadFont(FONT_PATH, &loadFontErrorStr);
	if (game->font == NULL)
	{
		PrintLine_E("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, FONT_PATH, loadFontErrorStr);
		AssertMsg(false, "Couldn't load font!");
	}
	
	app->fpsDisplayMenuItem = pd->system->addCheckmarkMenuItem("FPS", 1, FpsToggledCallback, nullptr);
	NotNull(app->fpsDisplayMenuItem);
	pd->system->setMenuItemValue(app->fpsDisplayMenuItem, app->fpsDisplayEnabled ? 1 : 0);
	
	app->debugConsoleMenuItem = pd->system->addCheckmarkMenuItem("Debug", 1, DebugConsoleToggledCallback, nullptr);
	NotNull(app->debugConsoleMenuItem);
	pd->system->setMenuItemValue(app->debugConsoleMenuItem, app->debugConsoleEnabled ? 1 : 0);
	
	game->displayTextPos.x = (400 - TEXT_WIDTH) / 2;
	game->displayTextPos.y = (240 - TEXT_HEIGHT) / 2;
	game->displayTextVelocity.x = 1;
	game->displayTextVelocity.y = 2;
	
	game->initialized = true;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void GameUpdate()
{
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		game->isInverted = !game->isInverted;
	}
	if (BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		game->displayTextVelocity.x += SignOfI32(game->displayTextVelocity.x);
		game->displayTextVelocity.y += SignOfI32(game->displayTextVelocity.y);
	}
	
	if (!IsCrankDocked())
	{
		if (!game->followingCrank)
		{
			game->displayTextOldVelocity = game->displayTextVelocity;
			pd->system->setPeripheralsEnabled(kAccelerometer);
			game->followingCrank = true;
		}
		// game->displayTextVelocity = Vec2Roundi(Vec2FromAngle(input->crankAngleRadians + ThreeHalfsPi32, 3));
		game->displayTextVelocity.x += RoundR32i(input->accelVec.x);
		game->displayTextVelocity.y += RoundR32i(input->accelVec.y);
	}
	else if (game->followingCrank)
	{
		game->displayTextVelocity = game->displayTextOldVelocity;
		pd->system->setPeripheralsEnabled(kNone);
		game->followingCrank = false;
	}
	
	game->displayTextPos += game->displayTextVelocity;
	
	if (game->displayTextPos.x < 0 || game->displayTextPos.x > ScreenSize.width  - TEXT_WIDTH)  { game->displayTextVelocity.x = -game->displayTextVelocity.x; }
	if (game->displayTextPos.y < 0 || game->displayTextPos.y > ScreenSize.height - TEXT_HEIGHT) { game->displayTextVelocity.y = -game->displayTextVelocity.y; }
	
	// +==============================+
	// |            Render            |
	// +==============================+
	pd->graphics->clear(game->isInverted ? kColorBlack : kColorWhite);
	pd->graphics->setDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	pd->graphics->setFont(game->font);
	PdDrawText(NewStr(DISPLAY_TEXT), game->displayTextPos);
	
	if (app->fpsDisplayEnabled)
	{
		pd->system->drawFPS(0,0);
	}
}
