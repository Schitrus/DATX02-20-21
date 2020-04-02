#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform float dt;
uniform float diffusion_constant;
uniform float alpha;
uniform float beta;
uniform int depth;
uniform vec3 gridSize;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    vec3 data;
    data  = texelFetch(data_field, position - dx, 0).xyz;
    data += texelFetch(data_field, position + dx, 0).xyz;
    data += texelFetch(data_field, position - dy, 0).xyz;
    data += texelFetch(data_field, position + dy, 0).xyz;
    data += texelFetch(data_field, position - dz, 0).xyz;
    data += texelFetch(data_field, position + dz, 0).xyz;
    data += alpha * texelFetch(data_field, position, 0).xyz;

    outData = data / beta;
}
