//fragment.glsl
#version 330 core
out vec4 FragColor; // The output variable for pixel color
uniform float uTime;

void main() {
 float hue = fract(uTime * 0.5);  // 0.5 = speed (adjust as needed)

    // Convert hue to RGB (simple approximation)
    vec3 rgb = vec3(
        abs(hue * 6.0 - 3.0) - 1.0,
        2.0 - abs(hue * 6.0 - 2.0),
        2.0 - abs(hue * 6.0 - 4.0)
    );
    rgb = clamp(rgb, 0.0, 1.0);

    FragColor = vec4(rgb, 1.0);  // Full opacity
}
