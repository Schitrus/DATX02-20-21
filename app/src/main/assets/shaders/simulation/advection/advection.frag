#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D data_field;

uniform float dt;
uniform float dh;
uniform int depth;
uniform vec3 gridSize;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    vec3 previous_position = vec3(position) * dh;

    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    previous_position -= dt * velocity;

    previous_position /= (dh * (gridSize - 1.0f));

    outData = texture(data_field, previous_position).xyz;
}

