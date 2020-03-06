#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int depth; 
out float outColor;
void main() {
   ivec2 tcoord = ivec2(gl_FragCoord.xy); 
   float value = texelFetch(data ,ivec3( tcoord.x - 1, tcoord.y, depth),0).x;
   value += texelFetch(data ,ivec3( tcoord.x + 1, tcoord.y, depth),0).x;
   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y - 1, depth),0).x;
   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y + 1, depth),0).x;
   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth +1 ),0).x;
   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth -1 ),0).x;
   outColor = value;
}