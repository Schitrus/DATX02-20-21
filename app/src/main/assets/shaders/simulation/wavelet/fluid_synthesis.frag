#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D turbulence_field;
layout(binding = 2) uniform sampler3D texture_field;
layout(binding = 3) uniform sampler3D energy_field;
layout(binding = 4) uniform sampler3D jacobianX;
layout(binding = 5) uniform sampler3D jacobianY;
layout(binding = 6) uniform sampler3D jacobianZ;

uniform vec3 gridSize;
uniform int depth;

out vec3 outVelocity;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity         = texture(velocity_field, (vec3(position) + vec3(0.5))/gridSize).xyz;

    vec3 texture_coord    = texture(texture_field, (vec3(position) + vec3(0.5))/gridSize).xyz;

    vec3 turbulence      = texture(turbulence_field, texture_coord).xyz;

    float energy_spectrum = texture(energy_field, (vec3(position) + vec3(0.5))/gridSize).x;

    mat3 jacobian;
    jacobian[0] = texture(jacobianX, (vec3(position) + vec3(0.5))/gridSize).xyz;
    jacobian[1] = texture(jacobianY, (vec3(position) + vec3(0.5))/gridSize).xyz;
    jacobian[2] = texture(jacobianZ, (vec3(position) + vec3(0.5))/gridSize).xyz;

    vec3 wavelet = velocity;

    if (determinant(jacobian) != 0.0f){
        wavelet = velocity + pow(2.0, (-5.0/6.0)) * energy_spectrum * turbulence * inverse(jacobian);;
    }
    outVelocity = wavelet;

}