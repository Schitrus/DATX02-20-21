#version 310 es
precision highp float;
in vec3 hit;
out vec4 outColor;
void main(){
    /*hello*/
    outColor = vec4(hit, 1.0f);
}