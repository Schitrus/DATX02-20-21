#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D pressure;
layout(binding = 1) uniform sampler3D velocity;
uniform int depth;
out vec3 outColor;


void main() {

    float x = texelFetch(pressure ,ivec3( tcoord.x + 1, tcoord.y, depth), 0).x - texelFetch(pressure ,ivec3( tcoord.x - 1, tcoord.y, depth), 0).x;
    float y = texelFetch(pressure ,ivec3( tcoord.x, tcoord.y + 1, depth), 0).x - texelFetch(pressure ,ivec3( tcoord.x, tcoord.y -1, depth), 0).x;
    float z = texelFetch(pressure ,ivec3( tcoord.x, tcoord.y, depth +1 ), 0).x - texelFetch(pressure ,ivec3( tcoord.x, tcoord.y, depth -1 ), 0).x;
    vec3 gradP = 0.5*vec3(x,y,z);

    vec3 vOld = texelFetch(velocity, tcoord, 0);
    vec3 vNew = vOld - gradP;

    outColor = vNew;
}
