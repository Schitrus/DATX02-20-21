#version 310 es

precision highp sampler3D;

layout(binding = 0) uniform sampler3D target;
layout(binding = 1) uniform sampler3D source;

uniform float dt;
uniform int depth;

out vec3 outColor;

void main() {
    ivec3 ipos = ivec3(gl_FragCoord.xy, depth);

    vec3 value = texelFetch(target, ipos, 0).xyz;
    vec3 sourceValue = texelFetch(source, ipos, 0).xyz;

    outColor = value + dt*sourceValue;
}