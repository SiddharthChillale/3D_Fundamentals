#pragma once

#include "Scene.h"
#include "Matrix.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"
#include "MouseTracker.h"
#include "RippleVertexSpecularPhongEffect.h"
#include "VertexLightTexturedEffect.h"
#include "Plane.h"

struct PointDiffuseParams {
	static constexpr float linear_attenuation = 0.9f;
	static constexpr float quadradic_attenuation = 0.6f;
	static constexpr float constant_attenuation = 0.682f;
};

struct PointSpecularParams {
	static constexpr float specular_power = 30.0f;
	static constexpr float specular_intensity = 0.6f;
};

class SpecularPhongPointScene : public Scene {
	using SpecularPhongPointEffect = SpecularPhongPointEffect<PointDiffuseParams, PointSpecularParams>;
	using VertexLightTexturedEffect = VertexLightTexturedEffect<PointDiffuseParams>;
	using RippleVertexSpecularPhongEffect = RippleVertexSpecularPhongEffect<PointDiffuseParams, PointSpecularParams>;

public:
	typedef ::Pipeline<SpecularPhongPointEffect> Pipeline;
	typedef ::Pipeline<VertexLightTexturedEffect> WallPipeline;
	typedef ::Pipeline<RippleVertexSpecularPhongEffect> RipplePipeline;
	typedef ::Pipeline<SolidEffect> LIPipeline;
	typedef Pipeline::Vertex Vertex;

public:
	struct Wall {
		const Surface* pTex;
		IndexTriangleList<VertexLightTexturedEffect::Vertex> model;
		Mat4 world;
	};
public:
	//ctor
	SpecularPhongPointScene(Graphics& gfx)
		:
		pZb(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		pipeline(gfx, pZb),
		lpipeline(gfx, pZb),
		wpipeline(gfx, pZb),
		rpipeline(gfx, pZb),
		mt(),
		Scene("Fullscale scene with multiple objects") {
		itlist.AdjustToTrueCenter();
		for (auto& v : lightindicator.vertices) {
			v.color = Colors::White;
		}

		walls.push_back({
			&tCeiling, 
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>(20, 20, width, width, tScaleCeiling), Mat4::RotationX(-PI / 2.0f) * Mat4::Translation(0.0f, height / 2.0f, 0.0f)
			});

		for (int i = 0; i < 4; i++) {
			walls.push_back({
				&tWall,
				Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>(20, 20, width, height, tScaleWall),
				Mat4::Translation(0.0f,0.0f,width / 2.0f)* Mat4::RotationY(float(i) * PI / 2.0f)
				});
		}

		walls.push_back({
			&tFloor,
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>(20,20,width,width,tScaleFloor),
			Mat4::RotationX(PI / 2.0) * Mat4::Translation(0.0f,-height / 2.0f,0.0f)
			});
		
	}
		
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		
		t += dt;

		if (kbd.KeyIsPressed('W'))
		{
			camera_pos += Vec4{ 0.0f,0.0f,1.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('A'))
		{
			camera_pos += Vec4{ -1.0f,0.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('S'))
		{
			camera_pos += Vec4{ 0.0f,0.0f,-1.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('D'))
		{
			camera_pos += Vec4{ 1.0f,0.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('C'))
		{
			camera_pos += Vec4{ 0.0f,1.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('Z'))
		{
			camera_pos += Vec4{ 0.0f,-1.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('Q'))
		{
			inv_camera_rot = inv_camera_rot * Mat4::RotationZ(cam_roll_speed * dt);
		}
		if (kbd.KeyIsPressed('E'))
		{
			inv_camera_rot = inv_camera_rot * Mat4::RotationZ(-cam_roll_speed * dt);
		}

		if (kbd.KeyIsPressed('I'))
		{
			light_pos += Vec4{ 0.0f,0.0f,1.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('J'))
		{
			light_pos += Vec4{ -1.0f,0.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('K'))
		{
			light_pos += Vec4{ 0.0f,0.0f,-1.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}
		if (kbd.KeyIsPressed('L'))
		{
			light_pos += Vec4{ 1.0f,0.0f,0.0f,0.0f } *inv_camera_rot.Inverse() * camera_speed * dt;
		}

		while (!mouse.IsEmpty())
		{
			const auto e = mouse.Read();
			switch (e.GetType())
			{
			case Mouse::Event::Type::LPress:
				mt.Engage(e.GetPos());
				break;
			case Mouse::Event::Type::LRelease:
				mt.Release();
				break;
			case Mouse::Event::Type::Move:
				if (mt.isEngaged())
				{
					const auto delta = mt.move(e.GetPos());
					inv_camera_rot = inv_camera_rot
						* Mat4::RotationY((float)-delta.x * htrack)
						* Mat4::RotationX((float)-delta.y * vtrack);
				}
				break;
			}
		}
		
		theta_y = wrap_angle(t * rotspeed);
		light_pos.y = l_height_amplitude * sin(wrap_angle((PI / (2.0f * l_height_amplitude)) * t));
		rpipeline.effect.vs.SetTime(t);
	}

	virtual void Draw()override {
		pipeline.BeginFrame();
		
		const auto proj = Mat4::ProjectionHFOV(hfov, aspect_ratio, 0.2f, 10.0f);
		const auto view = Mat4::Translation(-camera_pos) * inv_camera_rot;
		
		

		//render beautiful suzanne
		const Mat4 rot = Mat4::RotationX(theta_x) * Mat4::RotationY(theta_y) * Mat4::RotationZ(theta_z) * Mat4::Scaling(scale) * Mat4::Translation(model_pos);
		pipeline.effect.vs.BindWorldView(rot * view);
		pipeline.effect.vs.BindProjection(proj);
		pipeline.effect.ps.SetAmbientLight(light_ambient);
		pipeline.effect.ps.SetDiffuseLight(l);
		pipeline.effect.ps.SetLightPosition(light_pos * view );
		pipeline.Draw(itlist);

		//render light indicator
		lpipeline.effect.vs.BindWorldView(Mat4::Translation(light_pos) * view);
		lpipeline.effect.vs.BindProjection(proj);
		lpipeline.Draw(lightindicator);


		// render walls
		wpipeline.effect.vs.BindProjection(proj);
		wpipeline.effect.vs.SetAmbientLight(light_ambient);
		wpipeline.effect.vs.SetDiffuseLight(l);
		wpipeline.effect.vs.SetLightPosition(light_pos * view);
		
		
		for (const auto& wall : walls) {
			wpipeline.effect.vs.BindWorldView(wall.world * view);
			wpipeline.effect.ps.BindTexture(*wall.pTex);
			wpipeline.Draw(wall.model);
		}

		// render ripple plane
		rpipeline.effect.ps.BindTexture(tSauron);
		rpipeline.effect.ps.SetLightPosition(light_pos * view);
		rpipeline.effect.vs.BindWorldView(sauronWorld * view);
		rpipeline.effect.vs.BindProjection(proj);
		rpipeline.effect.ps.SetAmbientLight(light_ambient);
		rpipeline.effect.ps.SetDiffuseLight(l);
		rpipeline.Draw(sauron);
	}

private:
	float t = 0.0f;
	static constexpr float width = 4.0f;
	static constexpr float height = 1.75f;
	std::shared_ptr<ZBuffer> pZb;

	Pipeline pipeline;
	LIPipeline lpipeline;
	WallPipeline wpipeline;
	RipplePipeline rpipeline;
	MouseTracker mt;

	//camera model params
	static constexpr float aspect_ratio = 1.77777f;
	static constexpr float hfov = 85.0f;
	static constexpr float vfov = hfov / aspect_ratio;
	static constexpr float htrack = to_rad(hfov ) / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = to_rad(vfov ) / (float)Graphics::ScreenHeight;
	float camera_speed = 3.0f;
	Mat4 camera_rot = Mat4::Identity();
	Mat4 inv_camera_rot = Mat4::Identity();
	Vec3 camera_pos = { 0.0f, 0.0f, 0.0f };

	// suzanne model params
	IndexTriangleList<Vertex> itlist = IndexTriangleList<SpecularPhongPointScene::Vertex>::LoadNormals("Models\\suzanne.obj");
	Vec3 model_pos = { 1.2f, -0.4f, 1.2f };
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float rotspeed = PI / 4.0f;
	float scale = 0.4;
	static constexpr float cam_roll_speed = PI;

	// light params
	IndexTriangleList<SolidEffect::Vertex> lightindicator = Sphere::GetPlain<SolidEffect::Vertex>(0.05f);
	static constexpr float l_height_amplitude = 0.7f;
	static constexpr float l_height_period = 3.713f;
	Vec4 light_pos  = { 0.0f, 0.0f, 0.0f, 1.0f };
	Vec3 l = { 1.0f, 1.0f, 1.0f };
	Vec3 light_ambient = { 0.35f,0.35f,0.35f };
	
	// wall params
	static constexpr float tScaleCeiling = 0.5f;
	static constexpr float tScaleWall	 = 0.65f;
	static constexpr float tScaleFloor	 = 0.65f;
	Surface tCeiling = Surface::FromFile(L"Image//ceiling.png");
	Surface tWall = Surface::FromFile(L"Image//stonewall.png");
	Surface tFloor = Surface::FromFile(L"Image//floor.png");
	std::vector<Wall> walls;

	// ripple model params
	static constexpr float sauronSize = 0.6f;
	Mat4 sauronWorld = Mat4::RotationX(PI / 2.0f) * Mat4::Translation(0.3f, -0.8f, 0.0f);
	Surface tSauron = Surface::FromFile(L"Image//sauron.png");
	IndexTriangleList<RippleVertexSpecularPhongEffect::Vertex> sauron = Plane::GetSkinned<RippleVertexSpecularPhongEffect::Vertex>(50, 10, sauronSize, sauronSize, 0.6f);

};