#pragma once
#include "geometry.hpp"
#include <vector>
#include <string>
#include <map>
#include <GL/glew.h>

struct mesh{
	std::vector<triangle> tris;
	GLuint VBO, VAO;
	bool LoadFromObjectFile(const std::string& filename);
	bool LoadFromObjectFileNew(const std::string& filename);

};

struct Object3D{
	mesh meshData;
	vec3d position;
	vec3d rotation;
	vec3d rotationPerTick;
	vec3d positionPerTick;
	vec3d scale = {1,1,1};
	mat4x4 GetWorldMatrix();
	std::map<std::string, std::string> properties;
	long unsigned int selectedIndex;
};

void InitializeObjectGPU(Object3D &obj);

bool loadObjPositions(const std::string& path, std::vector<float>& outVertices);
