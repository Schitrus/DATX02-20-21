#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D target_field;

uniform float dt;
uniform int depth;
uniform float wind_angle;
uniform float wind_strength;

out vec3 outValue;

void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 target = texelFetch(target_field, position, 0).xyz;

    outValue = target + dt*vec3(wind_strength*cos(wind_angle), 0, wind_strength*sin(wind_angle));
}