#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int depth;
out vec3 outColor;
void main() {
   int dir = depth == 0 ? 1 : -1;
   ivec2 tcoord = ivec2(gl_FragCoord.xy);

   outColor = texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth + dir),0).xyz;
}