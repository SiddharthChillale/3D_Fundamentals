
#include "MainWindow.h"
#include "Game.h"
#include "Mat3.h"

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
	const float dt = 1.0f / 60.0f;

	if (wnd.kbd.KeyIsPressed('Q')) {
		wrap_angle(theta_x = theta_x + dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('W')) {
		wrap_angle(theta_y = theta_y + dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('E')) {
		wrap_angle(theta_z = theta_z + dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('A')) {
		wrap_angle(theta_x = theta_x - dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('S')) {
		wrap_angle(theta_y = theta_y - dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('D')) {
		wrap_angle(theta_z = theta_z - dTheta * dt);
	}
	if (wnd.kbd.KeyIsPressed('R')) {
		offset_z += 2.0f * dt;
	}
	if (wnd.kbd.KeyIsPressed('F')) {
		offset_z -= 2.0f * dt;
	}
}
#include "Mat3.h"
void Game::ComposeFrame()
{
	auto lines = cube.GetLines();
	const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);

	for (auto& v : lines.vertices) {
		v *= rot;
		v += {0.0f, 0.0f, offset_z};

		nst.Transform(v);
	}

	for (auto i = lines.indices.cbegin(), end = lines.indices.cend(); i != end; std::advance(i, 2)) {
		//auto t = (2.5f - lines.vertices[*i].z )/ 2.5f;
		gfx.DrawLine(lines.vertices[*i], lines.vertices[*next(i)], Colors::White);
		//gfx.DrawLine(lines.vertices[*i], lines.vertices[ *next(i)], Color(255u*t, 255u * t, 255u * t ));
	}
}