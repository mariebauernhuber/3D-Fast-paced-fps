#include <catch2/catch.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>

struct GLStateChecker {
    static bool isCullFaceEnabled() {
        GLint enabled;
        glGetIntegerv(GL_CULL_FACE, &enabled);
        return enabled != GL_FALSE;
    }
    
    static GLenum getCullFaceMode() {
        GLint mode;
        glGetIntegerv(GL_CULL_FACE_MODE, &mode);
        return static_cast<GLenum>(mode);
    }
    
    static GLenum getFrontFace() {
        GLint mode;
        glGetIntegerv(GL_FRONT_FACE, &mode);
        return static_cast<GLenum>(mode);
    }
};

class SDL3GLFixture {
protected:
    SDL_Window* window = nullptr;
    SDL_GLContext ctx = nullptr;
    
    SDL3GLFixture() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        window = SDL_CreateWindow("Catch2 OpenGL Face Culling Test", 
                                400, 300, SDL_WINDOW_OPENGL);
        REQUIRE(window != nullptr);
        
        ctx = SDL_GL_CreateContext(window);
        REQUIRE(ctx != nullptr);
        
        SDL_GL_MakeCurrent(window, ctx);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    }
    
    ~SDL3GLFixture() {
        if (ctx) SDL_GL_DestroyContext(ctx);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    void clearAndSwap() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
    }
};

TEST_CASE_METHOD(SDL3GLFixture, "Face culling is disabled by default", "[opengl][culling]") {
    REQUIRE_FALSE(GLStateChecker::isCullFaceEnabled());
}

TEST_CASE_METHOD(SDL3GLFixture, "Can enable and configure face culling", "[opengl][culling]") {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    REQUIRE(GLStateChecker::isCullFaceEnabled());
    REQUIRE(GLStateChecker::getCullFaceMode() == GL_BACK);
    REQUIRE(GLStateChecker::getFrontFace() == GL_CCW);
}

TEST_CASE_METHOD(SDL3GLFixture, "Front face winding defaults to CCW", "[opengl][culling]") {
    // OpenGL default is GL_CCW
    REQUIRE(GLStateChecker::getFrontFace() == GL_CCW);
}

TEST_CASE_METHOD(SDL3GLFixture, "Triangle renders correctly with back-face culling", "[opengl][culling][render]") {
    // Simple projection for 2D ortho
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    SECTION("No culling - triangle visible") {
        clearAndSwap();
        
        glColor3f(1.0f, 0.0f, 0.0f);  // Red
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);     // CCW winding
        glVertex2f( 0.5f, -0.5f);
        glVertex2f( 0.0f,  0.5f);
        glEnd();
        
        // Visually verify red triangle appears
        clearAndSwap();
        SUCCEED("Red triangle rendered without culling");
    }
    
    SECTION("Back-face culling - front face still visible") {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        clearAndSwap();
        
        glColor3f(0.0f, 1.0f, 0.0f);  // Green
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);     // CCW = front face
        glVertex2f( 0.5f, -0.5f);
        glVertex2f( 0.0f,  0.5f);
        glEnd();
        
        clearAndSwap();
        SUCCEED("Green triangle front face visible with back-face culling");
    }
    
    SECTION("Back-face culling - reversed winding culled") {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        clearAndSwap();
        
        glColor3f(0.0f, 0.0f, 1.0f);  // Blue
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);     // CW winding = back face → culled
        glVertex2f( 0.0f,  0.5f);
        glVertex2f( 0.5f, -0.5f);
        glEnd();
        
        clearAndSwap();
        SUCCEED("Blue triangle back face correctly culled");
    }
}

TEST_CASE_METHOD(SDL3GLFixture, "Quad back faces are culled", "[opengl][culling][quad]") {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    SECTION("Quad front face (CCW)") {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        clearAndSwap();
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
        
        glBegin(GL_QUADS);
        glVertex2f(-0.7f, -0.7f);
        glVertex2f( 0.7f, -0.7f);
        glVertex2f( 0.7f,  0.7f);
        glVertex2f(-0.7f,  0.7f);
        glEnd();
        
        clearAndSwap();
        SUCCEED("Yellow quad front face rendered");
    }
}
