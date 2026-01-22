#include "../include/shader-utils.hpp"
#include <string>
#include <iostream>
#include <fstream>

GLuint quadVAO, quadVBO;
GLuint screenShaderProgram;

const float quadVertices[24] = {  // 6 verts * 4 floats
    // First triangle
    -1.0f, -1.0f,  0.0f, 0.0f,  // Bottom-left
     1.0f,  1.0f,  1.0f, 1.0f,  // Top-right
    -1.0f,  1.0f,  0.0f, 1.0f,  // Top-left
    // Second triangle
    -1.0f, -1.0f,  0.0f, 0.0f,  // Bottom-left
     1.0f, -1.0f,  1.0f, 0.0f,  // Bottom-right
     1.0f,  1.0f,  1.0f, 1.0f   // Top-right
};

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

std::string LoadShaderSource(const char* filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(filePath));
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg); // Reset to the beginning

    std::string buffer(size, '\0');
    if (file.read(&buffer[0], size)) {
        return buffer;
    }
    
    return "";
}

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
	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
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
