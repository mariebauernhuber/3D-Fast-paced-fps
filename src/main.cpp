#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../include/mesh.hpp"
#include "../include/geometry.hpp"
#include "../include/renderer.hpp"
#include "../include/frustumCulling.hpp"
#include <imgui.h>
#include "../imgui/backends/imgui_impl_sdl3.h"
#include "../imgui/backends/imgui_impl_sdlrenderer3.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../include/editor.hpp"
#include <map>

extern bool debugModeTogggled;
extern float deltaTime;
extern float deltaTimeMod;
extern float newDeltaTime;
extern float targetFrameRate;
extern float realFrameRate;

extern long unsigned int selectedIndex;

float windowWidth, windowHeight;

bool paused = false;
bool is_running = false;

unsigned long long nObjRenderCycles = 0;

extern std::vector<Object3D> objects;

extern CullMode gCullMode;

extern mat4x4 matView, matProj;

extern unsigned long long nDrawCycles;

extern vec3d vCamera;
extern vec3d vLookDir;

float fYaw   = 0.0f;   // left/right
float fPitch = 0.0f;   // up/down
float fMaxPitch = 1.55f;
float fMouseSensitivity = 0.0025f;

bool consoleOpen = false;

GLuint fbo, textureColorBuffer;
GLuint rbo;

GLuint mainShaderProgram;

std::string LoadShaderSource(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << buffer.str();
    return buffer.str();
}

GLuint CreateShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    // 1. Compile Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    GLint ok = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &ok);
    if(!ok){
	GLint len = 0;
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &len);
	std::string log(len, ' ');
	glGetShaderInfoLog(vertexShader, len, nullptr, log.data());
	std::cerr << "vertexShader compile error: " << log << "\n";
    }
    
    // 2. Compile Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);
	
    ok = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &ok);
    if(!ok){
	GLint len = 0;
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &len);
	std::string log(len, ' ');
	glGetShaderInfoLog(fragmentShader, len, nullptr, log.data());
	std::cerr << "fragmentShader compile error: " << log << "\n";
    }

    // 3. Link Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 4. Cleanup individual shaders (they are now linked into the program)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int targetWindowWidth, targetWindowHeight;

GLuint quadVAO, quadVBO;
GLuint screenShaderProgram;
const float quadVertices[] = {
	-1.0f, 1.0f,	0.0f, 1.0f,
	-1.0f, -1.0f,	0.0f, 0.0f,
	1.0f, 1.0f, 	1.0f, 1.0f,

	-1.0f, -1.0f,	0.0f, 0.0f, 
	1.0f, -1.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 	1.0f, 1.0f
};

void SetupScreenQuad() {
	const char* screenVertexSource = R"(
	#version 330 core
	layout (location = 0) in vec2 aPos;
	layout (location =1) in vec2 aTexCoord;

	out vec2 TexCoord;

	void main(){
		gl_Position = vec4(aPos, 0.0, 1.0);
		TexCoord = aTexCoord;
	}
	)";

	const char* screenFragmentSource = R"(
	#version 330 core
	out vec4 FragColor;

	in vec2 TexCoord;

	uniform sampler2D screenTexture;

	void main() {
	FragColor = texture(screenTexture, TexCoord);
	}
	)";

	screenShaderProgram = CreateShaderProgram(screenVertexSource, screenFragmentSource);

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

int main(int argc, char* argv[]){
    // 2. Set OpenGL Attributes for Core Profile 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    if(!SDL_Init(SDL_INIT_VIDEO)){
        return 1;
    }


    targetWindowWidth = 1920;
    targetWindowHeight = 1080;

    window = SDL_CreateWindow("Amazing game", targetWindowWidth, targetWindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_RELATIVE_MODE | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_OPENGL);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    //SDL_GL_MakeCurrent(window, gl_context);

    // Initialize GLEW after context creation
    glewExperimental = GL_TRUE; 
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 5. Global OpenGL State Setup
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

std::string vCode = LoadShaderSource("../vertex.glsl");
std::string fCode = LoadShaderSource("../fragment.glsl");

const char* vSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

};
)";
const char* fSource = R"(
//fragment.glsl
#version 330 core
out vec4 FragColor; // The output variable for pixel color

void main() {
    // RGBA format (Red, Green, Blue, Alpha)
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); 
};
)";

mainShaderProgram = CreateShaderProgram(vSource, fSource);

    Object3D ship;
    ship.meshData.LoadFromObjectFile("src/VideoShip.obj");
    ship.position = {0.0f, 0.0f, 5.0f};
    ship.rotation = {0.0f, 0.0f, 0.0f};
    ship.properties["sillyness"] = "mrowwww";
    ship.properties["name"] = "Test ship 1";
    InitializeObjectGPU(ship);
    objects.push_back(ship);

    // 7. Framebuffer (FBO) Initialization
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Color Texture Attachment
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)targetWindowWidth, (int)targetWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // Depth/Stencil Renderbuffer Attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)targetWindowWidth, (int)targetWindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // CRITICAL: Check Completeness before proceeding
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default screen

    SetupScreenQuad();
    
    GLuint screenVAO, screenVBO, screenShader;

    SDL_SetWindowFullscreen(window, true);

    CalculateScreenTransforms(window);
    CalculateScreenProjection();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Optional: Enable Multi-Viewport

    SDL_SetWindowRelativeMouseMode(window, true);

    std::cout << "\nInit done!";

    is_running = true;
    SDL_Event event;

    while(is_running){
	    while(SDL_PollEvent(&event)){
		    ImGui_ImplSDL3_ProcessEvent(&event);
		    if (event.type == SDL_EVENT_QUIT) {
			    is_running = false;
		    }
		    if(event.type == SDL_EVENT_WINDOW_RESIZED){
			CalculateScreenTransforms(window);
			CalculateScreenProjection();
		    }


		    if(event.type == SDL_EVENT_KEY_DOWN){
			if(event.key.scancode == SDL_SCANCODE_F11){
			    if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN){
				SDL_SetWindowFullscreen(window, 0);
				CalculateScreenTransforms(window);
				CalculateScreenProjection();
			    }else{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				CalculateScreenTransforms(window);
				CalculateScreenProjection();
			    };
			    
			};
			if(event.key.scancode == SDL_SCANCODE_ESCAPE){
				paused = !paused;

			}
			if(event.key.scancode == SDL_SCANCODE_GRAVE) {
				debugModeTogggled = !debugModeTogggled;
			};
			if (event.key.scancode == SDL_SCANCODE_F6) {
			    if (gCullMode == CullMode::Back) gCullMode = CullMode::Front;
			    else if (gCullMode == CullMode::Front) gCullMode = CullMode::None;
			    else gCullMode = CullMode::Back;
			}
		    }

		if(!debugModeTogggled){
		    if(event.type == SDL_EVENT_MOUSE_MOTION){
			fYaw   += event.motion.xrel * fMouseSensitivity;
			fPitch -= event.motion.yrel * fMouseSensitivity;

			// Clamp pitch to avoid flipping
			if (fPitch > fMaxPitch)  fPitch = fMaxPitch;
			if (fPitch < -fMaxPitch) fPitch = -fMaxPitch;
		    }
		}

	    }

	    glUseProgram(mainShaderProgram);

	    // 1. Update delta time
	    CalculateDeltaTime();

	    // 2. Limit frame rate
	    float remainingFrameTime = (1.0f / targetFrameRate) - deltaTime;
	    if (remainingFrameTime > 0.001f){
		SDL_Delay(int(remainingFrameTime * 1000.0f));
	    }

	    //3. Now modify deltatime with deltatimemod to allow for timewarping without affecting framerate
	    newDeltaTime = deltaTime * deltaTimeMod;

	    // 3. Process keyboard input for camera movement
	    const bool *key_states = SDL_GetKeyboardState(NULL);
	    float fMoveSpeed = 8.0f * deltaTime;
		
	    // Forward / Back
	    if (key_states[SDL_SCANCODE_W])
		vCamera = Vector_Add(vCamera, Vector_Mul(Vector_Normalise({vLookDir.x, 0.0f, vLookDir.z}), fMoveSpeed));
	    if (key_states[SDL_SCANCODE_S])
		vCamera = Vector_Sub(vCamera, Vector_Mul(Vector_Normalise({vLookDir.x, 0.0f, vLookDir.z}), fMoveSpeed));

	    // Right / Left
	    vec3d vRight = Vector_Normalise(Vector_CrossProduct(vLookDir, {0,1,0}));
	    if (key_states[SDL_SCANCODE_D])
		vCamera = Vector_Sub(vCamera, Vector_Mul(vRight, fMoveSpeed));
	    if (key_states[SDL_SCANCODE_A])
		vCamera = Vector_Add(vCamera, Vector_Mul(vRight, fMoveSpeed));

	    // Up / Down
	    if (key_states[SDL_SCANCODE_SPACE])
		vCamera.y += 8.0f * deltaTime;
	    if (key_states[SDL_SCANCODE_LSHIFT])
		vCamera.y -= 8.0f * deltaTime;

	    // 4. Update camera orientation based on mouse look
	    vec3d vForward;
	    if(!debugModeTogggled){
		    vForward = {
			cosf(fPitch) * sinf(fYaw),
			sinf(fPitch),
			cosf(fPitch) * cosf(fYaw)
		    };
	    }

	    if(debugModeTogggled){
		    if(SDL_GetWindowMouseGrab(window)){ SDL_SetWindowMouseGrab(window, false); };
		    if(SDL_GetWindowRelativeMouseMode(window)){ SDL_SetWindowRelativeMouseMode(window, false); };
	    }else{
		    if(!SDL_GetWindowMouseGrab(window)){ SDL_SetWindowMouseGrab(window, true); };
		    if(!SDL_GetWindowRelativeMouseMode(window)){ SDL_SetWindowRelativeMouseMode(window, true); };
	    }

	    vLookDir = Vector_Normalise(vForward);
	
	    ship.rotation.y = ship.rotation.y + (5.0f * newDeltaTime);

	    // 5. Update view matrix
	    vec3d vUp = {0, 1, 0};
	    matView = Matrix_PointAt(vCamera, Vector_Add(vCamera, vLookDir), vUp);
	    matView = Matrix_QuickInverse(matView);

	    // --- RENDERING PHASE ---
	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	    glClearColor(0.0f, 0.0f, 0.0f,  1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both Color and Depth
	    glViewport(0, 0, targetWindowWidth, targetWindowHeight);

	    RenderObjectModern(ship, mainShaderProgram, matView, matProj);

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    glDisable(GL_DEPTH_TEST);
	    glClear(GL_COLOR_BUFFER_BIT);

	    glViewport(0, 0, targetWindowWidth, targetWindowHeight);

	    glUseProgram(screenShaderProgram);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	    glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTexture"), 0);

	    glClear(GL_COLOR_BUFFER_BIT);
	    ImGui_ImplOpenGL3_NewFrame();
	    ImGui_ImplSDL3_NewFrame();
	    ImGui::NewFrame();

	    if(debugModeTogggled){DrawObjectEditor(objects);};

	    glBindVertexArray(quadVAO);
	    glDrawArrays(GL_TRIANGLES, 0, 6);
	    glBindVertexArray(0);
	    ImGui::Render();
	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



	    // 8. Present final frame
	    SDL_GL_SwapWindow(window);

	    // 9. Update real frame rate
	    realFrameRate = 1.0f / deltaTime;

	    // 11. Increment draw cycles
	    nDrawCycles++;
    }

    SDL_Quit();
    return 0;
}

