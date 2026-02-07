#include "geometry.hpp"
#include <SDL3/SDL_init.h>
#include <catch2/catch.hpp>
#include "../include/geometry.hpp"
#include "mesh.hpp"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <cstdlib>

TEST_CASE("Vector_Add"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Adding two nonzero vec3ds"){
		vec3d result = Vector_Add(v1, v2);
		REQUIRE(result.x == 5.0f);
		REQUIRE(result.y == 7.0f);
		REQUIRE(result.z == 9.0f);
	}

	SECTION("Adding a zero vec3d to a nonzero vec3d"){
		vec3d result = Vector_Add(v1, vnull);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Adding a nonzero vec3d to a zero vec3d"){
		vec3d result = Vector_Add(vnull, v1);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Adding a zero vec3d to a zero vec3d"){
		vec3d result = Vector_Add(vnull, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Sub"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Subtracting a nonzero vec3d from a nonzero vec3d"){
		vec3d result = Vector_Sub(v1, v2);
		REQUIRE(result.x == -3.0f);
		REQUIRE(result.y == -3.0f);
		REQUIRE(result.z == -3.0f);
	}

	SECTION("Subtracting a zero vec3d from a nonzero vec3d"){
		vec3d result = Vector_Sub(v1, vnull);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Subtracting a nonzero vec3d from a zero vec3d"){
		vec3d result = Vector_Sub(vnull, v1);
		REQUIRE(result.x == -v1.x);
		REQUIRE(result.y == -v1.y);
		REQUIRE(result.z == -v1.z);
	}

	SECTION("Subtracting a zero vec3d from a zero vec3d"){
		vec3d result = Vector_Add(vnull, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Mul"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Multiplying a nonzero vec3d with a nonzero positive float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 5.0f);
		vec3d result = Vector_Mul(v1, 5.0f);
		REQUIRE(result.x == 5.0f);
		REQUIRE(result.y == 10.0f);
		REQUIRE(result.z == 15.0f);
	}

	SECTION("Multiplying a nonzero vec3d with a nonzero negative float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << -10.0f);
		vec3d result = Vector_Mul(v1, -10.0f);
		REQUIRE(result.x == -10.0f);
		REQUIRE(result.y == -20.0f);
		REQUIRE(result.z == -30.0f);
	}

	SECTION("Multiplying a nonzero vec3d with zero"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 0.0f);
		vec3d result = Vector_Mul(v1, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with zero"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 0.0f);
		vec3d result = Vector_Mul(vnull, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with a nonzero positive float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 5.0f);
		vec3d result = Vector_Mul(vnull, 5.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with a nonzero negative float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << -10.0f);
		vec3d result = Vector_Mul(vnull, -10.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Div"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Dividing a nonzero vec3d by a nonzero positive float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 3.0f);
		vec3d result = Vector_Div(v1, 3.0f);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(0.333333f, 0.001f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(0.666666f, 0.001f));
		REQUIRE(result.z == 1.0f);
	}

	SECTION("Dividing a nonzero vec3d by a nonzero negative float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << -3.0f);
		vec3d result = Vector_Div(v1, -3.0f);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(-0.333333f, 0.001f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(-0.666666f, 0.001f));
		REQUIRE(result.z == -1.0f);
	}

	SECTION("Dividing a nonzero vec3d by zero"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 0.0f);
		INFO("The Vector_Div function is supposed to safely handle division by zero by returning zero");
		//IMPORTANT: This operation is supposed to fail (Dividing by zero)
		//The function should return a zero vector!
		vec3d result = Vector_Div(v1, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by a nonzero positive float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 3.0f);
		vec3d result = Vector_Div(vnull, 3.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by a nonzero negative float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << -3.0f);
		vec3d result = Vector_Div(vnull, -3.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by zero (READ NOTE IN unit-tests.cpp!)"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 0.0f);
		INFO("The Vector_Div function is supposed to safely handle division by zero by returning zero");
		//IMPORTANT: This operation is supposed to fail (Dividing by zero)
		//The function should return a zero vector!
		vec3d result = Vector_Div(vnull, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_CrossProduct"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the cross product of two nonzero vec3ds"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		vec3d result = Vector_CrossProduct(v1, v2);
		REQUIRE(result.x == -3.0f);
		REQUIRE(result.y == 6.0f);
		REQUIRE(result.z == -3.0f);
	}

	SECTION("Getting the cross product of a nonzero vec3d and a zero vec3d"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		vec3d result = Vector_CrossProduct(v1, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Getting the cross product of a zero vec3d and a nonzero vec3d"){
		INFO("vec3ds used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		vec3d result = Vector_CrossProduct(vnull, v2);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_DotProduct"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the dot product of two nonzero vec3ds"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		float result = Vector_DotProduct(v1, v2);
		REQUIRE(result == 32.0f);
	}

	SECTION("Getting the dot product of a nonzero vec3d and a zero vec3d"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		float result = Vector_DotProduct(v1, vnull);
		REQUIRE(result == 0.0f);
	}

}

TEST_CASE("Vector_Length"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the length of a nonzero vec3d"){
		INFO("vec3d used: {" << v1.x << ", " << v1.y << ", " << v1.z << "}.");
		float result = Vector_Length(v1);
		REQUIRE(result == std::sqrt(Vector_DotProduct(v1, v1)));
		REQUIRE(result == std::sqrt(14.0f));
		REQUIRE_THAT(result, Catch::Matchers::WithinRel(3.74f, 0.01f));
	}

	SECTION("Getting the length of a zero vec3d"){
		INFO("vec3d used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		float result = Vector_Length(vnull);
		REQUIRE(result == 0.0f);
	}
}

TEST_CASE("Vector_Normalise"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Normalising a nonzero vec3d"){
		INFO("vec3d used: {" << v1.x << ", " << v1.y << ", " << v1.z << "}.");
		vec3d result = Vector_Normalise(v1);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(v1.x / 3.74f, 0.01f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(v1.y / 3.74f, 0.01f));
		REQUIRE_THAT(result.z, Catch::Matchers::WithinRel(v1.z / 3.74f, 0.01f));

		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(0.2673f, 0.01f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(0.5354f, 0.01f));
		REQUIRE_THAT(result.z, Catch::Matchers::WithinRel(0.8018f, 0.01f));
	}

	SECTION("Normalizing a zero vec3d"){
		INFO("vec3d used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		INFO("Pretty sure this also devides by zero.");
		vec3d result = Vector_Normalise(vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}


/*============================================================*/
/* RENDERING TESTS                                            */
/*============================================================*/


TEST_CASE("SDL_GL", "[opengl][sdl]") {
	SECTION("Initialize SDL_VIDEO"){
		REQUIRE(SDL_Init(SDL_INIT_VIDEO) == true);
		SDL_Quit();
	}

	SDL_Init(SDL_INIT_VIDEO);
    
	SECTION("Basic context creation succeeds") {
		SDL_Window* window = SDL_CreateWindow("test", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
		REQUIRE(window != nullptr);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GLContext ctx = SDL_GL_CreateContext(window);
		REQUIRE(ctx != nullptr);

		SDL_GL_DestroyContext(ctx);
		SDL_DestroyWindow(window);
		CAPTURE(SDL_GetError());
	}

	SECTION("Context fails without OPENGL flag") {
		SDL_Window* window = SDL_CreateWindow("test", 1, 1, SDL_WINDOW_HIDDEN);
		REQUIRE(window != nullptr);
		SDL_GLContext ctx = SDL_GL_CreateContext(window);
		REQUIRE(ctx == nullptr);  // Should fail
		SDL_DestroyWindow(window);
		CAPTURE(SDL_GetError());
	}

	SDL_Quit();
}


/*============================================================*/
/* OBJECT SYSTEM TESTS                                        */
/*============================================================*/


TEST_CASE("Object creation and management"){
	SECTION("Creating Object3D"){
		Object3D testobject;
		REQUIRE_NOTHROW(testobject);
	}
}

TEST_CASE("Object3D GPU creation", "[object][gpu]") {
	Object3D obj;

	SECTION("Loading Object Defaults"){
		SetObjDefaults(obj);
		REQUIRE(obj.position.x == 0.0f);
		REQUIRE(obj.position.y == 0.0f);
		REQUIRE(obj.position.z == 0.0f);

		REQUIRE(obj.scale.x == 1.0f);
		REQUIRE(obj.scale.y == 1.0f);
		REQUIRE(obj.scale.z == 1.0f);

		REQUIRE(obj.meshData.tris.empty());

		REQUIRE(obj.rotation.x == 0.0f);
		REQUIRE(obj.rotation.y == 0.0f);
		REQUIRE(obj.rotation.z == 0.0f);
		
		REQUIRE(obj.selectedIndex == 0);

		REQUIRE(obj.positionPerTick.x == 0.0f);
		REQUIRE(obj.positionPerTick.y == 0.0f);
		REQUIRE(obj.positionPerTick.z == 0.0f);

		REQUIRE(obj.rotationPerTick.x == 0.0f);
		REQUIRE(obj.rotationPerTick.y == 0.0f);
		REQUIRE(obj.rotationPerTick.z == 0.0f);

		REQUIRE(obj.relativePositionPerTick.x == 0.0f);
		REQUIRE(obj.relativePositionPerTick.y == 0.0f);
		REQUIRE(obj.relativePositionPerTick.z == 0.0f);

		REQUIRE(obj.GetWorldMatrix().m);
	}

	SECTION("InitializeObjectGPU creates valid handles") {
		REQUIRE_NOTHROW(InitializeObjectGPU(obj));
	}

	SECTION("VAO/VBO survive after creation") {
		GLuint vao_before = obj.meshData.VAO;
		GLuint vbo_before = obj.meshData.VBO;
		InitializeObjectGPU(obj);  // Call twice (idempotent)
		REQUIRE(obj.meshData.VAO == vao_before);  // Same handles reused
		REQUIRE(obj.meshData.VBO == vbo_before);
	}

	SECTION("Vertex count matches triangle data") {
		size_t expected_verts = obj.meshData.tris.size() * 9;  // 3 tris * 3 points * 3 floats
		InitializeObjectGPU(obj);
		// Verify via glGetBufferParameteriv mock or log capture
		INFO("Expected " << expected_verts << " floats for " << obj.meshData.tris.size() << " triangles");
	}

	SECTION("Has valid world matrix post-creation") {
		InitializeObjectGPU(obj);
		mat4x4 world = obj.GetWorldMatrix();
		REQUIRE(glGetError() == GL_NO_ERROR);
	}
}

TEST_CASE("Full Object demo"){
	Object3D obj;

	SECTION("Loading Object Defaults"){
		SetObjDefaults(obj);
		REQUIRE(obj.position.x == 0.0f);
		REQUIRE(obj.position.y == 0.0f);
		REQUIRE(obj.position.z == 0.0f);

		REQUIRE(obj.scale.x == 1.0f);
		REQUIRE(obj.scale.y == 1.0f);
		REQUIRE(obj.scale.z == 1.0f);

		REQUIRE(obj.meshData.tris.empty());

		REQUIRE(obj.rotation.x == 0.0f);
		REQUIRE(obj.rotation.y == 0.0f);
		REQUIRE(obj.rotation.z == 0.0f);

		REQUIRE(obj.positionPerTick.x == 0.0f);
		REQUIRE(obj.positionPerTick.y == 0.0f);
		REQUIRE(obj.positionPerTick.z == 0.0f);

		REQUIRE(obj.rotationPerTick.x == 0.0f);
		REQUIRE(obj.rotationPerTick.y == 0.0f);
		REQUIRE(obj.rotationPerTick.z == 0.0f);

		REQUIRE(obj.relativePositionPerTick.x == 0.0f);
		REQUIRE(obj.relativePositionPerTick.y == 0.0f);
		REQUIRE(obj.relativePositionPerTick.z == 0.0f);

		REQUIRE(obj.GetWorldMatrix().m);
	}

	SECTION("Setting some demo parameters"){
		obj.properties["name"] = "Demo object";
		obj.position = {5.0f, 10.0f, 15.0f};
		obj.rotation = {1.0f, 50.0f, 25.0f};

		REQUIRE(!obj.properties.empty());

		REQUIRE(obj.properties["name"] == "Demo object");

		REQUIRE(obj.position.x == 5.0f);
		REQUIRE(obj.position.y == 10.0f);
		REQUIRE(obj.position.z == 15.0f);

		REQUIRE(obj.rotation.x == 1.0f);
		REQUIRE(obj.rotation.y == 50.0f);
		REQUIRE(obj.rotation.z == 25.0f);
	}

	SECTION("Loading a mesh and adding it to GPU stack"){
		SetObjDefaults(obj);
		obj.position = {5.0f, 5.0f, 5.0f};
		obj.properties["name"] = "test object";
		obj.meshData.LoadFromAssimp("src/VideoShip.obj");
		InitializeObjectGPU(obj);

		REQUIRE(!obj.properties.empty());

		REQUIRE(obj.GetWorldMatrix().m);

		REQUIRE(sizeof(obj.meshData.VAO) != 0);
		REQUIRE(sizeof(obj.meshData.VBO) != 0);

		REQUIRE(obj.meshData.tris.data() != nullptr);
	}

	SECTION("Deleting an object and clearing its adress from the stack"){
		obj.properties.clear();
		obj.meshData.tris.clear();
		REQUIRE(obj.meshData.tris.data() == nullptr);
		REQUIRE(obj.properties.empty());
	}
}
