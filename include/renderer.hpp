#pragma once
#include "geometry.hpp"
#include <SDL3/SDL.h>
#include "mesh.hpp"

static SDL_Window* window;

enum class CullMode {
    None,
    Back,
    Front
};

void CalculateScreenTransforms(SDL_Window* window);
void CalculateScreenProjection();
void CalculateDeltaTime();
void execOncePerSec();
void MinuteTimer();
void PrintDebugInfo();
vec2d ProjectToScreen(const vec3d &v);
void DrawLine(float x1, float y1, float x2, float y2);
void RenderObject(Object3D &obj, const mat4x4 &matView, const mat4x4 &matProj);
void RenderObjectModern(Object3D &obj, GLuint shaderProgram, const mat4x4 &matView, const mat4x4 &matProj);
void RenderObjectModernViaID(Object3D &obj, int ID, GLuint shaderProgram, const mat4x4 &matView, const mat4x4 &matProj, GLuint frontFace, GLuint cullMode);
