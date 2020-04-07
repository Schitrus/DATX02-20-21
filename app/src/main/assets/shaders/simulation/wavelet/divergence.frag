#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D texture_field;

uniform int depth;

out float outDivergence;

// Performs the divergence part of the projection step
void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    float x = texelFetch(texture_field, position + dx, 0).x - texelFetch(velocity_field, position - dx, 0).x;
    float y = texelFetch(texture_field, position + dy, 0).y - texelFetch(velocity_field, position - dy, 0).y;
    float z = texelFetch(texture_field, position + dz, 0).z - texelFetch(velocity_field, position - dz, 0).z;

    outDivergence = -0.5f * (x + y + z);
}