#version 320 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D old_data_field;
layout(binding = 1) uniform sampler3D data_field;

uniform float dt;
uniform float diffusion_constant;
uniform int depth;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    vec3 gridSize = vec3(16.0);

    float a = dt*diffusion_constant;

    vec3 data;
    data  = texelFetch(data_field, position - dx, 0).xyz;
    data += texelFetch(data_field, position + dx, 0).xyz;
    data += texelFetch(data_field, position - dy, 0).xyz;
    data += texelFetch(data_field, position + dy, 0).xyz;
    data += texelFetch(data_field, position - dz, 0).xyz;
    data += texelFetch(data_field, position + dz, 0).xyz;

    data *= a;
    data += texelFetch(old_data_field, position, 0).xyz;

    outData = data / (1.0f + (6.0f * a));
}
