#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity;
layout(binding = 1) uniform sampler3D data;

uniform float dt;
uniform int depth;
uniform vec3 gridSize;

out vec3 outColor;

void main() {

    vec3 ipos = floor(vec3(gl_FragCoord.xy, depth));
    vec3 pos = ipos / (gridSize - 1.0f);

    vec3 vel = texelFetch(velocity, ivec3(ipos), 0).xyz;

    pos -= dt * vel;

    outColor = texture(data, pos).xyz;
}

