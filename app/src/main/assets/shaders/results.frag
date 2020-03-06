#version 310 es
precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D result;
in vec2 texcoord;
out vec4 outColor;
void main() {
vec3 coords = vec3(texcoord, 0.5f);
    float q = texture(result, coords).x;
    float c = q / 1.0f;
    outColor = vec4(c, c, c, 1.0);
}