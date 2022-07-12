#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class WaveVertexTextureEffect {
public:
	class Vertex {
	public:
		Vertex() = default;
		Vertex(const Vec3& pos)
			:
			pos(pos){}
		Vertex(const Vec3& pos, const Vertex& src)
			:
			pos(pos),
			t(src.t){}
		Vertex(const Vec3& pos, const Vec2& t)
			:
			pos(pos),
			t(t){}
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}

	public:
		Vec3 pos;
		Vec2 t;
	};

	class PixelShader {
	public:
		template<class Input> 
		Color operator()(const Input& in)const {
			return pTex->GetPixel(
				(unsigned int)std::min(in.t.x * tex_width + 0.5f, tex_xclamp),
				(unsigned int)std::min(in.t.y * tex_height + 0.5f, tex_yclamp)

			);
		}

		void BindTexture(const std::wstring& filename) {
			pTex = std::make_unique<Surface>(Surface::FromFile(filename));
			tex_width = float(pTex->GetWidth());
			tex_height = float(pTex->GetHeight());
			tex_xclamp = float(tex_width  - 1.0f);
			tex_yclamp = float(tex_height - 1.0f);

		}
	private:
		std::unique_ptr<Surface> pTex;
		float tex_width;
		float tex_height;
		float tex_xclamp;
		float tex_yclamp;

	};

	class VertexShader {
	public:
		typedef Vertex Output;

	public:

		void SetTime(float t) {
			time = t;
		}

		void BindRotation(const Mat3& rotation_in) {
			rotation = rotation_in;
		}

		void BindTranslation(const Vec3& translation_in) {
			translation = translation_in;
		}

		Output operator()(const Vertex& v_in)const {
			Vec3 pos = v_in.pos * rotation + translation;
			
			pos.y += amplitude * sin(time * freqScroll + pos.x * freqWave);
			return { pos, v_in.t };
		}

	private:
		Mat3 rotation;
		Vec3 translation;
		float time = 0.0f;
		float freqWave = 10.0f;
		float freqScroll = 5.0f;
		float amplitude = 0.05f;
	};

	typedef DefaultGeometryShader<Vertex> GeometryShader;

public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;

};