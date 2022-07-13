#pragma once

#include <vector>
#include <fstream>
#include <cctype>
#include <sstream>

#include "Vec3.h"
#include "tiny_obj_loader.h"

template<class T>
class IndexTriangleList {
public:
	IndexTriangleList() = default;
	IndexTriangleList(std::vector<T> vertices_in, std::vector<size_t> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}

	static IndexTriangleList<T> Load(const std::string& filename) {
		IndexTriangleList<T> triList;

		bool isCCW = false;
		{
			std::ifstream file(filename);
			std::string firstline;
			std::getline(file, firstline);
			std::transform(firstline.begin(), firstline.end(), firstline.begin(), std::tolower);
			if (firstline.find("ccw") != std::string::npos) {
				isCCW = true;
			}
		}

		// filled by obj loading fn
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::string err;

		// load/parse obj file
		const bool ret = LoadObj(&attrib, &shapes, nullptr, &err, filename.c_str());

		// check for errors
		if (!err.empty() && err.substr(0, 4) != "WARN") {
			throw std::runtime_error(("LoadObj returned error: " + err + "File:" + filename).c_str());
		}
		if (!ret) {
			throw std::runtime_error(("LoadObj returned false File:" + filename).c_str());
		}
		if (shapes.size() == 0u) {
			throw std::runtime_error(("LoadObj object file had no shapes File:" + filename).c_str());
		}

		//extract vertex data
		triList.vertices.reserve(attrib.vertices.size() / 3u);
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			triList.vertices.emplace_back(Vec3(
				attrib.vertices[i + 0],
				attrib.vertices[i + 1],
				attrib.vertices[i + 2]
			));
		}

		//extract index data
		const auto& mesh = shapes[0].mesh;
		triList.indices.reserve(mesh.indices.size());
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			if (mesh.num_face_vertices[f] != 3u) {

				std::stringstream ss;
				ss << "LoadObj error face #" << f << "has" << mesh.num_face_vertices[f] << "vertices";

				throw std::runtime_error(ss.str().c_str());
			}

			for (size_t vn = 0; vn < 3u; vn++) {
				const auto idx = mesh.indices[f * 3u + vn];
				triList.indices.push_back(size_t(idx.vertex_index));
			}

			// reverse winding is file is CCW
			if (isCCW) {
				std::swap(triList.indices.back(), *std::prev(triList.indices.end(), 2));
			}

		}

		return triList;

	}

	std::vector<T> vertices;
	std::vector<size_t> indices;
};