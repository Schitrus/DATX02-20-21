#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D pressure_field;
layout(binding = 1) uniform sampler3D divergence_field;

uniform int depth;

out float outPressure;

// Performs jacobi iteration to approximate solution to pressure equation
void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    // Vectors used for adding/subtracting pressure vectors
    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    // Get value of divergence field at position
    float divergence = texelFetch(divergence_field, position, 0).x;

    // Use jacobi iteration formula
    float pressure;
    pressure  = texelFetch(pressure_field, position - dx, 0).x;
    pressure += texelFetch(pressure_field, position + dx, 0).x;
    pressure += texelFetch(pressure_field, position - dy, 0).x;
    pressure += texelFetch(pressure_field, position + dy, 0).x;
    pressure += texelFetch(pressure_field, position - dz, 0).x;
    pressure += texelFetch(pressure_field, position + dz, 0).x;

    outPressure = (pressure - divergence) / 6.0f;
}
