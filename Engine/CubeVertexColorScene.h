#pragma once
#include "Scene.h"
#include "Cube.h"
#include "Mat3.h"
#include "Pipeline.h"
#include "VertexColorEffect.h"

class CubeVertexColorScene : public Scene {
public:
	typedef Pipeline<VertexColorEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	CubeVertexColorScene(Graphics& gfx)
		:
		itList(Cube::GetPlain<Vertex>()),
		pipeline(gfx),
		Scene("Colored cube vertex gradient scene") {
		itList.vertices[0].color = Vec3(Colors::Red);
		itList.vertices[1].color = Vec3(Colors::Green);
		itList.vertices[2].color = Vec3(Colors::Blue);
		itList.vertices[3].color = Vec3(Colors::Yellow);
		itList.vertices[4].color = Vec3(Colors::Cyan);
		itList.vertices[5].color = Vec3(Colors::Magenta);
		itList.vertices[6].color = Vec3(Colors::White);
		itList.vertices[7].color = Vec3(Colors::Black);
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override{
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

	virtual void Draw()override {
		pipeline.BeginFrame();

		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);

		const Vec3 translate = { 0.0f, 0.0f, offset_z };
		

		pipeline.BindRotation(rot);
		pipeline.BindTranslation(translate);
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