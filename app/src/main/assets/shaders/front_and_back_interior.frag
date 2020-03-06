#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int depth;
out float outColor;
void main() {
   int dir = depth == 0 ? 1 : -1;   // todo remove if statement
   ivec2 tcoord = ivec2(gl_FragCoord.xy);
   float value = texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth + dir),0).x;
   outColor = value;
}