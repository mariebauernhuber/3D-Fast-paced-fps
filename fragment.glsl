//fragment.glsl
#version 330 core
out vec4 FragColor; // The output variable for pixel color

void main() {
    // RGBA format (Red, Green, Blue, Alpha)
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); 
if (gl_FrontFacing) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green for front faces
    } else {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for back faces
    };
}
