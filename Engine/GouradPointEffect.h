#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class GouradPointEffect {
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
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}
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
				color(src.color),
				pos(pos)
			{}
			Output(const Vec3& pos, const Vec3& color)
				:
				color(color),
				pos(pos)
			{}
			Output& operator+=(const Output& rhs)
			{
				pos += rhs.pos;
				color += rhs.color;
				return *this;
			}
			Output operator+(const Output& rhs) const
			{
				return Output(*this) += rhs;
			}
			Output& operator-=(const Output& rhs)
			{
				pos -= rhs.pos;
				color -= rhs.color;
				return *this;
			}
			Output operator-(const Output& rhs) const
			{
				return Output(*this) -= rhs;
			}
			Output& operator*=(float rhs)
			{
				pos *= rhs;
				color *= rhs;
				return *this;
			}
			Output operator*(float rhs) const
			{
				return Output(*this) *= rhs;
			}
			Output& operator/=(float rhs)
			{
				pos /= rhs;
				color /= rhs;
				return *this;
			}
			Output operator/(float rhs) const
			{
				return Output(*this) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 color;
		};
	public:
		void BindRotation(const Mat3& in_rotation) {
			rotation = in_rotation;
		}
		void BindTranslation(const Vec3& in_translation) {
			translation = in_translation;
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
		Output operator()(const Vertex& v)const {
			const auto pos = v.pos * rotation + translation;

			const auto v_to_l = light_pos - pos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;

			const auto attenuation = 1.f / (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * sq(dist));
			const auto d = light_diffuse * attenuation * std::max(0.0f, (v.n * rotation) * dir);

			const auto c = material_color.GetHadamard(d + light_ambient).Saturate() * 255.0f;
			return { pos, c };
		}
	private:
		Mat3 rotation;
		Vec3 translation;
		Vec3 light_pos = { 0.f, 0.f, 0.5f };
		Vec3 light_diffuse = { 1.f, 1.f, 1.f };
		Vec3 light_ambient = { 0.1f, 0.1f, 0.1f };
		Vec3 material_color = { 0.8f, 0.85f, 1.0f };

		float linear_attenuation = 1.0f;
		float quadratic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
	};

	class PixelShader {
	public:
		template<class Input>
		Color operator()(const Input& in)const {
			return Color(in.color);
		}
	};

	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;

public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;
};