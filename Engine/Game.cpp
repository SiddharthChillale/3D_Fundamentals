
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
	const Color colors[12] = {
		Colors::White,
		Colors::Blue,
		Colors::Cyan,
		Colors::Gray,
		Colors::Green,
		Colors::Magenta,
		Colors::LightGray,
		Colors::Red,
		Colors::Yellow,
		Colors::White,
		Colors::Blue,
		Colors::Cyan
	};
	auto triangles = cube.GetTriangles();
	const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);

	for (auto& v : triangles.vertices) {
		v *= rot;
		v += {0.0f, 0.0f, offset_z};

		nst.Transform(v);
	}

	for (size_t i = 0, end = triangles.indices.size() / 3; i < end; i++) {
		
		gfx.DrawTriangle(triangles.vertices[triangles.indices[i * 3]],
						 triangles.vertices[triangles.indices[i * 3 + 1]],
						 triangles.vertices[triangles.indices[i * 3 + 2]] ,
						 colors[i]);
		
	}
}