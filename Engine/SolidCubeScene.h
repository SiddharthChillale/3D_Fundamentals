#pragma once
#include "Scene.h"
#include "NDCSpaceTransformer.h"
#include "Cube.h"
#include "Mat3.h"

class SolidCubeScene : public Scene{

public:
	SolidCubeScene() = default;

	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt)override {

		if (kbd.KeyIsPressed('Q')) {
			wrap_angle(theta_x = theta_x + dTheta * dt);
		}
		if (kbd.KeyIsPressed('W')) {
			wrap_angle(theta_y = theta_y + dTheta * dt);
		}
		if (kbd.KeyIsPressed('E')) {
			wrap_angle(theta_z = theta_z + dTheta * dt);
		}
		if (kbd.KeyIsPressed('A')) {
			wrap_angle(theta_x = theta_x - dTheta * dt);
		}
		if (kbd.KeyIsPressed('S')) {
			wrap_angle(theta_y = theta_y - dTheta * dt);
		}
		if (kbd.KeyIsPressed('D')) {
			wrap_angle(theta_z = theta_z - dTheta * dt);
		}
		if (kbd.KeyIsPressed('R')) {
			offset_z += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('F')) {
			offset_z -= 2.0f * dt;
		}
	}
	virtual void Draw(Graphics& gfx) const override{
		
		auto triangles = cube.GetTriangles();
		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);

		// Apply world spce translation and rotations
		for (auto& v : triangles.vertices) {
			v *= rot;
			v += {0.0f, 0.0f, offset_z};
		}

		for (size_t i = 0, end = triangles.indices.size() / 3; i < end; i++) {
			const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]];
			const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]];
			const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]];

			triangles.cullFlags[i] = (v1 - v0).cross(v2 - v0) * v0 > 0.0f;
		}

		// Transform from world space to screen space
		for (auto& v : triangles.vertices) {
			nst.Transform(v);
		}

		for (size_t i = 0, end = triangles.indices.size() / 3; i < end; i++) {

			if (triangles.cullFlags[i] == false) {
				gfx.DrawTriangle(triangles.vertices[triangles.indices[i * 3]],
					triangles.vertices[triangles.indices[i * 3 + 1]],
					triangles.vertices[triangles.indices[i * 3 + 2]],
					colors[i]);
			}

		}
	}

private:
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	static constexpr float dTheta = PI;
	NDCSpaceTransformer nst;
	Cube cube = Cube(1.0f);

	static constexpr Color colors[12] = {
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
};