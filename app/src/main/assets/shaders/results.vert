#version 310 es
layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 tex;
out vec2 texcoord;
void main() {
    texcoord =  tex;
    gl_Position = vec4(pos, 1.0);
}