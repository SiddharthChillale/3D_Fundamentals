#pragma once 
#include "Colors.h"
#include "Vec4.h"

struct DefaultPhongDiffuseParams {
	static constexpr float linear_atten = 1.0f;
	static constexpr float quadratic_atten = 2.619f;
	static constexpr float constant_atten = 0.382f;
};

struct DefaultSpecularParams {
	static constexpr float specular_power = 30.0f;
	static constexpr float specular_intensity = 0.6f;
};

template<class PointDiffuse = DefaultPhongDiffuseParams, class Specular = DefaultSpecularParams>
class BasePhongShader {
public:
	template<class Input>
	Color Shade(const Input& in, const Vec4& material_color)const {
		const auto surf_norm = in.n.GetNormalized();
		const auto v_to_l = light_pos - in.worldPos;
		const auto dist = v_to_l.Len();

		const auto dir = v_to_l / dist;

		const auto attenuation = 1.0f / (DefaultPhongDiffuseParams::constant_atten + 
										DefaultPhongDiffuseParams::linear_atten* dist +
										DefaultPhongDiffuseParams::quadratic_atten * sq(dist));
		const auto d = light_diffuse * attenuation * std::max(0.0f, surf_norm * dir);

		const auto w = surf_norm * (v_to_l * surf_norm);
		const auto r = w * 2.0f - v_to_l;

		const auto s = light_diffuse * Specular::specular_intensity * std::pow(std::max(0.0f, -r.GetNormalized() * in.worldPos.GetNormalized()), Specular::specular_power);

		return Color(material_color.GetHadamard(d + light_ambient + s).Saturate() * 255.0f);
	}

	void SetDiffuseLight(const Vec3& light) {
		light_diffuse = light;
	}
	void SetAmbientLight(const Vec3& light) {
		light_ambient = light;
	}
	void SetLightPosition(const Vec3& pos) {
		light_pos = pos;
	}
private:
	Vec3 light_pos = { 0.0f, 0.0f, 0.5f };
	Vec3 light_diffuse = { 1.0f, 1.0f, 1.0f };
	Vec3 light_ambient = { 0.1f, 0.1f, 0.1f };
};