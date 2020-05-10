#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D noise1;
layout(binding = 1) uniform sampler3D noise2;
layout(binding = 2) uniform sampler3D noise3;

uniform vec3 gridSize;
uniform int depth;

out vec3 outData;

const ivec3 dx = ivec3(1, 0, 0);
const ivec3 dy = ivec3(0, 1, 0);
const ivec3 dz = ivec3(0, 0, 1);

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 wavelet;
    wavelet.x = 0.5 * ( (texelFetch(noise1, position+dy, 0).x - texelFetch(noise1, position-dy, 0).x)
                      - (texelFetch(noise2, position+dz, 0).x - texelFetch(noise2, position-dz, 0).x));
    wavelet.y = 0.5 * ( (texelFetch(noise3, position+dz, 0).x - texelFetch(noise3, position-dz, 0).x)
                      - (texelFetch(noise1, position+dx, 0).x - texelFetch(noise1, position-dx, 0).x));
    wavelet.z = 0.5 * ( (texelFetch(noise2, position+dx, 0).x - texelFetch(noise2, position-dx, 0).x)
                      - (texelFetch(noise3, position+dy, 0).x - texelFetch(noise3, position-dy, 0).x));

    outData = wavelet;
}