#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D pressure_field;
layout(binding = 1) uniform sampler3D velocity_field;

uniform int depth;
// Distance between each grid element. Assumes that grid elements are cubical such that dh can be applied to all three axis
uniform float dh;

out vec3 outVelocity;

// Performs gradient subtraction part of the projection step
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    // Vectors used for adding/subtracting pressure vectors
    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    float x = texelFetch(pressure_field, position + dx, 0).x - texelFetch(pressure_field, position - dx, 0).x;
    float y = texelFetch(pressure_field, position + dy, 0).x - texelFetch(pressure_field, position - dy, 0).x;
    float z = texelFetch(pressure_field, position + dz, 0).x - texelFetch(pressure_field, position - dz, 0).x;
    vec3 gradient_pressure = 0.5 * vec3(x,y,z) / dh;

    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    outVelocity = velocity - gradient_pressure;
}
