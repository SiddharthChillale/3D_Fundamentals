
#include "MainWindow.h"
#include "Game.h"
#include "SolidCubeScene.h"
#include "CubeOrderScene.h"
#include "XMutualScene.h"
#include "TexCubeScene.h"


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )	
{
	scenes.push_back(std::make_unique<SolidCubeScene>());
	scenes.push_back(std::make_unique<CubeOrderScene>());
	scenes.push_back(std::make_unique<XMutualScene>());
	scenes.push_back(std::make_unique< TexCubeScene>());
	curScene = scenes.begin();
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = ft.Mark();
	while (!wnd.kbd.KeyIsEmpty())
	{
		const auto e = wnd.kbd.ReadKey();
		if (e.GetCode() == VK_TAB && e.IsPress())
		{
			if (wnd.kbd.KeyIsPressed(VK_SHIFT))
			{
				ReverseCycleScenes();
			}
			else
			{
				CycleScenes();
			}
		}
	}
	(*curScene)->Update(wnd.kbd, wnd.mouse, dt);
}

void Game::ComposeFrame()
{
	(*curScene)->Draw(gfx);
}

void Game::CycleScenes()
{
	if (++curScene == scenes.end())
	{
		curScene = scenes.begin();
	}
}

void Game::ReverseCycleScenes()
{
	if (curScene == scenes.begin())
	{
		curScene = scenes.end() - 1;
	}
	else
	{
		--curScene;
	}
	
}
