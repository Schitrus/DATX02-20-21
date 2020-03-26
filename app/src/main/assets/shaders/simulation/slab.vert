#version 320 es
layout(location = 0) in vec3 pos;

//The bare minimum vertex shader, used together with various slab operation shaders
void main(){
    gl_Position = vec4(pos, 1.0f);
}