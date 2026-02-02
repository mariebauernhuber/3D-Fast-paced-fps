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
	bool LoadFromAssimp(const std::string& filename);

};

struct Object3D{
	mesh meshData;
	vec3d position;
	vec3d rotation;
	vec3d rotationPerTick;
	vec3d positionPerTick;
	vec3d relativePositionPerTick;
	vec3d scale;
	mat4x4 GetWorldMatrix();
	std::map<std::string, std::string> properties;
	long unsigned int selectedIndex;
};

void InitializeObjectGPU(Object3D &obj);

bool loadObjPositions(const std::string& path, std::vector<float>& outVertices);

void SetObjDefaults(Object3D &obj);
