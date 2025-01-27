#pragma once
#include "Scene.h"
#include "Cube.h"
#include "Matrix.h"
#include "Pipeline.h"
#include "SolidEffect.h"

class CubeSolidScene : public Scene {
public:
	typedef Pipeline<SolidEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	CubeSolidScene(Graphics& gfx)
		:
		itList(Cube::GetPlainIndependentFaces<Vertex>()),
		pipeline(gfx),
		Scene("Colored cube vertex gradient scene") {
		const Color colors[] = {
			Colors::Red,Colors::Green,Colors::Blue,Colors::Magenta,Colors::Yellow,Colors::Cyan
		};

		for (int i = 0; i < itList.vertices.size(); i++) {
			itList.vertices[i].color = colors[i / 4];
		}
	}

	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		if (kbd.KeyIsPressed('Q'))
		{
			theta_x = wrap_angle(theta_x + dTheta * dt);
		}
		if (kbd.KeyIsPressed('W'))
		{
			theta_y = wrap_angle(theta_y + dTheta * dt);
		}
		if (kbd.KeyIsPressed('E'))
		{
			theta_z = wrap_angle(theta_z + dTheta * dt);
		}
		if (kbd.KeyIsPressed('A'))
		{
			theta_x = wrap_angle(theta_x - dTheta * dt);
		}
		if (kbd.KeyIsPressed('S'))
		{
			theta_y = wrap_angle(theta_y - dTheta * dt);
		}
		if (kbd.KeyIsPressed('D'))
		{
			theta_z = wrap_angle(theta_z - dTheta * dt);
		}
		if (kbd.KeyIsPressed('R'))
		{
			offset_z += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('F'))
		{
			offset_z -= 2.0f * dt;
		}
	}

	virtual void Draw() override {

		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);
		const Vec3 translate = Vec3{ 0.0f, 0.0f, offset_z };

		pipeline.BeginFrame();
		pipeline.effect.vs.BindRotation(rot);
		pipeline.effect.vs.BindTranslation(translate);

		pipeline.Draw(itList);
	}



private:
	IndexTriangleList<Vertex> itList;
	Pipeline pipeline;
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;

};