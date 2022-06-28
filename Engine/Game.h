
#pragma once

#include "Graphics.h"
#include "NDCSpaceTransformer.h"
#include "Cube.h"

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	NDCSpaceTransformer nst;
	Cube cube;
	/********************************/
};