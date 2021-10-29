#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//#define DEBUG_MESH

const float Mesh::PI = 3.141592f;

glm::vec3 stoVec3(std::vector<std::string>& vec, int start = 0)
{
	glm::vec3 result = glm::vec3();

	for (int i = start; i < vec.size() && i < start + 3; i++)
	{
		result[i - start] = std::stof(vec[i]);
	}

	return result;
}

void printVec3(glm::vec3& vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

void printVec2(glm::vec2& vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ")";
}

void Mesh::GenerateVerts()
{
	if (verts != nullptr)
		delete[] verts;

	verts = new float[positions.size() * FLOATS_PER_VERT];

	for (int i = 0; i < positions.size(); i++)
	{
		unsigned int index = i * FLOATS_PER_VERT;

		verts[index + 0] = positions[i].x;
		verts[index + 1] = positions[i].y;
		verts[index + 2] = positions[i].z;

		verts[index + 3] = texCoords[i].x;
		verts[index + 4] = texCoords[i].y;

		verts[index + 5] = normals[i].x;
		verts[index + 6] = normals[i].y;
		verts[index + 7] = normals[i].z;
	}
}

Mesh::~Mesh()
{
	if (verts != nullptr)
		delete[] verts;
}

void Mesh::print(std::string name = "Mesh")
{
	std::cout << "---------- " << name << " START ----------" << std::endl;

	for (int i = 0; i < positions.size(); i++)
	{
		std::cout << "v" << i << ":" << std::endl;

		std::cout << "\tp: ";
		printVec3(positions[i]);
		std::cout << std::endl;

		std::cout << "\tn: ";
		printVec3(normals[i]);
		std::cout << std::endl;

		std::cout << "\ttc: ";
		printVec2(texCoords[i]);
		std::cout << std::endl;

		std::cout << std::endl;
	}

	for (int i = 0; i < triangles.size() - 2; i += 3)
	{
		std::cout << "t" << i / 3 << ": " << triangles[i] << " " 
			<< triangles[i + 1] << " " << triangles[i + 2] << std::endl;
	}

	std::cout << "---------- " << name << " END ----------" << std::endl;
}

float* Mesh::getVerts()
{
	if (verts != nullptr)
		return verts;

	GenerateVerts();

	return verts;
}


std::vector<std::string> tokenize(std::string s) {
	std::stringstream stream(s);

	std::vector<std::string> result;

	std::string token;
	while (stream >> token)
	{
		result.push_back(token);
	}

	return result;
}


std::vector<std::string> tokenize(std::string s, std::string del) {
	int start = 0;
	int end = s.find(del);

	std::vector<std::string> tokens;

	while (end != -1) {
		tokens.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}

	tokens.push_back(s.substr(start));

	return tokens;
}

Mesh Mesh::Cube()
{
#ifndef DEBUG_MESH
	return LoadMesh("res/meshes/Cube.obj");
#else
	Mesh cubeMesh = LoadMesh("res/meshes/Cube.obj");
	cubeMesh.print("Cube");
	return cubeMesh;
#endif
}

Mesh Mesh::Plane()
{
#ifndef DEBUG_MESH
	return LoadMesh("res/meshes/Plane.obj");
#else
	Mesh cubeMesh = LoadMesh("res/meshes/Plane.obj");
	cubeMesh.print("Plane");
	return cubeMesh;
#endif
}

Mesh Mesh::Circle(glm::vec3 center, float radius, float thickness, int segments)
{
	Mesh m;

	float halfThick = thickness / 2.0f;

	int numVerts = segments * 2;

	int v = 0;
	for (int i = 0; i < segments; i++)
	{
		float theta = (float)i / segments * 2 * PI;

		float y = center.y + radius * sinf(theta);
		float x = center.x + radius * cosf(theta);

		glm::vec3 p = glm::vec3(x, y, 0);
		glm::vec3 dir = glm::normalize(center - p);

		glm::vec3 close = p + dir * halfThick;
		glm::vec3 far = p - dir * halfThick;

		m.positions.push_back(close);
		m.positions.push_back(far);

		m.normals.push_back(glm::vec3());
		m.normals.push_back(glm::vec3());

		m.texCoords.push_back(glm::vec2());
		m.texCoords.push_back(glm::vec2());
	}

	int t = 0;
	for (int i = 0; i < segments - 1; i++)
	{
		int v0 = i * 2;

		m.triangles.push_back(v0 + 0);
		m.triangles.push_back(v0 + 1);
		m.triangles.push_back(v0 + 3);

		m.triangles.push_back(v0 + 0);
		m.triangles.push_back(v0 + 3);
		m.triangles.push_back(v0 + 2);
	}

	m.triangles.push_back(numVerts - 2);
	m.triangles.push_back(1);
	m.triangles.push_back(0);

	m.triangles.push_back(numVerts - 2);
	m.triangles.push_back(numVerts - 1);
	m.triangles.push_back(1);

	return m;
}

Mesh Mesh::Line(glm::vec3 start, glm::vec3 end, float thickness)
{
	Mesh m;

	float halfThick = thickness / 2;
	glm::vec3 dir = glm::normalize(end - start);
	glm::vec3 perp = glm::vec3(-dir.y, dir.x, 0);

	m.positions =
	{
		start + (perp * halfThick),
		end + (perp * halfThick),
		end - (perp * halfThick),
		start - (perp * halfThick)
	};

	m.texCoords =
	{
		glm::vec2(),
		glm::vec2(),
		glm::vec2(),
		glm::vec2()
	};

	m.normals =
	{
		glm::vec3(),
		glm::vec3(),
		glm::vec3(),
		glm::vec3()
	};

	m.triangles =
	{
		0, 1, 2,
		0, 2, 3
	};

	return m;
}

Mesh Mesh::LoadMesh(const std::string& filepath)
{
	std::vector<std::string> fileTokens = tokenize(filepath, ".");
	if (fileTokens[fileTokens.size() - 1] != "obj")
	{
		std::cout << "Can not read filetype \"." << fileTokens[fileTokens.size() - 1] << "\"\n";
		return Mesh();
	}

	std::vector<glm::vec3> vPositions;
	std::vector<glm::vec3> vNormals;

	Mesh m;

	std::ifstream stream(filepath);
	std::string line;
	
	while (getline(stream, line)) 
	{
		std::vector<std::string> tokens = tokenize(line);
		if (tokens.size() == 0)
			continue;

		if (tokens[0] == "v")
		{
			vPositions.push_back(stoVec3(tokens, 1));
		}
		else if (tokens[0] == "vn")
		{
			vNormals.push_back(stoVec3(tokens, 1));
		}
		else if (tokens[0] == "f")
		{
			int tStart = m.positions.size();

			for (int i = 1; i < tokens.size(); i++) 
			{
				std::vector<std::string> indicies = tokenize(tokens[i], "/");

				int p = stoi(indicies[0]) - 1;
				int n = stoi(indicies[2]) - 1;

				m.positions.push_back(vPositions[p]);
				m.normals.push_back(vNormals[n]);
				m.texCoords.push_back(glm::vec2());
			}

			for (int i = 1; i < tokens.size() - 2; i++)
			{
				m.triangles.push_back(tStart);
				m.triangles.push_back(tStart + i);
				m.triangles.push_back(tStart + i + 1);
			}
		}
		
	}
	
	return m;
}
