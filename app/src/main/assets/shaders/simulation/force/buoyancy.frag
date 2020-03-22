#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D temperature;
layout(binding = 1) uniform sampler3D velocity;

uniform float dt;
uniform float alpha;
uniform int depth;

out vec3 outColor;

void main() {

    float Tair = 0.0;

    ivec3 ipos = ivec3(gl_FragCoord.xy, depth);
    float temp = texelFetch(temperature, ipos, 0).x;
    vec3 v = texelFetch(velocity, ipos, 0).xyz;

    outColor = v + (alpha * (temp - Tair) * vec3(0.0f, 1.0f, 0.0f) * dt);
}
