#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D turbulence_field;

uniform vec3 gridSize;
uniform int depth;
uniform float band;
uniform float min_band;
uniform float dissipation_rate;

out float outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity = texture(velocity_field, vec3(position)/gridSize).xyz;
    float turbulence = texture(turbulence_field, vec3(position)/gridSize).x;
    float energy_spectrum = kolmogorov * pow(dissipation_rate, (2.0/3.0)) * pow(gridSize/2.0, -(5.0/3.0));

    velocity += pow(2.0, -(5.0/6.0)) * energy_spectrum * turbulence;

}