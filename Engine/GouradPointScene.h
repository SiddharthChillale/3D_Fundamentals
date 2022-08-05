#pragma once

#include "Scene.h"
#include "GouradPointEffect.h"
#include "Mat3.h"
#include "Cube.h"
#include "Pipeline.h"
#include "SolidEffect.h"

class GouradPointScene : public Scene {
public:
	typedef ::Pipeline<GouradPointEffect> Pipeline;
	typedef ::Pipeline<SolidEffect> LightIndicatorPipeline;
	typedef Pipeline::Vertex Vertex;

public:
	GouradPointScene(Graphics& gfx, IndexTriangleList<Vertex> tl) 
		:
		itlist(std::move(tl)),
		pZb(std::make_shared<ZBuffer>(gfx.ScreenHeight, gfx.ScreenWidth)),
		pipeline(gfx, pZb),
		lPipeline(gfx, pZb),
		Scene("gourad shader scene free mesh")
	{
		itlist.AdjustToTrueCenter();
		offset_z = itlist.GetRadius() * 1.6f;
		for (auto& v : lightindicator.vertices) {
			v.color = Colors::White;
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
		if (kbd.KeyIsPressed('U'))
		{
			lpos_x += 0.2f * dt;
		}
		if (kbd.KeyIsPressed('I'))
		{
			lpos_y += 0.2f * dt;
		}
		if (kbd.KeyIsPressed('O'))
		{
			lpos_z += 0.2f * dt;
		}
		if (kbd.KeyIsPressed('J'))
		{
			lpos_x -= 0.2f * dt;
		}
		if (kbd.KeyIsPressed('K'))
		{
			lpos_y -= 0.2f * dt;
		}
		if (kbd.KeyIsPressed('L'))
		{
			lpos_z -= 0.2f * dt;
		}
		if (kbd.KeyIsPressed('R'))
		{
			offset_z += 0.2f * dt;
		}
		if (kbd.KeyIsPressed('F'))
		{
			offset_z -= 0.2f * dt;
		}
	}

	virtual void Draw() override {
		pipeline.BeginFrame();
		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);
		const Vec3 trans = { 0.0f, 0.0f, offset_z };
		pipeline.effect.vs.BindRotation(rot);
		pipeline.effect.vs.BindTranslation(trans);
		pipeline.effect.vs.SetLightPosition({ lpos_x, lpos_y, lpos_z });

		pipeline.Draw( itlist );

		lPipeline.effect.vs.BindTranslation({ lpos_x, lpos_y, lpos_z });
		lPipeline.effect.vs.BindRotation(Mat3::Identity());
		lPipeline.Draw(lightindicator);
	}
private:
	IndexTriangleList<Vertex> itlist;
	IndexTriangleList<SolidEffect::Vertex> lightindicator = Sphere::GetPlain<SolidEffect::Vertex>(0.05f);
	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LightIndicatorPipeline lPipeline;
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float lpos_x = 0.0f;
	float lpos_y = 0.0f;
	float lpos_z = 0.6f;
	
};