#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform int depth;

out vec3 outData;

void main() {
   int direction = depth == 0 ? 1 : -1;
   ivec3 position = ivec3(gl_FragCoord.xy, depth + direction);

   outData = texelFetch(data_field, position, 0).xyz;
   //outData = vec3(1.0f);
}