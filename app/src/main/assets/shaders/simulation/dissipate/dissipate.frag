#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform float dt;
uniform float dissipation_rate;
uniform int depth;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    vec3 data = texelFetch(data_field, position, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position + dx, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position - dx, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position + dy, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position - dy, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position + dz, 0).xyz;
    data += dissipation_rate * texelFetch(data_field, position - dz, 0).xyz;

    outData = data / (1.0f + (6.0 * dt * dissipation_rate)) ;
}
