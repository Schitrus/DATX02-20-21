#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D externalForce;

uniform float dt;
uniform int depth;

out vec3 outValue;

//Adds together the target texture with the source texture with a given dt
void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 target = texelFetch(velocity_field, position, 0).xyz;
    vec3 source = texelFetch(externalForce, position, 0).xyz;

    outValue = target + source;
}
