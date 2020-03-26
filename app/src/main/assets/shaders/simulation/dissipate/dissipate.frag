#version 320 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D pressure_field;

uniform float dt;
uniform float dissipation_rate;
uniform int depth;

out float outPressure;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    float pressure = texelFetch(pressure_field, position, 0).x;

    outPressure = pressure / (1.0f + (dt * dissipation_rate)) ;
}
