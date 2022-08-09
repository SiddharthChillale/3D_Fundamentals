#pragma once

#include "Scene.h"
#include "Matrix.h"
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
		model_pos.z = itlist.GetRadius() * 1.6f;
		for (auto& v : lightindicator.vertices) {
			v.color = Colors::White;
		}
	}
		
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		
		if (kbd.KeyIsPressed('W'))
		{
			camera_pos.z += camera_speed * dt;
		}
		if (kbd.KeyIsPressed('A'))
		{
			camera_pos.x -= camera_speed * dt;
		}
		if (kbd.KeyIsPressed('S'))
		{
			camera_pos.z -= camera_speed * dt;
		}
		if (kbd.KeyIsPressed('D'))
		{
			camera_pos.x += camera_speed * dt;
		}

		if (kbd.KeyIsPressed('I'))
		{
			light_pos.z += camera_speed * dt;
		}
		if (kbd.KeyIsPressed('J'))
		{
			light_pos.x -= camera_speed * dt;
		}
		if (kbd.KeyIsPressed('K'))
		{
			light_pos.z -= camera_speed * dt;
		}
		if (kbd.KeyIsPressed('L'))
		{
			light_pos.x += camera_speed * dt;
		}
		
	}

	virtual void Draw()override {
		pipeline.BeginFrame();
		
		const auto proj = Mat4::ProjectionHFOV(hfov, aspect_ratio, 0.5f, 10.0f);
		const auto view = Mat4::Translation(-camera_pos);
		
		const Mat4 rot = Mat4::RotationX(theta_x) * Mat4::RotationY(theta_y) * Mat4::RotationZ(theta_z) * Mat4::Translation(model_pos);

		pipeline.effect.vs.BindWorld(rot);
		pipeline.effect.vs.BindView(view);
		pipeline.effect.vs.BindProjection(proj);
		pipeline.effect.ps.SetLightPosition(light_pos * view );
		pipeline.Draw(itlist);

		lpipeline.effect.vs.BindWorldView(Mat4::Translation(light_pos) * view);
		lpipeline.effect.vs.BindProjection(proj);
		lpipeline.Draw(lightindicator);

	}

private:
	IndexTriangleList<Vertex> itlist;
	IndexTriangleList<SolidEffect::Vertex> lightindicator = Sphere::GetPlain<SolidEffect::Vertex>(0.05f);

	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LIPipeline lpipeline;
	
	static constexpr float aspect_ratio = 1.77777f;
	static constexpr float hfov = 100.0f;
	static constexpr float vfov = hfov / aspect_ratio;

	static constexpr float htrack = hfov / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = vfov / (float)Graphics::ScreenHeight;
	static constexpr float camera_speed = 1.0f;

	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;

	Vec3 camera_pos = { 0.0f, 0.0f, 0.0f };
	Vec3 model_pos  = { 0.0f, 0.0f, 2.0f };
	Vec4 light_pos  = { 0.0f, 0.0f, 0.6f, 1.0f };
};