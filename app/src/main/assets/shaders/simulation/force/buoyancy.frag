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

    ivec3 ipos = ivec3(gl_FragCoord.xy, depth);
    float temp = texelFetch(temperature, ipos, 0);
    float v = texelFetch(velocity, ipos, 0);

    outColor = v + (alpha * temp * vec3(0.0f, 1.0f, 0.0f) * dt);
}
