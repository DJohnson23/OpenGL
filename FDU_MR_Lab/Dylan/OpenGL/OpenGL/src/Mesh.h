#pragma once

#include "glm/glm.hpp"
#include <string>
#include <vector>

class Mesh {
private:
	static const float PI;
	static const float SQR2;
	float* verts = nullptr;
	void GenerateVerts();

	static void AddCylinder(Mesh& m, float height, float radius, const glm::mat4& transform);

public:

	~Mesh();
	static const int FLOATS_PER_VERT = 8;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> triangles;

	void print(std::string);
	float* getVerts();

	static Mesh Cube();
	static Mesh Plane();
	static Mesh DoublePyramid(float squareLength, float height, float radius);
	static Mesh Circle(glm::vec3 center, float radius, float thickness, int segments = 60);
	static Mesh Line(glm::vec3 start, glm::vec3 end, float thickness);
	static Mesh LoadMesh(const std::string& filepath);

};