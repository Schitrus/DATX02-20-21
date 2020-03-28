#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D data_field;

uniform float dt;
uniform int depth;
uniform vec3 gridSize;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    vec3 previous_position = vec3(position) + vec3(0.5) - dt * velocity;

    // Note: Linear interpolation due to linear texture
    outData = texture(data_field, previous_position / gridSize).xyz;
}

