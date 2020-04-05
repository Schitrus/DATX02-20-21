#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D x_field; // x vector (Ax = b)
layout(binding = 1) uniform sampler3D b_field; // b vector (Ax = b)

uniform int depth;
uniform float alpha; // constant in jacobi formula
uniform float beta; // constant in jacobi formula

out vec3 outData;

// Performs jacobi iteration to approximate solution to pressure equation
void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    // Vectors used for adding/subtracting pressure vectors
    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    // Use jacobi iteration formula
    vec3 data;
    data  = texelFetch(x_field, position - dx, 0).xyz;
    data += texelFetch(x_field, position + dx, 0).xyz;
    data += texelFetch(x_field, position - dy, 0).xyz;
    data += texelFetch(x_field, position + dy, 0).xyz;
    data += texelFetch(x_field, position - dz, 0).xyz;
    data += texelFetch(x_field, position + dz, 0).xyz;
    data += alpha * texelFetch(b_field, position, 0).xyz;

    outData = data / beta;
}
