//THIS FILE IS CURRENTLY UNUSED!
#include "../include/objdefs.hpp"
#include "mesh.hpp"
extern std::vector<Object3D> objects;

void DefObjs3D(){
// Reserve for 1000 objects to avoid reallocation
objects.reserve(1000);

Object3D ship;
ship.meshData.LoadFromObjectFile("src/VideoShip.obj");
ship.properties["sillyness"] = "mrowwww";
ship.properties["name"] = "Test ship 1";
ship.scale = {1.0f, 1.0f, 1.0f};
ship.position = {0.0f, 0.0f, 10.0f};
ship.rotation = {0.0f, 0.0f, 0.0f};
ship.rotationPerTick = {1.0f, 0.0f, 0.0f};
ship.positionPerTick = {1.0f, 0.0f, 0.0f};
objects.push_back(ship);
InitializeObjectGPU(ship);

Object3D dickMaster;
dickMaster.meshData.LoadFromObjectFileNew("src/flower.obj");
dickMaster.position = {20.0f, 20.0f, 20.0f};
dickMaster.rotation = {0.0f, 0.0f, 0.0f};
dickMaster.rotationPerTick = {1.0f, 0.0f, 0.0f};
dickMaster.properties["name"] = "sometimes i wonder if you know what it means";
objects.push_back(dickMaster);
InitializeObjectGPU(dickMaster);

Object3D teddy;
teddy.meshData.LoadFromObjectFileNew("src/teddybear.obj");
teddy.position = {20.0f, 0.0f, 0.0f};
teddy.rotation = {0.0f, 0.0f, 0.0f};
teddy.rotationPerTick = {5.0f, 0.0f, 0.0f};
teddy.positionPerTick = {5.0f, 5.0f, 5.0f};
teddy.properties["name"] = "A pretty teddybear :3";
objects.push_back(teddy);
InitializeObjectGPU(teddy);
}
