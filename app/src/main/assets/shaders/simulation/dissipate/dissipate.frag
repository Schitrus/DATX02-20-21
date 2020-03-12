#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D pressure;

uniform float dt;
uniform float aS;
uniform int depth;

out float outColor;

void main() {

    ivec3 ipos = ivec3(gl_FragCoord.xy, depth);

    float oldP = texelFetch(pressure, ipos, 0).x;

    outColor = oldP / (1.0f + (dt * aS)) ;
}
