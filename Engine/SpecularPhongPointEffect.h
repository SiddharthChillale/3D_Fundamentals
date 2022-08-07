#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class SpecularPhongPointEffect {
public:
	class Vertex {
	public:
		Vertex() = default;
		Vertex(const Vec3& pos)
			:
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vertex& src)
			:
			n(src.n),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vec3& n)
			:
			n(n),
			pos(pos)
		{}
	public:
		Vec3 pos;
		Vec3 n;
	};

	class VertexShader {
	public:
		class Output {
		public:
			Output() = default;
			Output(const Vec3& pos)
				:
				pos(pos)
			{}
			Output(const Vec3& pos, const Output& src)
				:
				n(src.n),
				worldPos(src.worldPos),
				pos(pos)
			{}
			Output(const Vec3& pos, const Vec3& n, const Vec3& worldPos)
				:
				n(n),
				pos(pos),
				worldPos(worldPos)
			{}
			Output& operator+=(const Output& rhs)
			{
				pos += rhs.pos;
				n += rhs.n;
				worldPos += rhs.worldPos;
				return *this;
			}
			Output operator+(const Output& rhs) const
			{
				return Output(*this) += rhs;
			}
			Output& operator-=(const Output& rhs)
			{
				pos -= rhs.pos;
				n -= rhs.n;
				worldPos -= rhs.worldPos;
				return *this;
			}
			Output operator-(const Output& rhs) const
			{
				return Output(*this) -= rhs;
			}
			Output& operator*=(float rhs)
			{
				pos *= rhs;
				n *= rhs;
				worldPos *= rhs;
				return *this;
			}
			Output operator*(float rhs) const
			{
				return Output(*this) *= rhs;
			}
			Output& operator/=(float rhs)
			{
				pos /= rhs;
				n /= rhs;
				worldPos /= rhs;
				return *this;
			}
			Output operator/(float rhs) const
			{
				return Output(*this) /= rhs;
			}
		public:
			Vec4 pos;
			Vec4 n;
			Vec3 worldPos;
		};
	public:
		void BindTransformation(const Mat4& transformation_in) {
			transformation = transformation_in;
		}
		
		Output operator()(const Vertex& v) const {
			const auto pos = Vec4(v.pos ) * transformation;
			return { pos, Vec4( v.n, 0.0f) * transformation , pos };
		}
		
	public:
		Mat4 transformation;
	};

	class PixelShader {
	public:

		template<class Input>
		Color operator()(const Input& in) const {
			const auto surf_norm = in.n.GetNormalized();

			const auto v_to_l = light_pos - in.worldPos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;

			const auto attenuation = 1.0f / (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * sq(dist));

			const auto d = light_diffuse * attenuation * std::max(0.0f, surf_norm * dir);
			const auto w = surf_norm * (v_to_l * surf_norm);
			const auto r = w * 2.0f - v_to_l;

			const auto s = light_diffuse * specular_intensity * std::pow(std::max(0.0f, -r.GetNormalized() * in.worldPos.GetNormalized()), specular_power);

			return Color(material_color.GetHadamard(d + light_ambient + s).Saturate() * 255.0f);
		}

		void SetDiffuseLight(const Vec3& c) {
			light_diffuse = c;
		}
		void SetAmbientLight(const Vec3& c) {
			light_ambient = c;
		}
		void SetLightPosition(const Vec3& pos) {
			light_pos = pos;
		}
	public:
		Vec3 light_pos = { 0.0f, 0.0f, 0.5f };
		Vec3 light_diffuse = { 1.0f, 1.0f, 1.0f };
		Vec3 light_ambient = { 0.1f, 0.1f, 0.1f };
		Vec3 material_color = { 0.8f, 0.85f, 1.0f };

		float linear_attenuation = 1.0f;
		float quadratic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;

		float specular_power = 30.f;
		float specular_intensity = 0.6f;
	};

	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;
};