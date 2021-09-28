#pragma once

#include "glm/glm.hpp"
#include <string>
#include <vector>

class Mesh {
public:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<int> triangles;

	static Mesh Cube();
	static Mesh LoadMesh(const std::string& filepath);

};