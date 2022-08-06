#pragma once

#include "Scene.h"
#include "Mat3.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"

class SpecularPhongPointScene : public Scene {
public:
	typedef ::Pipeline<SpecularPhongPointEffect> Pipeline;
	typedef ::Pipeline<SolidEffect> LIPipeline;
	typedef Pipeline::Vertex Vertex;

public:
	//ctor
	SpecularPhongPointScene(Graphics& gfx, IndexTriangleList<Vertex> tl)
		:
		itlist(std::move(tl)),
		pZb(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		pipeline(gfx, pZb),
		lpipeline(gfx, pZb),
		Scene("specular phong shader") {
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

	virtual void Draw()override {
		pipeline.BeginFrame();

		const Mat3 rot = Mat3::RotationX(theta_x) * Mat3::RotationY(theta_y) * Mat3::RotationZ(theta_z);
		const Vec3 trans = { 0.0f, 0.0f, offset_z };

		pipeline.effect.vs.BindRotation(rot);
		pipeline.effect.vs.BindTranslation(trans);
		pipeline.effect.ps.SetLightPosition({ lpos_x, lpos_y, lpos_z });
		pipeline.Draw(itlist);

		lpipeline.effect.vs.BindRotation(Mat3::Identity());
		lpipeline.effect.vs.BindTranslation({ lpos_x, lpos_y, lpos_z });
		lpipeline.Draw(lightindicator);
	}

private:
	IndexTriangleList<Vertex> itlist;
	IndexTriangleList<SolidEffect::Vertex> lightindicator = Sphere::GetPlain<SolidEffect::Vertex>(0.05f);
	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LIPipeline lpipeline;
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;

	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;

	float lpos_x = 0.0f;
	float lpos_y = 0.0f;
	float lpos_z = 0.6f;
};