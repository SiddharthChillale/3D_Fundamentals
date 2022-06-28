
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	cube(1.0f)
{
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
}
#include "Mat3.h"
void Game::ComposeFrame()
{
	auto lines = cube.GetLines();

	for (auto& v : lines.vertices) {
		nst.Transform(v);
	}

	for (auto i = lines.indices.cbegin(), end = lines.indices.cend(); i != end; std::advance(i, 2)) {
		gfx.DrawLine(lines.vertices[*i], lines.vertices[ *next(i)], Colors::White);
	}
}