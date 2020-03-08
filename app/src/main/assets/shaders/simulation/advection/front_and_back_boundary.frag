#version 310 es
precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform vec3 gridSize;
uniform int depth;
uniform float scale;
out vec3 outColor;
void main() {

    ivec3 ipos = ivec3(gl_FragCoord.xy, depth);
    vec2 pos = floor(gl_FragCoord.xy) / (gridSize.xy - 1.0f);

    vec2 middle = vec2(0.5, 0.5);
    int depthDir = depth == 0 ? 1 : -1;
    ivec3 dir = ivec3 ((middle - pos) * 2.0f, 0);

    vec3 value = 0.5f * scale * (texelFetch(data, ipos + dir, 0).xyz + texelFetch(data, ipos + ivec3 (0, 0, depthDir), 0).xyz);

    // corners
    if (abs(dir.x) == abs(dir.y)){
        value = texelFetch(data, ipos + ivec3 (0, 0, depthDir), 0).xyz;
        value += texelFetch(data, ipos + ivec3 (dir.x, 0, 0), 0).xyz;
        value += texelFetch(data, ipos + ivec3 (0, dir.y, 0), 0).xyz;
        value *= scale * 1.0f/3.0f;
    }
    outColor = value;
}