#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


#include "glm/gtc/matrix_transform.hpp"

//#define DEBUG_MESH

const float Mesh::PI = 3.141592f;
const float Mesh::SQR2 = 1.41421356;

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

void Mesh::AddCylinder(Mesh& m, const float height, const float radius, const glm::mat4& transform)
{
	const int SEGMENTS = 10;

	int start = m.positions.size();

	for (int i = 0; i < SEGMENTS; i++)
	{
		float theta = (float)i / SEGMENTS * 2.0f * PI;
		float x = radius * cosf(theta);
		float z = -radius * sinf(theta);

		glm::vec3 p1 = transform * glm::vec4(x, 0, z, 1);
		glm::vec3 p2 = transform * glm::vec4(x, height, z, 1);
		glm::vec3 c = transform * glm::vec4(0, 0, 0, 1);

		m.positions.push_back(p1);
		m.positions.push_back(p2);

		glm::vec3 norm = glm::normalize(p1 - c);

		m.normals.push_back(norm);
		m.normals.push_back(norm);

		m.texCoords.push_back(glm::vec2());
		m.texCoords.push_back(glm::vec2());
	}

	for (int i = 0; i < SEGMENTS - 1; i++)
	{
		int v = start + i * 2;

		m.triangles.push_back(v + 0);
		m.triangles.push_back(v + 2);
		m.triangles.push_back(v + 3);

		m.triangles.push_back(v + 0);
		m.triangles.push_back(v + 3);
		m.triangles.push_back(v + 1);
	}

	m.triangles.push_back(m.positions.size() - 2);
	m.triangles.push_back(start);
	m.triangles.push_back(start + 1);

	m.triangles.push_back(m.positions.size() - 2);
	m.triangles.push_back(start + 1);
	m.triangles.push_back(m.positions.size() - 1);
}

void Mesh::AddSphere(Mesh& m, const float radius, const glm::vec3& center)
{
	const int HEIGHT_SEGMENTS = 10;
	const int ROUND_SEGMENTS = 10;

	const float deltaSphereTheta = PI / HEIGHT_SEGMENTS;
	const float deltaCircleTheta = 2 * PI / ROUND_SEGMENTS;

	int start = m.positions.size();

	m.positions.push_back(center + glm::vec3(0, radius, 0));
	m.normals.push_back(glm::vec3(0, 1, 0));
	m.texCoords.push_back(glm::vec2());

	for (int i = 1; i < HEIGHT_SEGMENTS; i++)
	{
		float sphereTheta = (PI / 2.0f) - deltaSphereTheta * i;
		float r = radius * cosf(sphereTheta);
		float y = radius * sinf(sphereTheta);

		for (int j = 0; j < ROUND_SEGMENTS; j++)
		{
			float circleTheta = j * deltaCircleTheta;

			float x = r * cosf(circleTheta);
			float z = r * sinf(circleTheta);

			glm::vec3 relPoint(x, y, z);

			m.positions.push_back(center + relPoint);
			m.normals.push_back(glm::normalize(relPoint));
			m.texCoords.push_back(glm::vec2());
		}
	}

	m.positions.push_back(center + glm::vec3(0, -radius, 0));
	m.normals.push_back(glm::vec3(0, -1, 0));
	m.texCoords.push_back(glm::vec2());

	int last = m.positions.size() - 1;

	for (int i = 0; i < HEIGHT_SEGMENTS - 2; i++)
	{
		int v = start + 1 + i * ROUND_SEGMENTS;
		for (int j = 0; j < ROUND_SEGMENTS - 1; j++)
		{
			int curV = v + j;
			m.triangles.push_back(curV);
			m.triangles.push_back(curV + ROUND_SEGMENTS);
			m.triangles.push_back(curV + ROUND_SEGMENTS + 1);

			m.triangles.push_back(curV);
			m.triangles.push_back(curV + ROUND_SEGMENTS + 1);
			m.triangles.push_back(curV + 1);
		}

		m.triangles.push_back(v + ROUND_SEGMENTS - 1);
		m.triangles.push_back(v + ROUND_SEGMENTS + ROUND_SEGMENTS - 1);
		m.triangles.push_back(v + ROUND_SEGMENTS);

		m.triangles.push_back(v + ROUND_SEGMENTS - 1);
		m.triangles.push_back(v + ROUND_SEGMENTS);
		m.triangles.push_back(v);
	}

	for (int i = 1; i < ROUND_SEGMENTS; i++)
	{
		int vTop = start + i;
		int vBottom = last - i;

		m.triangles.push_back(start);
		m.triangles.push_back(vTop);
		m.triangles.push_back(vTop + 1);

		m.triangles.push_back(last);
		m.triangles.push_back(vBottom);
		m.triangles.push_back(vBottom - 1);
	}

	m.triangles.push_back(start);
	m.triangles.push_back(start + ROUND_SEGMENTS);
	m.triangles.push_back(start + 1);

	m.triangles.push_back(last);
	m.triangles.push_back(last - ROUND_SEGMENTS);
	m.triangles.push_back(last - 1);
}

Mesh Mesh::DoublePyramid(const float squareLength, const float height, const float radius)
{
	float actualLength = squareLength - 2.0f * radius;

	Mesh m;
	
	glm::mat4 moveRight = glm::translate(glm::mat4(1.0f), glm::vec3(actualLength / 2.0f, 0, 0));
	glm::mat4 moveLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-actualLength / 2.0f, 0, 0));
	glm::mat4 moveForward = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, actualLength / 2.0f));
	glm::mat4 moveBack = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -actualLength / 2.0f));
	glm::mat4 moveUp = glm::translate(glm::mat4(1.0f), glm::vec3(0, height / 2, 0));

	glm::mat4 rotLeft = glm::rotate(glm::mat4(1.0f), PI / 2.0f, glm::vec3(0, 0, 1));
	glm::mat4 rotForward = glm::rotate(glm::mat4(1.0f), PI / 2.0f, glm::vec3(1, 0, 0));

	//Square
	glm::mat4 transform = moveUp * moveForward * moveRight * rotLeft;
	AddCylinder(m, actualLength, radius, transform);

	transform = moveUp * moveBack * moveRight * rotLeft;
	AddCylinder(m, actualLength, radius, transform);
	
	transform = moveUp * moveRight * moveBack * rotForward;
	AddCylinder(m, actualLength, radius, transform);

	transform = moveUp * moveLeft * moveBack * rotForward;
	AddCylinder(m, actualLength, radius, transform);

	float xz = 0.5f * squareLength - radius;
	float y = 0.5f * height - radius;
	float legHeight = sqrtf((2 * xz * xz) + (y * y));
	float d = actualLength / 2 * SQR2;

	float theta = PI / 2.0f - atan2f(y, d);
	
	rotForward = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1, 0, 0));
	moveUp = glm::translate(glm::mat4(1.0f), glm::vec3(0, radius, 0));

	//Bottom Legs
	rotLeft = glm::rotate(glm::mat4(1.0f), PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 3 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 5 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 7 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	//Top legs
	rotForward = glm::rotate(glm::mat4(1.0f), theta + PI, glm::vec3(1, 0, 0));
	moveUp = glm::translate(glm::mat4(1.0f), glm::vec3(0, height - radius, 0));

	rotLeft = glm::rotate(glm::mat4(1.0f), PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 3 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 5 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	rotLeft = glm::rotate(glm::mat4(1.0f), 7 * PI / 4.0f, glm::vec3(0, 1, 0));
	transform = moveUp * rotLeft * rotForward;
	AddCylinder(m, legHeight, radius, transform);

	// JOINTS
	AddSphere(m, radius, glm::vec3(0, radius, 0));
	AddSphere(m, radius, glm::vec3(0, height - radius, 0));

	float sphereD = squareLength / 2.0f - radius;
	AddSphere(m, radius, glm::vec3(sphereD, height / 2, sphereD));
	AddSphere(m, radius, glm::vec3(-sphereD, height / 2, sphereD));
	AddSphere(m, radius, glm::vec3(sphereD, height / 2, -sphereD));
	AddSphere(m, radius, glm::vec3(-sphereD, height / 2, -sphereD));
	
	return m;
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

			glm::vec3 point = vPositions[vPositions.size() - 1];
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

Mesh Mesh::UIRectangle(glm::vec2 min, glm::vec2 max)
{
	Mesh m;

	m.positions =
	{
		glm::vec3(min.x, min.y, 0.0f),
		glm::vec3(min.x, max.y, 0.0f),
		glm::vec3(max.x, max.y, 0.0f),
		glm::vec3(max.x, min.y, 0.0f)
	};

	m.texCoords =
	{
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
		glm::vec2(1, 0)
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
