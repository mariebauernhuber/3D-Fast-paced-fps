#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl3.h"
#include "../include/editor.hpp"
#include "../include/geometry.hpp"
#include "../include/imgui-gruvbox.hpp"
#include "../include/mesh.hpp"
#include "../include/renderer.hpp"
#include "../include/shader-utils.hpp"
#include "../include/unit-tests.hpp"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <cstdlib>
#include <imgui.h>
#include <iostream>
#include <map>
#include <vector>
#include <catch2/catch.hpp>

extern bool debugModeTogggled;
extern float deltaTime;
extern float deltaTimeMod;
extern float newDeltaTime;
extern float targetFrameRate;
extern float realFrameRate;
extern bool isImGuiGameViewportInMouseLock;
bool shouldAllowMove = true;
extern float secondsElapsedSinceStartup;
extern float secondTiming;

std::vector<Object3D> objects;

extern long unsigned int selectedIndex;

float windowWidth, windowHeight;

std::string cullingMode = "GL_BACK";

bool paused = false;
bool is_running = false;

unsigned long long nObjRenderCycles = 0;

extern CullMode gCullMode;

extern mat4x4 matView, matProj;

extern unsigned long long nDrawCycles;

extern vec3d vCamera;
extern vec3d vLookDir;

float fYaw = 0.0f;   // left/right
float fPitch = 0.0f; // up/down
float fMaxPitch = 1.55f;
float fMouseSensitivity = 0.0025f;

GLuint fbo, textureColorBuffer, rbo;

GLuint mainShaderProgram;
GLuint colorCycleShader;

int targetWindowWidth, targetWindowHeight;

extern GLuint quadVAO, quadVBO;
extern GLuint screenShaderProgram;

mesh CreateTestCubeCCW() {
    mesh testMesh;
    testMesh.tris.clear();
    
    // 8 cube vertices (unit cube centered at origin)
    std::vector<vec3d> verts = {
        {-0.5, -0.5,  0.5},  // 0 front-bottom-left
        { 0.5, -0.5,  0.5},  // 1 front-bottom-right
        { 0.5,  0.5,  0.5},  // 2 front-top-right
        {-0.5,  0.5,  0.5},  // 3 front-top-left
        {-0.5, -0.5, -0.5},  // 4 back-bottom-left
        { 0.5, -0.5, -0.5},  // 5 back-bottom-right
        { 0.5,  0.5, -0.5},  // 6 back-top-right
        {-0.5,  0.5, -0.5}   // 7 back-top-left
    };
    
    // 12 triangles (2 per face), CCW winding when viewed from outside
    // Front (+Z)
    testMesh.tris.push_back({verts[0], verts[1], verts[2]});
    testMesh.tris.push_back({verts[0], verts[2], verts[3]});
    
    // Back (-Z)
    testMesh.tris.push_back({verts[5], verts[4], verts[7]});
    testMesh.tris.push_back({verts[5], verts[7], verts[6]});
    
    // Right (+X)
    testMesh.tris.push_back({verts[1], verts[5], verts[6]});
    testMesh.tris.push_back({verts[1], verts[6], verts[2]});
    
    // Left (-X)
    testMesh.tris.push_back({verts[4], verts[0], verts[3]});
    testMesh.tris.push_back({verts[4], verts[3], verts[7]});
    
    // Top (+Y)
    testMesh.tris.push_back({verts[3], verts[2], verts[6]});
    testMesh.tris.push_back({verts[3], verts[6], verts[7]});
    
    // Bottom (-Y)
    testMesh.tris.push_back({verts[4], verts[5], verts[1]});
    testMesh.tris.push_back({verts[4], verts[1], verts[0]});
    
    return testMesh;
}

extern int run_tests();

int main(int argc, char *argv[]) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return 1;
  }

  glEnable(GL_DEBUG_OUTPUT);

  targetWindowWidth = 1920;
  targetWindowHeight = 1080;

  window =
      SDL_CreateWindow("Amazing game", targetWindowWidth, targetWindowHeight,
                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_RELATIVE_MODE |
                           SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_OPENGL);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  // SDL_GL_MakeCurrent(window, gl_context);

  // Initialize GLEW after context creation
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // 5. Global OpenGL State Setup
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE); // Enable culling

  mainShaderProgram = CreateShaderProgram(LoadShaderSource("vertex.glsl").c_str(), LoadShaderSource("fragment.glsl").c_str());

  // Reserve for 1000 objects to avoid reallocation
  objects.reserve(1000);

  Object3D ship;
  ship.meshData.LoadFromObjectFile("src/VideoShip.obj");
  SetObjDefaults(ship);
  objects.push_back(ship);
  InitializeObjectGPU(ship);

  // Shoutout to tobi :3
  Object3D dickMaster;
  dickMaster.meshData.LoadFromObjectFileNew("src/flower.obj");
  SetObjDefaults(dickMaster);
  dickMaster.properties["name"] = "Dick Master :3";
  dickMaster.position = {20.0f, 0.0f, 0.0f};
  dickMaster.scale = {0.1f, 0.1f, 0.1f};
  objects.push_back(dickMaster);
  InitializeObjectGPU(dickMaster);

  Object3D testCubeCCW;
  testCubeCCW.meshData = CreateTestCubeCCW();
  SetObjDefaults(testCubeCCW);
  testCubeCCW.position = {0.0f, 5.0f, 0.0f};
  objects.push_back(testCubeCCW);
  InitializeObjectGPU(testCubeCCW);

  // 7. Framebuffer (FBO) Initialization
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Color Texture Attachment
  glGenTextures(1, &textureColorBuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)targetWindowWidth,
               (int)targetWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureColorBuffer, 0);

  // Depth/Stencil Renderbuffer Attachment
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        (int)targetWindowWidth, (int)targetWindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);

  // CRITICAL: Check Completeness before proceeding
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default screen

  SetupScreenQuad();
  SDL_SetWindowFullscreen(window, true);

  CalculateScreenTransforms(window);
  CalculateScreenProjection();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  io.ConfigFlags |=
      ImGuiConfigFlags_ViewportsEnable; // Optional: Enable Multi-Viewport

  SDL_SetWindowRelativeMouseMode(window, true);

  gruvboxDark();

  if (argc > 1) {
    std::string_view arg = argv[1];

    if (arg == "--test" || arg == "-t") {
      return RunUnitTests();
    } else {
      std::cout << "\nUnknown argument: " << arg << "\n";
      return 1;
    }
  }

  // glPolygonMode(GL_FRONT, GL_LINE);
  // glPolygonMode(GL_BACK, GL_FILL);

  std::cout << "\nInit done!";

  is_running = true;
  SDL_Event event;

  while (is_running) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) {
        is_running = false;
      }
      if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        CalculateScreenTransforms(window);
        CalculateScreenProjection();
      }

      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_F11) {
          if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
            SDL_SetWindowFullscreen(window, 0);
            CalculateScreenTransforms(window);
            CalculateScreenProjection();
          } else {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            CalculateScreenTransforms(window);
            CalculateScreenProjection();
          };
        };
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
          paused = !paused;
        }
        if (event.key.scancode == SDL_SCANCODE_GRAVE) {
          debugModeTogggled = !debugModeTogggled;
          shouldAllowMove = !shouldAllowMove;
        };
        if (event.key.scancode == SDL_SCANCODE_F6) {
          if (cullingMode == "GL_BACK") {
            cullingMode = "GL_FRONT";
            std::cout << cullingMode;
            glCullFace(GL_FRONT);
          } else if (cullingMode == "GL_FRONT") {
            cullingMode = "GL_FRONT_AND_BACK";
            std::cout << cullingMode;
            glCullFace(GL_FRONT_AND_BACK);
          } else if (cullingMode == "GL_FRONT_AND_BACK") {
            cullingMode = "GL_BACK";
            std::cout << cullingMode;
            glCullFace(GL_BACK);
          }
        }
        if (event.key.scancode == SDL_SCANCODE_F7) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (event.key.scancode == SDL_SCANCODE_F8) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (event.key.scancode == SDL_SCANCODE_V) {
          shouldAllowMove = !shouldAllowMove;
        }
      }

      if (shouldAllowMove) {
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
          fYaw += event.motion.xrel * fMouseSensitivity;
          fPitch -= event.motion.yrel * fMouseSensitivity;

          // Clamp pitch to avoid flipping
          if (fPitch > fMaxPitch)
            fPitch = fMaxPitch;
          if (fPitch < -fMaxPitch)
            fPitch = -fMaxPitch;
        }
      };
    }

    int timeLoc = glGetUniformLocation(mainShaderProgram, "uTime");
    glUseProgram(mainShaderProgram);
    glUniform1f(timeLoc, secondTiming); // pass time to shader

    // 1. Update delta time
    CalculateDeltaTime();

    // 2. Limit frame rate
    float remainingFrameTime = (1.0f / targetFrameRate) - deltaTime;
    if (remainingFrameTime > 0.001f) {
      SDL_Delay(int(remainingFrameTime * 1000.0f));
    }

    // 3. Now modify deltatime with deltatimemod to allow for timewarping
    // without affecting framerate
    newDeltaTime = deltaTime * deltaTimeMod;

    // 3. Process keyboard input for camera movement
    const bool *key_states = SDL_GetKeyboardState(NULL);
    float fMoveSpeed = 8.0f * deltaTime;
    if (shouldAllowMove) {
      // Forward / Back
      if (key_states[SDL_SCANCODE_W])
        vCamera = Vector_Add(
            vCamera,
            Vector_Mul(Vector_Normalise({vLookDir.x, 0.0f, vLookDir.z}),
                       fMoveSpeed));
      if (key_states[SDL_SCANCODE_S])
        vCamera = Vector_Sub(
            vCamera,
            Vector_Mul(Vector_Normalise({vLookDir.x, 0.0f, vLookDir.z}),
                       fMoveSpeed));

      // Right / Left
      vec3d vRight = Vector_Normalise(Vector_CrossProduct(vLookDir, {0, 1, 0}));
      if (key_states[SDL_SCANCODE_D])
        vCamera = Vector_Sub(vCamera, Vector_Mul(vRight, fMoveSpeed));
      if (key_states[SDL_SCANCODE_A])
        vCamera = Vector_Add(vCamera, Vector_Mul(vRight, fMoveSpeed));

      // Up / Down
      if (key_states[SDL_SCANCODE_SPACE])
        vCamera.y += 8.0f * deltaTime;
      if (key_states[SDL_SCANCODE_LSHIFT])
        vCamera.y -= 8.0f * deltaTime;
    }

    // 4. Update camera orientation based on mouse look
    vec3d vForward;
    if (!debugModeTogggled) {
      vForward = {cosf(fPitch) * sinf(fYaw), sinf(fPitch),
                  cosf(fPitch) * cosf(fYaw)};
    }

    if (!shouldAllowMove) {
      if (SDL_GetWindowMouseGrab(window)) {
        SDL_SetWindowMouseGrab(window, false);
      };
      if (SDL_GetWindowRelativeMouseMode(window)) {
        SDL_SetWindowRelativeMouseMode(window, false);
      };
    } else {
      if (!SDL_GetWindowMouseGrab(window)) {
        SDL_SetWindowMouseGrab(window, true);
      };
      if (!SDL_GetWindowRelativeMouseMode(window)) {
        SDL_SetWindowRelativeMouseMode(window, true);
      };
    }

    vLookDir = Vector_Normalise(vForward);

    for (unsigned long i = 0; i < objects.size(); i++) {
      objects[i].rotation.x =
          objects[i].rotation.x + objects[i].rotationPerTick.x * newDeltaTime;
      objects[i].rotation.y =
          objects[i].rotation.y + objects[i].rotationPerTick.y * newDeltaTime;
      objects[i].rotation.z =
          objects[i].rotation.z + objects[i].rotationPerTick.z * newDeltaTime;

      objects[i].position.x =
          objects[i].position.x + objects[i].positionPerTick.x * newDeltaTime;
      objects[i].position.y =
          objects[i].position.y + objects[i].positionPerTick.y * newDeltaTime;
      objects[i].position.z =
          objects[i].position.z + objects[i].positionPerTick.z * newDeltaTime;
    };

    // 5. Update view matrix
    vec3d vUp = {0, 1, 0};
    matView = Matrix_PointAt(vCamera, Vector_Add(vCamera, vLookDir), vUp);
    matView = Matrix_QuickInverse(matView);

    // --- RENDERING PHASE ---
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear both Color and Depth
    glViewport(0, 0, targetWindowWidth, targetWindowHeight);

    RenderObjectModernViaID(ship, 0, mainShaderProgram, matView, matProj, GL_CW, GL_BACK);
    RenderObjectModernViaID(dickMaster, 1, mainShaderProgram, matView, matProj, GL_CW, GL_BACK);
    RenderObjectModernViaID(testCubeCCW, 2, mainShaderProgram, matView, matProj, GL_CW, GL_BACK);
	
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

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

    if (debugModeTogggled) {
      DrawObjectEditor(objects);
    };

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // 8. Present final frame
    // SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SwapWindow(window);

    // 9. Update real frame rate
    realFrameRate = 1.0f / deltaTime;

    // 11. Increment draw cycles
    nDrawCycles++;

    MinuteTimer();
  }

  SDL_Quit();
  return 0;
}
