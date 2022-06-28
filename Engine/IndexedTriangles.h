#pragma once

#include <vector>
#include "Vec3.h"

struct IndexTriangleList {

	IndexTriangleList(std::vector<Vec3> vertices_in, std::vector<size_t> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
		cullFlags.resize(indices.size() / 3, false);
	}

	std::vector<Vec3> vertices;
	std::vector<size_t> indices;
	std::vector<bool> cullFlags;
};