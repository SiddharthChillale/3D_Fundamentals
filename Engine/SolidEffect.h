#pragma once
#include "Pipeline.h"
#include "DefaultVertexShader.h"
#include "DefaultGeometryShader.h"

class SolidEffect {
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex(const Vec3& pos)
			:
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vertex& src)
			:
			color(src.color),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Color& color)
			:
			color(color),
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
		Color color;
	};

	typedef DefaultVertexShader<Vertex> VertexShader;
	typedef DefaultGeometryShader<Vertex> GeometryShader;


	class PixelShader {
	public:
		template<class I>
		Color operator()(const I& in) const
		{
			return in.color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;
	GeometryShader gs;
};