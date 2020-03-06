#version 310 es
precision highp float;
in vec3 hit;
// Hello there
out vec4 outColor;
void main(){
  outColor = vec4(hit, 1.0f);
}