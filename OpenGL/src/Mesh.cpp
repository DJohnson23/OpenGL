#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//#define DEBUG_MESH

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

std::vector<std::string> tokenize(std::string s, std::string del = " ") {
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
