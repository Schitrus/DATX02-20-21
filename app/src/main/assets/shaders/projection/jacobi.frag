#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D pressure;
layout(binding = 1) uniform sampler3D div;
uniform int depth;
out float outColor;

float jacobi(){

    ivec2 tcoord = ivec2(gl_FragCoord.xy);

    float dC = texelFetch(div, ivec3(tcoord, depth), 0).x;

    float value = texelFetch(pressure ,ivec3( tcoord.x - 1, tcoord.y, depth), 0).x;
    value += texelFetch(pressure ,ivec3( tcoord.x + 1, tcoord.y, depth), 0).x;
    value += texelFetch(pressure ,ivec3( tcoord.x, tcoord.y - 1, depth), 0).x;
    value += texelFetch(pressure ,ivec3( tcoord.x, tcoord.y + 1, depth), 0).x;
    value += texelFetch(pressure ,ivec3( tcoord.x, tcoord.y, depth +1 ), 0).x;
    value += texelFetch(pressure ,ivec3( tcoord.x, tcoord.y, depth -1 ), 0).x;

    return (value - dC) / 6.0f;
}


void main() {
    outColor = jacobi();
}
