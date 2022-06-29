#pragma once

#include "Vec3.h"
#include "IndexedLineList.h"
#include "IndexedTriangles.h"
#include "TexVertex.h"
#include <vector>

class Cube {
public:
	Cube(float size) {
		const float side = size / 2.f;

		vertices.emplace_back( -side, -side, -side);
		vertices.emplace_back( side, -side, -side );
		vertices.emplace_back( -side, side, -side );
		vertices.emplace_back( side, side, -side  );
		vertices.emplace_back( -side, -side, side );
		vertices.emplace_back( side, -side, side  );
		vertices.emplace_back( -side, side, side  );
		vertices.emplace_back( side, side, side   );

		
		tc.emplace_back(0.0f, 1.0f);
		tc.emplace_back(1.0f, 1.0f);
		tc.emplace_back(0.0f, 0.0f);
		tc.emplace_back(1.0f, 0.0f);
		tc.emplace_back(1.0f, 1.0f);
		tc.emplace_back(0.0f, 1.0f);
		tc.emplace_back(1.0f, 0.0f);
		tc.emplace_back(0.0f, 0.0f);

	}

	IndexedLineList GetLines()const {
		return {
			vertices,
			{
				0,1 , 1,3 , 3,2 , 2,0,
				0,4 , 1,5 , 3,7 , 2,6,
				4,5 , 5,7 , 7,6 , 6,4
			}
		};
	}

	IndexTriangleList<Vec3> GetTriangles()const {
		return {
			vertices,
			{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
			}
		};
	}

	IndexTriangleList<TexVertex> GetTrianglesTex()const {
		std::vector<TexVertex> tex_verts;
		tex_verts.reserve(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			tex_verts.emplace_back(vertices[i], tc[i]);
		}
		return{
			std::move(tex_verts),
			{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
			}
		};
	}

private:
	std::vector<Vec3> vertices;
	std::vector<Vec2> tc;
};