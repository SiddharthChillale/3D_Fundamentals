#pragma once

#include "Vec3.h"
#include "Matrix.h"
#include "IndexedTriangles.h"


class Sphere {
public:
	template<class V>
	static IndexTriangleList<V> GetPlain(float radius = 1.0f, int latDiv = 12, int longDiv = 24) {
		const Vec3 base = { 0.f, 0.f, radius };
		const float H_latAngle = PI / latDiv;
		const float V_longAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;


		// constrruct default vertices and then change their pos after. This is because we do not know what type of vertices are inserted, so cannot call the corresponding vertex constructor.
		for (int iLat = 1; iLat < latDiv; iLat++) {

			const auto latBase = base * Mat3::RotationX(H_latAngle * iLat);

			for (int iLong = 0; iLong < longDiv; iLong++) {
				vertices.emplace_back();
				vertices.back().pos = latBase * Mat3::RotationZ(V_longAngle * iLong);
			}
		}

		const auto iNorthPole = vertices.size();
		vertices.emplace_back();
		vertices.back().pos = base;
		const auto iSouthPole = vertices.size();
		vertices.emplace_back();
		vertices.back().pos = -base;

		// lambda for calculating index of a vertex. Similar to a 2D grid.
		const auto IdxCalculator = [longDiv](int iLat, int iLong) {
			return iLat * longDiv + iLong;
		};

		std::vector<size_t> indices;

		for (int iLat = 0; iLat < latDiv - 2; iLat++) {
			for (int iLong = 0; iLong < longDiv - 1; iLong++) {

				// insert two triangles in each "square" grid of LatxLong

				//upper triangle 
				indices.push_back(IdxCalculator(iLat, iLong)  );
				indices.push_back(IdxCalculator(iLat+1, iLong));
				indices.push_back(IdxCalculator(iLat, iLong+1));

				//bottom triangle
				indices.push_back(IdxCalculator(iLat, iLong+1)  );
				indices.push_back(IdxCalculator(iLat+1, iLong)  );
				indices.push_back(IdxCalculator(iLat+1, iLong+1));

			}
			indices.push_back(IdxCalculator(iLat, longDiv - 1));
			indices.push_back(IdxCalculator(iLat + 1, longDiv - 1));
			indices.push_back(IdxCalculator(iLat, 0));
			indices.push_back(IdxCalculator(iLat, 0));
			indices.push_back(IdxCalculator(iLat + 1, longDiv - 1));
			indices.push_back(IdxCalculator(iLat + 1, 0));
		}

		

		for (int iLong = 0; iLong < longDiv - 1; iLong++) {
			// north 
			indices.push_back(iNorthPole);
			indices.push_back(IdxCalculator(0, iLong));
			indices.push_back(IdxCalculator(0, iLong+1));

			// south
			indices.push_back(IdxCalculator(latDiv-2, iLong+1));
			indices.push_back(IdxCalculator(latDiv-2, iLong));
			indices.push_back( iSouthPole );
		}

		//wrap triangles
		//north
		indices.push_back(iNorthPole);
		indices.push_back(IdxCalculator(0, longDiv - 1));
		indices.push_back(IdxCalculator(0, 0));

		//south
		indices.push_back(IdxCalculator(latDiv - 2, 0));
		indices.push_back(IdxCalculator(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return {
			std::move(vertices), std::move(indices)
		};
	}

	template<class V>
	static IndexTriangleList<V> GetPlainNormals(float radius = 1.0f, int latDiv = 12, int longDiv = 24) {
		auto sphere = GetPlain<V>(radius, latDiv, longDiv);

		for (auto& v : sphere.vertices) {
			v.n = v.pos.GetNormalized();
		}
		return sphere;
	}
};