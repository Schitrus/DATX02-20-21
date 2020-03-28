#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D temperature_field;

uniform float dt;
uniform int depth;
uniform vec3 gridSize;

out float outTemperature;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    vec3 previous_position = vec3(position) + vec3(0.5) - dt * velocity;

    float temperature = texture(temperature_field, previous_position / gridSize).x;

    float ambient_temperature = 0.0f;
    float max_temperature = 3000.0f - 273.15f;
    float temperature_loss = pow((temperature - ambient_temperature) / (max_temperature - ambient_temperature), 4.0f);

    outTemperature = temperature - dt * 3000.0 * temperature_loss;
}