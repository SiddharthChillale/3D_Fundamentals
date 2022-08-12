#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"
#include "BaseVertexShader.h"
#include "BasePhongShader.h"

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
	class VSOutput {
	public:
		VSOutput() = default;
		VSOutput(const Vec4& pos)
			:
			pos(pos)
		{}
		VSOutput(const Vec4& pos, const VSOutput& src)
			:
			n(src.n),
			worldPos(src.worldPos),
			pos(pos)
		{}
		VSOutput(const Vec4& pos, const Vec3& n, const Vec3& worldPos)
			:
			n(n),
			pos(pos),
			worldPos(worldPos)
		{}
		VSOutput& operator+=(const VSOutput& rhs)
		{
			pos += rhs.pos;
			n += rhs.n;
			worldPos += rhs.worldPos;
			return *this;
		}
		VSOutput operator+(const VSOutput& rhs) const
		{
			return VSOutput(*this) += rhs;
		}
		VSOutput& operator-=(const VSOutput& rhs)
		{
			pos -= rhs.pos;
			n -= rhs.n;
			worldPos -= rhs.worldPos;
			return *this;
		}
		VSOutput operator-(const VSOutput& rhs) const
		{
			return VSOutput(*this) -= rhs;
		}
		VSOutput& operator*=(float rhs)
		{
			pos *= rhs;
			n *= rhs;
			worldPos *= rhs;
			return *this;
		}
		VSOutput operator*(float rhs) const
		{
			return VSOutput(*this) *= rhs;
		}
		VSOutput& operator/=(float rhs)
		{
			pos /= rhs;
			n /= rhs;
			worldPos /= rhs;
			return *this;
		}
		VSOutput operator/(float rhs) const
		{
			return VSOutput(*this) /= rhs;
		}
	public:
		Vec4 pos;
		Vec3 n;
		Vec3 worldPos;
	};
	class VertexShader : public BaseVertexShader<VSOutput> {
	public:
		Output operator()(const Vertex& v) const {
			const auto pos = Vec4(v.pos ) ;
			return { pos * worldViewProj, Vec4{v.n, 0.0f } *worldView, pos* worldView };
		}
	public:
		
	};

	class PixelShader : public BasePhongShader<> {
	public:
		template<class Input>
		Color operator()(const Input& in) const {
			return Shade(in, material_color);
		}

	public:
		
		Vec3 material_color = { 0.8f, 0.85f, 1.0f };

	};

	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;
};