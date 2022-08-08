#pragma once

#include "WinDefines.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangles.h"
#include "NDCSpaceTransformer.h"
#include "Matrix.h"
#include "ZBuffer.h"

#include <memory>
#include <algorithm>

// fixed-function triangle drawing pipeline
// draws textured triangle lists with clamping
template<class Effect>
class Pipeline {
public:
	// vertex type used for geometry and throughout pipeline
	typedef typename Effect::Vertex Vertex;
	typedef typename Effect::VertexShader::Output VSOut;
	typedef typename Effect::GeometryShader::Output GSOut;

public:
	Pipeline(Graphics& gfx) 
	:
	Pipeline(gfx, std::make_shared<ZBuffer> (gfx.ScreenWidth, gfx.ScreenHeight))
	{}
	Pipeline(Graphics& gfx, std::shared_ptr<ZBuffer>pZb_in) :
		gfx(gfx),
		pZb(std::move(pZb_in)) {
		assert(pZb->GetHeight() == gfx.ScreenHeight && pZb->GetWidth() == gfx.ScreenWidth);
	}

	void Draw(IndexTriangleList<Vertex>& triList) {
		ProcessVertices(triList.vertices, triList.indices);
	}
	
	void BeginFrame() {
		pZb->Clear();
	}
	

private:

	

	// vertex processing function
	// transforms vertices and then passes vtx & idx lists to triangle assembler
	void ProcessVertices(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {

		// vector of vertices after their transformation
		std::vector<VSOut> verticesOut(vertices.size());

		// transform vertices using rotation_matrix + translation
		std::transform(vertices.begin(), vertices.end(), verticesOut.begin(), effect.vs);

		// assemble into triangles again
		AssembleTriangles(verticesOut, indices);
	}


	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to post process
	// culls (does not send) back facing triangles
	void AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<size_t>& indices) {

		const auto eyepos = Vec4{ 0.0f, 0.0f, 0.0f, 1.0f } * effect.vs.GetProj();
		for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
			const auto& v0 = vertices[indices[i * 3 + 0]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];

			// take care of backface culling
			if (((v1.pos - v0.pos).cross(v2.pos - v0.pos)) * Vec3(v0.pos - eyepos) <= 0.0f) {
				// process the three vertices into a triangle
				ProcessTriangle(v0, v1, v2,i);
			}
		}
	}

	// triangle processing function
	// takes 3 vertices to generate triangle
	// sends generated triangle to post-processing
	void ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2, size_t triangle_index) {
		// generate triangle from 3 vertices using gs
		// and send to post-processing
		ClipCullTriangle(effect.gs(v0, v1, v2, triangle_index));
	}

	void ClipCullTriangle(Triangle<GSOut>& t) {
		if (t.v0.pos.x > t.v0.pos.w && t.v1.pos.x > t.v1.pos.w && t.v2.pos.x > t.v2.pos.w) {
			return;
		}
		if (t.v0.pos.x < -t.v0.pos.w && t.v1.pos.x < -t.v1.pos.w && t.v2.pos.x < -t.v2.pos.w) {
			return;
		}
		if (t.v0.pos.y > t.v0.pos.w && t.v1.pos.y > t.v1.pos.w && t.v2.pos.y > t.v2.pos.w) {
			return;
		}
		if (t.v0.pos.y < -t.v0.pos.w && t.v1.pos.y < -t.v1.pos.w && t.v2.pos.y < -t.v2.pos.w) {
			return;
		}
		if (t.v0.pos.z > t.v0.pos.w && t.v1.pos.z > t.v1.pos.w && t.v2.pos.z > t.v2.pos.w) {
			return;
		}
		if (t.v0.pos.z < 0.0f && t.v1.pos.z < 0.0f && t.v2.pos.z < 0.0f) {
			return;
		}


		const auto Clip1 = [this](GSOut& v0, GSOut& v1, GSOut& v2) {
			const auto alphaA = (-v0.pos.z) / (v1.pos.z - v0.pos.z);
			const auto alphaB = (-v0.pos.z) / (v2.pos.z - v0.pos.z);

			const auto v1a = interpolate(v0, v1, alphaA);
			const auto v2a = interpolate(v0, v2, alphaB);

			PostProcessTriangleVertices(Triangle<GSOut>{v1a, v1, v2});
			PostProcessTriangleVertices(Triangle<GSOut>{v2a, v1a, v2});

		};
		
		const auto Clip2 = [this](GSOut& v0, GSOut& v1, GSOut& v2) {
			const auto alpha0 = (-v0.pos.z) / (v2.pos.z - v0.pos.z);
			const auto alpha1 = (-v1.pos.z) / (v2.pos.z - v1.pos.z);

			v0 = interpolate(v0, v2, alpha0);
			v1 = interpolate(v1, v2, alpha0);

			PostProcessTriangleVertices(Triangle<GSOut>{v0, v1, v2});
		};

		if (t.v0.pos.z < 0.0f) {
			if (t.v1.pos.z < 0.0f) {
				Clip2(t.v0, t.v1, t.v2);
			}
			else if (t.v2.pos.z < 0.0f) {
				Clip2(t.v0, t.v2, t.v1);
			}
			else {
				Clip1(t.v0, t.v1, t.v2);
			}
		}
		else if (t.v1.pos.z < 0.0f) {
			if (t.v2.pos.z < 0.0f) {
				Clip2(t.v1, t.v2, t.v0);
			}
			else {
				Clip1(t.v1, t.v0, t.v2);
			}
		}
		else if (t.v2.pos.z < 0.0f) {
			Clip1(t.v2, t.v0, t.v1);
		}
		else {
			PostProcessTriangleVertices(t);
		}
	}

	// Perspective and viewport transform
	void PostProcessTriangleVertices(Triangle<GSOut>& triangle) {
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



	void DrawTriangle(const Triangle<GSOut>& triangle) {

		// using pointers so we can swap (for sorting purposes)
		const GSOut* pv0 = &triangle.v0;
		const GSOut* pv1 = &triangle.v1;
		const GSOut* pv2 = &triangle.v2;

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

	void DrawFlatTopTriangle(const GSOut& it0, const GSOut& it1, const GSOut& it2) {
		
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

	void DrawFlatBottomTriangle(const GSOut& it0, const GSOut& it1, const GSOut& it2) {
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

	void DrawFlatTriangle(const GSOut& it0,
		const GSOut& it1,
		const GSOut& it2,
		const GSOut& dv0,
		const GSOut& dv1,
		GSOut itEdge1) {
		
		
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// calculate start and end scanlines
		const int yStart = std::max((int)ceil(it0.pos.y - 0.5f), 0);
		const int yEnd   = std::min((int)ceil(it2.pos.y - 0.5f), (int)gfx.ScreenHeight - 1); // the scanline AFTER the last line drawn

		// do interpolant prestep
		itEdge0 += dv0 * (float(yStart) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float(yStart) + 0.5f - it0.pos.y);

		
		for (int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1)
		{
			// calculate start and end pixels
			const int xStart = std::max((int)ceil(itEdge0.pos.x - 0.5f), 0);
			const int xEnd   = std::min((int)ceil(itEdge1.pos.x - 0.5f), (int)gfx.ScreenWidth - 1); // the pixel AFTER the last pixel drawn

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

				if (pZb->TestAndSet(x, y, iLine.pos.z)) {
					const float w = 1.0f / iLine.pos.w;
					const auto attr = iLine * w;

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
	NDCSpaceTransformer nst;
	std::shared_ptr<ZBuffer> pZb;
};