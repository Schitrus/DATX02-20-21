#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform vec3 gridSize;
uniform int depth;
uniform float scale;

out vec3 outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    vec2 uv_position = floor(gl_FragCoord.xy) / (gridSize.xy - 1.0f);

    vec2 center = vec2(0.5, 0.5);

    ivec3 direction = ivec3((center - uv_position) * 2.0f, 0);

    vec3 data = scale * texelFetch(data_field, position + direction, 0).xyz;

    //corners //abs(dir.x) == abs(dir.y)
    if (abs(direction) == ivec3(1, 1, 0)){
        data  = scale * texelFetch(data_field, position + ivec3(direction.x, 0, 0), 0).xyz;
        data += scale * texelFetch(data_field, position + ivec3(0, direction.y, 0), 0).xyz;
        data *= 0.5f;
    }
    outData = vec3(0.0f);
}