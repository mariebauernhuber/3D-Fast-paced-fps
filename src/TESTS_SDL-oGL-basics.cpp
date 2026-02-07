#include "mesh.hpp"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <catch2/catch.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>
#include <array>
#include <cmath>
#include <thread>
#include <chrono>

struct SDLTestFixture {
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    
    SDLTestFixture() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        
        window = SDL_CreateWindow("Catch2 OpenGL3 SDL3 Tests",
                                SDL_WINDOWPOS_CENTERED | SDL_WINDOWPOS_CENTERED |
                                800, 600, SDL_WINDOW_OPENGL);
        context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, context);
        SDL_GL_SetSwapInterval(1);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        REQUIRE(window != nullptr);
        REQUIRE(context != nullptr);
    }
    
    ~SDLTestFixture() {
        if (context) SDL_GL_DestroyContext(context);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

static GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        INFO("Shader compilation failed: " << infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint createShaderProgram(const char* vsSource, const char* fsSource) {
    GLuint program = glCreateProgram();
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        INFO("Program linking failed: " << infoLog);
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

TEST_CASE_METHOD(SDLTestFixture, "SDL3 Window Creation", "[sdl3][window]") {
    SECTION("Basic window attributes") {
	SDL_SetWindowSize(window, 800, 600);
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        REQUIRE(w == 800);
        REQUIRE(h == 600);
        
        SDL_SetWindowSize(window, 1024, 768);
        SDL_GetWindowSize(window, &w, &h);
        REQUIRE(w == 1024);
        REQUIRE(h == 768);
    }
    
    SECTION("Window position and visibility") {
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_HideWindow(window);
        SDL_ShowWindow(window);
        SDL_MaximizeWindow(window);
        SDL_Delay(100);
        SDL_RestoreWindow(window);
    }
    
    SECTION("Window flags") {
        SDL_WindowFlags flags = SDL_GetWindowFlags(window);
        REQUIRE((flags & SDL_WINDOW_OPENGL) != 0);
        REQUIRE((flags & SDL_WINDOW_RESIZABLE) == 0);
        
        SDL_SetWindowResizable(window, true);
        flags = SDL_GetWindowFlags(window);
        REQUIRE((flags & SDL_WINDOW_RESIZABLE) != 0);
    }
}

TEST_CASE_METHOD(SDLTestFixture, "SDL3 OpenGL Context", "[opengl][context]") {
    SECTION("GL version and capabilities") {
        const GLubyte* version = glGetString(GL_VERSION);
        INFO("OpenGL Version: " << version);
        REQUIRE(version != nullptr);
        
        const GLubyte* vendor = glGetString(GL_VENDOR);
        INFO("OpenGL Vendor: " << vendor);
        
        const GLubyte* renderer = glGetString(GL_RENDERER);
        INFO("OpenGL Renderer: " << renderer);
    }
    
    SECTION("Viewport and scissor") {
        glViewport(0, 0, 800, 600);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        REQUIRE(viewport[2] == 800);
        REQUIRE(viewport[3] == 600);
        
        glScissor(100, 100, 200, 150);
        glEnable(GL_SCISSOR_TEST);
        glDisable(GL_SCISSOR_TEST);
    }
    
    SECTION("Clear buffers") {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearDepth(1.0);
        glClearStencil(0);
    }
}

TEST_CASE_METHOD(SDLTestFixture, "SDL3 Input Handling", "[sdl3][input]") {
    SECTION("Keyboard events") {
        SDL_Event event;
        const Uint32 start = SDL_GetTicks();
        int keyPressCount = 0;
        
        while (SDL_GetTicks() - start < 500) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    keyPressCount++;
                    INFO("Key pressed: " << (int)event.key.scancode);
                }
            }
            SDL_Delay(10);
        }
        // Note: This is interactive - press keys during test execution
        INFO("Key presses detected: " << keyPressCount);
    }
    
    SECTION("Mouse events") {
	SDL_SetWindowRelativeMouseMode(window, true);
	REQUIRE(SDL_GetWindowRelativeMouseMode(window));
	SDL_SetWindowRelativeMouseMode(window, false);
	REQUIRE(!SDL_GetWindowRelativeMouseMode(window));
    }
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 Shaders and Programs", "[opengl][shaders]") {
    const char* vsSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 ourColor;
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            ourColor = aColor;
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fsSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 ourColor;
        in vec2 TexCoord;
        
        uniform sampler2D texture1;
        uniform sampler2D texture2;
        uniform float mixValue;
        
        void main() {
            FragColor = mix(texture(texture1, TexCoord), 
                           texture(texture2, TexCoord), mixValue) * vec4(ourColor, 1.0);
        }
    )";
    
    GLuint program = createShaderProgram(vsSource, fsSource);
    REQUIRE(program != 0);
    
    GLint modelLoc = glGetUniformLocation(program, "model");
    GLint viewLoc = glGetUniformLocation(program, "view");
    GLint projLoc = glGetUniformLocation(program, "projection");
    REQUIRE(modelLoc != -1);
    REQUIRE(viewLoc != -1);
    REQUIRE(projLoc != -1);
    
    glDeleteProgram(program);
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 VAO VBO EBO", "[opengl][buffers]") {
    // Triangle data
    std::array<GLfloat, 32> vertices = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
    
    std::array<GLuint, 6> indices = { 0, 1, 3, 1, 2, 3 };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 Textures", "[opengl][texture]") {
    GLuint texture1, texture2;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);
    
    // Texture 1
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generate 4x4 texture with gradients
    std::vector<GLubyte> pixels(4*4*4);
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            int idx = (y*4 + x) * 4;
            pixels[idx + 0] = 255 * (x / 3.0f); // R
            pixels[idx + 1] = 255 * (y / 3.0f); // G
            pixels[idx + 2] = 128;              // B
            pixels[idx + 3] = 255;              // A
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Verify texture parameters
    GLint wrapS;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    REQUIRE(wrapS == GL_REPEAT);
    
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 Framebuffer", "[opengl][fbo]") {
    GLuint fbo, rbo, textureColorBuffer;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1, &textureColorBuffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Color attachment
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    
    // Depth/stencil attachment
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    REQUIRE(status == GL_FRAMEBUFFER_COMPLETE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &textureColorBuffer);
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 Uniform Buffer Objects", "[opengl][ubo]") {
    GLuint uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    
    glDeleteBuffers(1, &uboMatrices);
}

TEST_CASE_METHOD(SDLTestFixture, "SDL3 Timer and Timing", "[sdl3][timing]") {
    Uint32 start = SDL_GetTicks();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Uint32 end = SDL_GetTicks();
    REQUIRE(end - start >= 90); // Allow some tolerance
    
    Uint64 perfStart = SDL_GetPerformanceCounter();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    Uint64 perfEnd = SDL_GetPerformanceCounter();
    REQUIRE((perfEnd - perfStart) >= SDL_GetPerformanceFrequency() / 20);
}

TEST_CASE_METHOD(SDLTestFixture, "OpenGL3 Blend and Depth", "[opengl][state]") {
    SECTION("Blending") {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_BLEND);
    }
    
    SECTION("Depth testing") {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);
    }
    
    SECTION("Stencil testing") {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glDisable(GL_STENCIL_TEST);
    }
    
    SECTION("Cull face") {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDisable(GL_CULL_FACE);
    }
}
