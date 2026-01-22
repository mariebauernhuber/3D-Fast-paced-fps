#include "../include/renderer.hpp"
#include "../include/frustumCulling.hpp"
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../include/mesh.hpp"
#include "imgui.h"
#include <SDL3/SDL.h>

bool debugModeTogggled = false;
extern unsigned long long nObjRenderCycles;
extern unsigned long long nTrisPushedBack;
unsigned long long nDrawCycles = 0;

extern std::vector<Object3D> objects;

extern int windowWidth, windowHeight;
float fTheta;
float fNear = 0.1f;
float fFar = 1000.0f;
float fFov = 90.0f;
float fFovRad;
float fAspectRatio;
float deltaTime;
float deltaTimeMod = 1;
float newDeltaTime;
float secondTiming = 0;

float targetFrameRate = 60;
float realFrameRate;

vec3d vCamera = { 0.0f, 0.0f, 0.0f };
vec3d vLookDir;

unsigned long long framesElapsedSinceStartup;
float secondsElapsedSinceStartup;
float minutesElapsedSinceStartup;
float hoursElapsedSinceStartup;
float daysElapsedSinceStartup;

mat4x4 matView, matProj;

mat4x4 matRotZ = Matrix_MakeIdentity();
mat4x4 matRotX = Matrix_MakeIdentity();
mat4x4 matRotY = Matrix_MakeIdentity();
mat4x4 matTrans = Matrix_MakeIdentity();
mat4x4 matWorld = Matrix_MakeIdentity();

CullMode gCullMode = CullMode::Back;

void CalculateScreenTransforms(SDL_Window* window){
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    fAspectRatio = (float)windowHeight / (float)windowWidth;
    fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159265358979323f);
}

void CalculateScreenProjection(){
    matProj = Matrix_MakeProjection(90.0f, fAspectRatio, fNear, fFar);
    std::cout << "\nmatProj made!";
    std::cout << "\nfAspectRatio: " << fAspectRatio;
    std::cout << "\nfNear: " << fNear;
    std::cout << "\nfFar: " << fFar;
    std::cout << "\nwindowHeight: " << (float)windowHeight;
    std::cout << "\nwindowWidth: " << (float)windowWidth;
}


void CalculateDeltaTime(){ 
    static Uint64 lastCounter = 0;

    Uint64 currentCounter = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();

    secondTiming = SDL_GetTicks() / 1000.0f;
    if(secondTiming > 1000.0f){
	    secondTiming = 0.0f;
    }


    deltaTime = 0.0f;

    if (lastCounter != 0)
    {
        deltaTime = (float)(currentCounter - lastCounter) / (float)frequency;
    }

    lastCounter = currentCounter;
}

void execOncePerSec(){  // A function that will execute once every new second since startup

}

void MinuteTimer(){
    framesElapsedSinceStartup++;

    if(framesElapsedSinceStartup == targetFrameRate){
        secondsElapsedSinceStartup++;
        framesElapsedSinceStartup = 0;

        minutesElapsedSinceStartup = secondsElapsedSinceStartup / 60.0f;
        hoursElapsedSinceStartup = minutesElapsedSinceStartup / 60.0f;
        daysElapsedSinceStartup = hoursElapsedSinceStartup / 24.0f;

        execOncePerSec();
    }
}

void PrintDebugInfo(){
    if(debugModeTogggled) {
    std::cout << "Targets: \n" << "deltaTime: " << (1 / targetFrameRate) << "\n" << "frameRate: " << targetFrameRate << "\n";

    std::cout << "Real measurements: \n" << "frameRate: " << realFrameRate << "\n" << "deltaTime: " << deltaTime << "\n";

    std::cout << "Timing functions: \n" << "frameProgressToSecondTimer: " << framesElapsedSinceStartup << "\n";
    std::cout << "secondsElapsedSinceStartup: " << secondsElapsedSinceStartup << "\n";
    std::cout << "minutesElapsedSinceStartup: " << minutesElapsedSinceStartup << "\n";
    std::cout << "hoursElapsedSinceStartup: " << hoursElapsedSinceStartup << "\n";
    std::cout << "daysElapsedSinceStartup: " << daysElapsedSinceStartup << "\n\n";

    std::cout << "Draw Cycles completed: " << nDrawCycles << "\n";
    std::cout << "OBJ render cycles completed: " << nObjRenderCycles <<"\n";
    std::cout << "TrisPushBacks: " << nTrisPushedBack << "\n\n";
    std::cout << "fAspectRatio: " << fAspectRatio << "\n";

    std::cout << "Camera Position:\n X: " << vCamera.x << "\n" << " Y: " << vCamera.y << "\n";
    std::cout << "\033[2J\033[1;1H"; //ANSI CODES to clear console screen
    
    }
}

vec2d ProjectToScreen(const vec3d &v) {
    // v.x and v.y are in range [-1, 1] after the perspective divide
    return {
        (v.x + 1.0f) * 0.5f * (float)windowWidth,          // Maps -1..1 to 0..Width
        (1.0f - (v.y + 1.0f) * 0.5f) * (float)windowHeight // Maps -1..1 to 0..Height (inverted Y)
    };
}

void DrawLine(float x1, float y1, float x2, float y2){
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 1.0f); // red, green, blue values
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void RenderObjectCPU(Object3D &obj, const mat4x4 &matView, const mat4x4 &matProj) {
    std::vector<triangle> vecTrianglesToRaster;

    mat4x4 matWorld = obj.GetWorldMatrix();

    for(const auto &tri : obj.meshData.tris) {
        triangle triTransformed, triViewed;

        // 1. Transform to world space
        triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
        triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
        triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

        // 2. Transform to view space
        triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
        triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
        triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);

        // 3. Backface culling
        vec3d normal = Vector_CrossProduct(
            Vector_Sub(triViewed.p[1], triViewed.p[0]),
            Vector_Sub(triViewed.p[2], triViewed.p[0])
        );
        normal = Vector_Normalise(normal);

        if(gCullMode == CullMode::Back && Vector_DotProduct(normal, triViewed.p[0]) >= 0.0f) continue;
        if(gCullMode == CullMode::Front && Vector_DotProduct(normal, triViewed.p[0]) < 0.0f) continue;

        // 4. Clip triangle against frustum planes
        auto clippedTris = ClipTriangleToFrustumOptimized(triViewed);
	

        // 5. Project and add to raster list
        for(auto &triProj : clippedTris) {
            triangle triProjected;
            triProjected.p[0] = Matrix_MultiplyVector(matProj, triProj.p[0]);
            triProjected.p[1] = Matrix_MultiplyVector(matProj, triProj.p[1]);
            triProjected.p[2] = Matrix_MultiplyVector(matProj, triProj.p[2]);

            // Perspective divide
            for(int i=0; i<3; i++){
                triProjected.p[i].x /= triProjected.p[i].w;
                triProjected.p[i].y /= triProjected.p[i].w;
                triProjected.p[i].z /= triProjected.p[i].w;
            }
		
            vecTrianglesToRaster.push_back(triProjected);
        }
    }
	
    // 6. Depth sort
    std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](const triangle &t1, const triangle &t2){
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
    }); 

    // 7. Rasterize
    for(const auto &t : vecTrianglesToRaster){
        vec2d p0 = ProjectToScreen(t.p[0]);
        vec2d p1 = ProjectToScreen(t.p[1]);
        vec2d p2 = ProjectToScreen(t.p[2]);
	
	DrawLine(p0.x, p0.y, p1.x, p1.y);
	DrawLine(p1.x, p1.y, p2.x, p2.y);
	DrawLine(p2.x, p2.y, p0.x, p0.y);
    }

}

void RenderObjectModern(Object3D &obj, GLuint shaderProgram, const mat4x4 &matView, const mat4x4 &matProj) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos;
    pos[0] = obj.position.x;
    pos[1] = obj.position.y;
    pos[2] = obj.position.z;

    model = glm::translate(model, pos);

    // 1. Pass transformation matrices as uniforms
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    mat4x4 matWorld = obj.GetWorldMatrix();

    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &matWorld.m[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &matView.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &matProj.m[0][0]);

    // 2. Bind and Draw
    glBindVertexArray(obj.meshData.VAO); 
    glDrawArrays(GL_TRIANGLES, 0, obj.meshData.tris.size() * 3);
    glBindVertexArray(0);
}

void RenderObjectModernViaID(Object3D &obj, int ID, GLuint shaderProgram, const mat4x4 &matView, const mat4x4 &matProj, GLuint frontFace, GLuint cullMode){
    glUseProgram(shaderProgram);
    glFrontFace(frontFace);
    glCullFace(cullMode);
	

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos;
    pos[0] = objects[ID].position.x;
    pos[1] = objects[ID].position.y;
    pos[2] = objects[ID].position.z;

    if(objects[ID].rotation.x == 360.0f){
	    objects[ID].rotation.x = 0.0f;
    }else if(objects[ID].rotation.x > 360.0f){
	    objects[ID].rotation.x = 0.0f + (objects[ID].rotation.x - 360.0f);
    }

    if(objects[ID].rotation.y == 360.0f){
	    objects[ID].rotation.y = 0.0f;
    }else if(objects[ID].rotation.y > 360.0f){
	    objects[ID].rotation.y = 0.0f + (objects[ID].rotation.y - 360.0f);
    }

    if(objects[ID].rotation.z == 360.0f){
	    objects[ID].rotation.z = 0.0f;
    }else if(objects[ID].rotation.z > 360.0f){
	    objects[ID].rotation.z = 0.0f + (objects[ID].rotation.z - 360.0f);
    }

    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(objects[ID].rotation.x), {1.0f * newDeltaTime, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(objects[ID].rotation.y), {0.0f, 1.0f * newDeltaTime, 0.0f});
    model = glm::rotate(model, glm::radians(objects[ID].rotation.z), {0.0f, 0.0f, 1.0f * newDeltaTime});

    // 1. Pass transformation matrices as uniforms
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    mat4x4 matWorld = objects[ID].GetWorldMatrix();

    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &matWorld.m[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &matView.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &matProj.m[0][0]);

    // 2. Bind and Draw
    glBindVertexArray(obj.meshData.VAO); 
    glDrawArrays(GL_TRIANGLES, 0, obj.meshData.tris.size() * 3);
    glBindVertexArray(0);
}
