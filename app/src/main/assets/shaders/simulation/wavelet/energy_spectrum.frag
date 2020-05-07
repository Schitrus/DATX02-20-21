#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;

uniform int depth;
uniform float meterToVoxels;  //conversion factor from meter to voxels

out float outEnergy;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity_vector = texelFetch(velocity_field, position, 0).xyz;

    float velocity = length(velocity_vector);

    float energy = 0.5 * velocity * velocity * meterToVoxels * meterToVoxels;

    outEnergy = energy * pow(2.0, -(5.0/3.0));

}