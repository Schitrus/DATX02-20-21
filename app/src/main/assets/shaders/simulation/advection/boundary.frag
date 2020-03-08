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

    ivec3 dir = ivec3 ((middle - pos) * 2.0f, 0);

    vec3 value = scale * texelFetch(data, ipos + dir, 0).xyz;

    //corners //abs(dir.x) == abs(dir.y)
    if (abs(dir) == ivec3(1, 1, 0)){
        value = scale * texelFetch(data, ipos + ivec3(dir.x, 0, 0), 0).xyz;
        value += scale * texelFetch(data, ipos + ivec3(0, dir.y, 0), 0).xyz;
        value *= 0.5f;
    }
    outColor = value;
}