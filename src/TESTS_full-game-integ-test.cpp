//This is a full test of how a normal player would interact with the game. 
//This file will likely be very long.

#include "mesh.hpp"
#include "shader-utils.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <catch2/catch.hpp>

TEST_CASE("test"){
	SECTION("test"){
		REQUIRE(1+1==2);
	}
}

TEST_CASE("Starting the game normally by running the bin in term with no args"){
	REQUIRE(1+1==2);

	//Window var to be used in this TEST_CASE
	static SDL_Window* window;
	static SDL_GLContext gl_context;
	
	SECTION("Init (system)"){
		//Set opengl attribs, check docs for more info
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Init sdl video sys
		REQUIRE(SDL_Init(SDL_INIT_VIDEO));
		glEnable(GL_DEBUG_OUTPUT);

		//Create window
		window = SDL_CreateWindow("game", 1920, 1080, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_RELATIVE_MODE | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_OPENGL);
		REQUIRE(window != nullptr);
		REQUIRE_THAT(SDL_GetWindowTitle(window), Catch::Matchers::Matches("game"));

		//Create opengl context for window
		gl_context = SDL_GL_CreateContext(window);
		REQUIRE(gl_context != nullptr);

		//Init glew
		glewExperimental = GL_TRUE;
		REQUIRE_NOTHROW(glewInit());
	}

	SECTION("Making the window fullscreen"){
		float mouseX, mouseY;
		SDL_Point mousepos;
		SDL_GetGlobalMouseState(&mouseX, &mouseY);
		mousepos.x = mouseX;
		mousepos.y = mouseY;
		// Get display index at mouse position  
		int displayIndex = SDL_GetDisplayForPoint(&mousepos);

		if (displayIndex < 0) displayIndex = 1;  // Fallback to primary

		// Get display bounds
		SDL_Rect displayBounds;
		SDL_GetDisplayBounds(displayIndex, &displayBounds);

		// Resize/match window to display, then fullscreen
		SDL_SetWindowSize(window, displayBounds.w, displayBounds.h);
		SDL_SetWindowPosition(window, displayBounds.x, displayBounds.y);
		SDL_SetWindowFullscreen(window, true);  // Or SDL_WINDOW_FULLSCREEN
	}

	SECTION("Creating a shader to use"){
		GLuint testShader;
		testShader = CreateShaderProgram(LoadShaderSource("vertex.glsl").c_str(), LoadShaderSource("fragment.glsl").c_str());
		REQUIRE(window != nullptr);
		REQUIRE(testShader != 0);
	}

	SECTION("Render loop"){
		bool is_running = true;
		int drawcycles = 0;
		bool clearswitch = false;
		float clearvar = 0.0f;

		while(is_running){
	
			if(clearswitch){
				if(clearvar == 1.0f){
					clearswitch = true;
				}else{
					clearvar = clearvar + 0.1f;
				}
			}

			glClear(GL_COLOR_BUFFER_BIT);

			SDL_GL_MakeCurrent(window, gl_context);
			SDL_GL_SwapWindow(window);

			drawcycles++;
			if(drawcycles >= 120){
				is_running = false;
			}
		}
		REQUIRE(drawcycles >= 119);
	}

	SECTION("Shutdown"){
		SDL_Delay(500);
		REQUIRE_NOTHROW(SDL_Quit());
	}
}

