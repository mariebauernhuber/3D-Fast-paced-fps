#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 uMVP;
layout(location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
};

//void main() {
//    gl_Position = uMVP * vec4(aPos, 1.0);
//    vTexCoord = aTexCoord;
//}
