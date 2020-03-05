#version 310 es\n
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
out vec2 tex;
void main() {
  tex = uv;
  gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}