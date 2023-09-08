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
	
	app->debugConsoleMenuItem = pd->system->addCheckmarkMenuItem("Debug Console", 1, DebugConsoleToggledCallback, nullptr);
	NotNull(app->debugConsoleMenuItem);
	pd->system->setMenuItemValue(app->debugConsoleMenuItem, app->debugConsoleEnabled ? 1 : 0);
	
	game->displayTextX = (400-TEXT_WIDTH)/2;
	game->displayTextY = (240-TEXT_HEIGHT)/2;
	game->displayTextDx = 1;
	game->displayTextDy = 2;
	
	game->initialized = true;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void GameUpdate()
{
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(game->font);
	pd->graphics->drawText(DISPLAY_TEXT, strlen(DISPLAY_TEXT), kASCIIEncoding, game->displayTextX, game->displayTextY);
	
	game->displayTextX += game->displayTextDx;
	game->displayTextY += game->displayTextDy;
	
	if (game->displayTextX < 0 || game->displayTextX > ScreenSize.width - TEXT_WIDTH) { game->displayTextDx = -game->displayTextDx; }
	if (game->displayTextY < 0 || game->displayTextY > ScreenSize.height - TEXT_HEIGHT) { game->displayTextDy = -game->displayTextDy; }
	
	pd->system->drawFPS(0,0);
}
