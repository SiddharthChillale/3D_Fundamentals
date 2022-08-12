#pragma once

#include "Scene.h"
#include "Matrix.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"
#include "MouseTracker.h"

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
		mt(),
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
			camera_pos += Vec4{0.0f, 1.0f, 0.0f,  0.0f} * inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('A'))
		{
			camera_pos += Vec4{ -1.0f, 0.0f, 0.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('S'))
		{
			camera_pos += Vec4{ 0.0f,-1.0f, 0.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('D'))
		{
			camera_pos += Vec4{ 1.0f, 0.0f, 0.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}


		if (kbd.KeyIsPressed('I'))
		{
			light_pos += Vec4{ 0.0f,  0.0f, 1.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('J'))
		{
			light_pos += Vec4{ -1.0f, 0.0f, 0.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('K'))
		{
			light_pos += Vec4{ 0.0f, 0.0f, -1.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('L'))
		{
			light_pos += Vec4{ 1.0f, 0.0f, 0.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		
		if (!mouse.IsEmpty()) {
			const auto e = mouse.Read();
			switch (e.GetType()) {
			case Mouse::Event::Type::LPress:
				mt.Engage(e.GetPos());
				break;
			case Mouse::Event::Type::LRelease:
				mt.Release();
				break;
			case Mouse::Event::Type::Move:
				if (mt.isEngaged()) {
					const auto delta = mt.move(e.GetPos());
					inv_camera_rot = inv_camera_rot * Mat4::RotationY((float)-delta.x * htrack)*Mat4::RotationX((float)-delta.y * vtrack);
				}
				break;

			case Mouse::Event::Type::WheelUp:
				camera_pos += Vec4{ 0.0f, 0.0f, 1.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed *3 * dt;
				break;

			case Mouse::Event::Type::WheelDown:
				camera_pos += Vec4{ 0.0f, 0.0f, -1.0f, 0.0f } *inv_camera_rot.Inverse() * camera_speed * 3*  dt;
				break;
			}
		}
		
	}

	virtual void Draw()override {
		pipeline.BeginFrame();
		
		const auto proj = Mat4::ProjectionHFOV(hfov, aspect_ratio, 0.5f, 10.0f);
		const auto view = Mat4::Translation(-camera_pos) * inv_camera_rot;
		
		const Mat4 rot = Mat4::RotationX(theta_x) * Mat4::RotationY(theta_y) * Mat4::RotationZ(theta_z) * Mat4::Translation(model_pos);

		pipeline.effect.vs.BindWorldView(rot * view);
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
	MouseTracker mt;
	static constexpr float aspect_ratio = 1.77777f;
	static constexpr float hfov = 100.0f;
	static constexpr float vfov = hfov / aspect_ratio;

	static constexpr float htrack = to_rad(hfov ) / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = to_rad(vfov ) / (float)Graphics::ScreenHeight;
	float camera_speed = 3.0f;

	Mat4 camera_rot = Mat4::Identity();
	Mat4 inv_camera_rot = Mat4::Identity();
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;

	Vec3 camera_pos = { 0.0f, 0.0f, 0.0f };
	Vec3 model_pos  = { 0.0f, 0.0f, 2.0f };
	Vec4 light_pos  = { 0.0f, 0.0f, 0.6f, 1.0f };
};