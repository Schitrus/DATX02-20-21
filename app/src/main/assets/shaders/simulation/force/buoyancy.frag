#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D temperature_field;
layout(binding = 1) uniform sampler3D velocity_field;

uniform float dt;
uniform int depth;
uniform float scale;

out vec3 outVelocity;

void main() {

    float ambient_temperature = 0.0f;

    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    float temperature = texelFetch(temperature_field, position, 0).x;
    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    vec3 vertical_direction = vec3(0.0f, 1.0f, 0.0f);

    vec3 buoyancy = (temperature - ambient_temperature) * vertical_direction;

    outVelocity = velocity + scale * buoyancy * dt;
}
