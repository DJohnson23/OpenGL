#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

Mesh Mesh::Cube()
{
	return LoadMesh("res/meshes/Cube.obj");
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
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")\n";
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
				std::vector<std::string> indicies = tokenize(tokens[1], "/");

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
