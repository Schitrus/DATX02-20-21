#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D turbulence_field;
layout(binding = 2) uniform sampler3D texture_field;
layout(binding = 3) uniform sampler3D energy_field;

uniform vec3 gridSize;
uniform int depth;

out vec3 outVelocity;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity         = texture(velocity_field, vec3(position)/gridSize).xyz;

    vec3 texture_coord    = texture(texture_field, vec3(position)/gridSize).xyz;

    vec3 turbulence      = texture(turbulence_field, vec3(position)/gridSize).xyz;

    float energy_spectrum = texture(energy_field, vec3(position)/gridSize).x;

    outVelocity = velocity;// + energy_spectrum * turbulence;

}