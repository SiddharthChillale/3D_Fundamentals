#pragma once 

#include "Scene.h"
#include "Plane.h"
#include "Mat3.h"
#include "WaveVertexTextureEffect.h"

class VertexWaveScene : public Scene {
public:
	typedef Pipeline<WaveVertexTextureEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	VertexWaveScene(Graphics& gfx)
		:
		itList(Plane::GetSkinned<Vertex>(20)),
		pipeline(gfx),
		Scene("Text Plane Ripple nipple") {
		pipeline.effect.ps.BindTexture(L"Image\\sauron.png");
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
		time += dt;
	}

	virtual void Draw() override {
		pipeline.BeginFrame();
		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);

		const Vec3 translate = { 0.0f, 0.0f, offset_z };

		pipeline.effect.vs.BindRotation(rot);
		pipeline.effect.vs.BindTranslation(translate);

		pipeline.effect.vs.SetTime(time);
		// render triangles
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
	float time = 0.0f;

};