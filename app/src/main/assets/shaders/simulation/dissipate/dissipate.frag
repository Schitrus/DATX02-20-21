#version 320 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform float dt;
uniform float dissipation_rate;
uniform int depth;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 data = texelFetch(data_field, position, 0).xyz;

    outData = data / (1.0f + (dt * dissipation_rate)) ;
}
