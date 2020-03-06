#version 310 es
precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int width;
uniform int height; 
uniform int depth; 
uniform float scale; 
out float outColor;
void main() {
    ivec2 tcoord = ivec2(gl_FragCoord.xy); 
    float value = 0.0f;
    if(tcoord.x == 0){ value += scale * texelFetch(data , ivec3( tcoord.x + 1, tcoord.y, depth),0).x;}
    if(tcoord.x == width -1){ value += scale * texelFetch(data , ivec3( tcoord.x - 1, tcoord.y, depth),0).x;}
    if(tcoord.y == 0){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y + 1, depth ),0).x;}
    if(tcoord.y == height -1){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y - 1, depth),0).x;}
    bool corner = ((tcoord.x == 0 || tcoord.x == width -1) && (tcoord.y == 0 || tcoord.y == height -1 )); 
    if(corner){ value *= 0.5f; }
    outColor = value;
}