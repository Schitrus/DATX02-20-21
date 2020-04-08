#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;

uniform int depth;

out float outEnergy;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity_vector = texelFetch(velocity_field, position, 0).xyz;

    float velocity = length(velocity_vector);

    float energy = (1.0/2.0) * velocity * velocity;

    outEnergy = pow(2.0, -(5.0/6.0)) * energy;

}