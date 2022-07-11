#pragma once

#include "WinDefines.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangles.h"
#include "NDCSpaceTransformer.h"
#include "Mat3.h"
#include "ZBuffer.h"

#include <algorithm>

// fixed-function triangle drawing pipeline
// draws textured triangle lists with clamping
template<class Effect>
class Pipeline {
public:
	// vertex type used for geometry and throughout pipeline
	typedef typename Effect::Vertex Vertex;

public:
	Pipeline(Graphics& gfx) 
	:
	gfx(gfx),
	zb(gfx.ScreenWidth, gfx.ScreenHeight)
	{}
	void Draw(IndexTriangleList<Vertex>& triList) {
		ProcessVertices(triList.vertices, triList.indices);
	}
	void BindRotation(const Mat3& rotation_in) {
		rotation = rotation_in;
	}
	void BindTranslation(const Vec3& translation_in) {
		translation = translation_in;
	}
	void BeginFrame() {
		zb.Clear();
	}
	

private:

	// vertex processing function
	// transforms vertices and then passes vtx & idx lists to triangle assembler
	void ProcessVertices(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {

		// vector of vertices after their transformation
		std::vector<Vertex> verticesOut;

		// transform vertices using rotation_matrix + translation
		for (const auto& v : vertices) {
			verticesOut.emplace_back(v.pos * rotation + translation, v);
		}

		// assemble into triangles again
		AssembleTriangles(verticesOut, indices);
	}


	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to post process
	// culls (does not send) back facing triangles
	void AssembleTriangles(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {
		for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
			const auto& v0 = vertices[indices[i * 3 + 0]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];

			// take care of backface culling
			if (((v1.pos - v0.pos).cross(v2.pos - v0.pos)) * v0.pos <= 0.0f) {
				// process the three vertices into a triangle
				ProcessTriangle(v0, v1, v2);
			}
		}
	}

	// triangle processing function
	// takes 3 vertices to generate triangle
	// sends generated triangle to post-processing
	void ProcessTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		// generate triangle from 3 vertices using gs
		// and send to post-processing
		PostProcessTriangleVertices(Triangle<Vertex>{v0, v1, v2});
	}

	// Perspective and viewport transform
	void PostProcessTriangleVertices(Triangle<Vertex>& triangle) {
		nst.Transform(triangle.v0);
		nst.Transform(triangle.v1);
		nst.Transform(triangle.v2);

		DrawTriangle(triangle);
	}

	// ============================== triangle rasterization functions ===========================
	//	it0, it1, etc. stand for interpolants
	//	(values which are interpolated across a triangle in screen space)
	//
	//	entry point for tri rasterization
	//	sorts vertices, determines case, splits to flat tris, dispatches to flat tri funcs



	void DrawTriangle(const Triangle<Vertex>& triangle) {
		
		// using pointers so we can swap (for sorting purposes)
		const Vertex* pv0 = &triangle.v0;
		const Vertex* pv1 = &triangle.v1;
		const Vertex* pv2 = &triangle.v2;

		// sorting vertices by y
		if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);
		if (pv2->pos.y < pv1->pos.y) std::swap(pv1, pv2);
		if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);

		if (pv0->pos.y == pv1->pos.y) // natural flat top
		{
			// sorting top vertices by x
			if (pv1->pos.x < pv0->pos.x) std::swap(pv0, pv1);

			DrawFlatTopTriangle(*pv0, *pv1, *pv2);
		}
		else if (pv1->pos.y == pv2->pos.y) // natural flat bottom
		{
			// sorting bottom vertices by x
			if (pv2->pos.x < pv1->pos.x) std::swap(pv1, pv2);

			DrawFlatBottomTriangle(*pv0, *pv1, *pv2);
		}
		else // general triangle
		{
			// find splitting vertex interpolant
			const float alphaSplit =
				(pv1->pos.y - pv0->pos.y) /
				(pv2->pos.y - pv0->pos.y);
			const auto vi = interpolate(*pv0, *pv2, alphaSplit);

			if (pv1->pos.x < vi.pos.x) // major right
			{
				DrawFlatBottomTriangle(*pv0, *pv1, vi);
				DrawFlatTopTriangle(*pv1, vi, *pv2);
			}
			else // major left
			{
				DrawFlatBottomTriangle(*pv0, vi, *pv1);
				DrawFlatTopTriangle(vi, *pv1, *pv2);
			}
		}
	}

	void DrawFlatTopTriangle(const Vertex& it0, const Vertex& it1, const Vertex& it2) {
		
		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it2 - it0) / delta_y;
		const auto dit1 = (it2 - it1) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it1;

		// call the flat triangle render routine
		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);
	}

	void DrawFlatBottomTriangle(const Vertex& it0, const Vertex& it1, const Vertex& it2) {
		// calculate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it1 - it0) / delta_y;
		const auto dit1 = (it2 - it0) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it0;

		// call the flat triangle render routine
		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);
	}

	void DrawFlatTriangle(const Vertex& it0,
		const Vertex& it1,
		const Vertex& it2,
		const Vertex& dv0,
		const Vertex& dv1,
		Vertex itEdge1) {
		
		
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// calculate start and end scanlines
		const int yStart = (int)ceil(it0.pos.y - 0.5f);
		const int yEnd = (int)ceil(it2.pos.y - 0.5f); // the scanline AFTER the last line drawn

		// do interpolant prestep
		itEdge0 += dv0 * (float(yStart) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float(yStart) + 0.5f - it0.pos.y);

		
		for (int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1)
		{
			// calculate start and end pixels
			const int xStart = (int)ceil(itEdge0.pos.x - 0.5f);
			const int xEnd = (int)ceil(itEdge1.pos.x - 0.5f); // the pixel AFTER the last pixel drawn

			// create scanline interpolant startpoint
			// (some waste for interpolating x,y,z, but makes life easier not having
			//  to split them off, and z will be needed in the future anyways...)
			auto iLine = itEdge0;

			// calculate delta scanline interpolant / dx
			const float dx = itEdge1.pos.x - itEdge0.pos.x;
			const auto diLine = (itEdge1 - iLine) / dx;

			// prestep scanline interpolant
			iLine += diLine * (float(xStart) + 0.5f - itEdge0.pos.x);

			for (int x = xStart; x < xEnd; x++, iLine += diLine)
			{

				const float z = 1.0f / iLine.pos.z;

				if (zb.TestAndSet(x, y, z)) {
					const auto attr = iLine * z;

					// perform texture lookup, clamp, and write pixel
					gfx.PutPixel(x, y, effect.ps(attr));
				}
				
			}
		}
	}
	public:
		Effect effect;
private:
	Graphics& gfx;
	ZBuffer zb;
	Mat3 rotation;
	Vec3 translation;
	NDCSpaceTransformer nst;
	std::unique_ptr<Surface> pTex;

};