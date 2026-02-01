#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform int uUseTexture = 1;  // 1=use texture, 0=use color

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    if(uUseTexture == 0){
	    FragColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);
    }else{
	FragColor = texColor;
    }
}
