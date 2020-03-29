#version 320 es
layout(location = 0) in vec3 pos;
uniform mat4 mvp;
out vec3 hit;
void main() {
    gl_Position = mvp * vec4(pos, 1.0);
    hit = pos;
}