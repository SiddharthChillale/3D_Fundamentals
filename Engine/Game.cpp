
#include "MainWindow.h"
#include "Game.h"
#include "Sphere.h"

#include "testtriangle.h"
//#include "SkinCubeScene.h"
//#include "CubeVertexColorScene.h"
//#include "CubeSolidScene.h"
//#include "DoubleCubeScene.h"
//#include "VertexWaveScene.h"
//#include "CubeSolidGeometryScene.h"
//#include "CubeFlatIndependentScene.h"
//#include "GeometryFlatScene.h"
//#include "GouraudScene.h"
//#include "GouradPointScene.h"
//#include "PhongPointScene.h"
#include "SpecularPhongPointScene.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )	
{
	scenes.push_back(std::make_unique<SpecularPhongPointScene>(gfx, IndexTriangleList<SpecularPhongPointScene::Vertex>::LoadNormals("Models\\suzanne.obj")));
	scenes.push_back(std::make_unique<SpecularPhongPointScene>(gfx, TestTriangle::GetNormals<SpecularPhongPointScene::Vertex>()));
	/*scenes.push_back(std::make_unique<PhongPointScene>(gfx, IndexTriangleList<PhongPointScene::Vertex>::LoadNormals("Models\\suzanne.obj")));
	scenes.push_back(std::make_unique<PhongPointScene>(gfx,  Plane::GetNormals<PhongPointScene::Vertex>(16)));
	scenes.push_back(std::make_unique<SpecularPhongPointScene>(gfx, Plane::GetNormals<SpecularPhongPointScene::Vertex>(16)));
	
	scenes.push_back(std::make_unique<GouraudScene>(gfx, Sphere::GetPlainNormals<GouraudScene::Vertex>()));
	
	scenes.push_back(std::make_unique<GeometryFlatScene>(gfx, IndexTriangleList<GeometryFlatScene::Vertex>::Load("Models\\suzanne.obj")));
	scenes.push_back(std::make_unique<CubeFlatIndependentScene>(gfx));
	scenes.push_back(std::make_unique<VertexWaveScene>(gfx));
	scenes.push_back(std::make_unique<SkinCubeScene>(gfx, L"image\\office_skin.jpg"));
	scenes.push_back(std::make_unique<CubeVertexColorScene>(gfx));*/
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
	// cycle through scenes when tab is pressed
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
		else if (e.GetCode() == VK_ESCAPE && e.IsPress())
		{
			wnd.Kill();
		}
	}
	// update scene
	(*curScene)->Update(wnd.kbd, wnd.mouse, dt);
}

void Game::ComposeFrame()
{
	(*curScene)->Draw();
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
