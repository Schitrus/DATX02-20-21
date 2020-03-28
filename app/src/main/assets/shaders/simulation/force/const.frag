#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D target_field;
layout(binding = 1) uniform sampler3D source_field;

uniform float dt;
uniform int depth;

out float outValue;

void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    float target = texelFetch(target_field, position, 0).x;
    float source = texelFetch(source_field, position, 0).x;

    outValue = (source == 0.0f) ? target : source;
}