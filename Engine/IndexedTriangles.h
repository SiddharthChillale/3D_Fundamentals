#pragma once

#include <vector>
#include "Vec3.h"

template<class T>
class IndexTriangleList {
public:
	IndexTriangleList(std::vector<T> vertices_in, std::vector<size_t> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}

	std::vector<T> vertices;
	std::vector<size_t> indices;
};