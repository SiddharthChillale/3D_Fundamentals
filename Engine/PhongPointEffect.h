#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class PhongPointEffect {
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
		class Output{
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
			Vec3 pos;
			Vec3 n;
			Vec3 worldPos;
		};
	public:
		void BindRotation(const Mat3& rotation_in) {
			rotation = rotation_in;
		}
		void BindTranslation(const Vec3& translation_in) {
			translation = translation_in;
		}
		Output operator()(const Vertex& v)const {
			const auto pos = v.pos * rotation + translation;
			return { pos, v.n * rotation, pos };
		}

	public:
		Mat3 rotation;
		Vec3 translation;
	};

	class PixelShader {
	public:
		template<class Input>
		Color operator()(const Input& in)const {
			const auto v_to_l = light_pos - in.worldPos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;

			const auto attenuation = 1.0f / (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * sq(dist));
			const auto d = light_diffuse * attenuation * std::max(0.0f, in.n * dir);
			return Color(material_color.GetHadamard(d + light_ambient).Saturate() * 255.0f);
		}
		void SetDiffuseLight(const Vec3& c) {
			light_diffuse = c;
		}
		void SetAmbientLight(const Vec3& c) {
			light_ambient = c;
		}
		void SetLightPosition(const Vec3& pos_in) {
			light_pos = pos_in;
		}

	private:
		Vec3 light_pos = { 0.f, 0.f, 0.5f };
		Vec3 light_diffuse = { 1.f, 1.f, 1.f };
		Vec3 light_ambient = { 0.1f, 0.1f, 0.1f };
		Vec3 material_color = { 0.8f, 0.85f, 1.f };
		float linear_attenuation = 1.f;
		float quadratic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
	};

	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;


public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;
};