#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D source;

out vec3 outValue;

void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    outValue = texelFetch(source_field, position, 0).rgb;
}