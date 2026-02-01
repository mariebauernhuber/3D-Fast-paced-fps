#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
void main(){
	//FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	//FragColor = vec4(ourColor, 1.0f);\n"
	FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
}
