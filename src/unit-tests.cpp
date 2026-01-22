#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>

struct TestContext {
    GLFWwindow* window = nullptr;
    GLuint fbo = 0, rbo = 0, colorTex = 0, vao = 0, vbo = 0, ebo = 0;
    GLuint shaderProgram = 0;
    int width = 128, height = 128;
    unsigned char pixel[4] = {0};
    
    TestContext() {
        glfwInit();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(width, height, "UnitTests", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        
        glewInit();
        
        setupFBO();
        setupQuad();
        setupShader();
    }
    
    ~TestContext() {
        glDeleteProgram(shaderProgram);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);
        glDeleteTextures(1, &colorTex);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
private:
    void setupFBO() {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        
        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "FBO incomplete: " << status << std::endl;
        }
    }
    
    void setupQuad() {
        // CCW quad facing camera (z=-1), colored red
        const float vertices[] = {
            -0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f,  // BL 0
             0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f,  // BR 1
             0.5f,  0.5f, -1.0f, 1.0f, 0.0f, 0.0f,  // TR 2
            -0.5f,  0.5f, -1.0f, 1.0f, 0.0f, 0.0f   // TL 3
        };
        const unsigned indices[] = {0,1,2, 0,2,3};  // CCW
        
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    GLuint createShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compile error: " << infoLog << std::endl;
        }
        return shader;
    }
    
    void setupShader() {
        const char* vsSource = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
out vec3 vColor;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
}
)";
        const char* fsSource = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";
        
        shaderProgram = glCreateProgram();
        GLuint vs = createShader(GL_VERTEX_SHADER, vsSource);
        GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSource);
        
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs);
        glLinkProgram(shaderProgram);
        
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    
public:
    void renderQuad(bool useCW = false) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_DEPTH_TEST);
        glUseProgram(shaderProgram);
        
        // Simple orthographic projection (quad fills screen)
        float proj[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, -2.0f, 1.0f
        };
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, proj);
        
        glBindVertexArray(vao);
        if (useCW) {
            // Reverse triangle order for CW winding (0-2-1, 0-3-2)
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3));
        } else {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
    
    bool readCenterPixel() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(width/2, height/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        return pixel[0] > 50;  // Red > 50/255 threshold
    }
};

int test_ccw_front_default(TestContext& ctx) {
    std::cout << "Test 1: CCW front, cull BACK (default) - ";
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    ctx.renderQuad(false);
    bool passed = ctx.readCenterPixel();
    std::cout << (passed ? "PASS" : "FAIL") << std::endl;
    return passed ? 0 : 1;
}

int test_ccw_front_culled(TestContext& ctx) {
    std::cout << "Test 2: CCW front, cull FRONT - ";
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    
    ctx.renderQuad(false);
    bool passed = !ctx.readCenterPixel();
    std::cout << (passed ? "PASS" : "FAIL") << std::endl;
    return passed ? 0 : 1;
}

int test_cw_front_corrected(TestContext& ctx) {
    std::cout << "Test 3: CW vertices, glFrontFace(CW), cull BACK - ";
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    ctx.renderQuad(true);
    bool passed = ctx.readCenterPixel();
    std::cout << (passed ? "PASS" : "FAIL") << std::endl;
    return passed ? 0 : 1;
}

int RunUnitTests() {
    std::cout << "\n\n=== OpenGL Face Culling Unit Tests ===\n";
    
    TestContext ctx;
    if (!ctx.window) {
        std::cerr << "Failed to create GL context\n";
        return 1;
    }
    
    int failures = 0;
    failures += test_ccw_front_default(ctx);
    failures += test_ccw_front_culled(ctx);
    failures += test_cw_front_corrected(ctx);
    
    std::cout << "\nTests completed. Failures: " << failures << "/3\n";
    return failures;
}
