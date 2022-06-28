
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
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	static constexpr float dTheta = PI;
	NDCSpaceTransformer nst;
	Cube cube;
	/********************************/
};