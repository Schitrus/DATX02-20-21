#version 320 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform vec3 gridSize;
uniform int depth;
uniform float scale;

out vec3 outData;

// Performs boundary condition computation for
// only the front and back 2D grids of the 3D field
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    vec2 uv_position = floor(gl_FragCoord.xy) / (gridSize.xy - 1.0f);

    vec2 center = vec2(0.5, 0.5);
    ivec3 direction = ivec3((center - uv_position) * 2.0f, (depth == 0 ? 1 : -1));

    vec3 data = 0.5f * scale * (texelFetch(data_field, position + ivec3(direction.xy, 0),   0).xyz
                              + texelFetch(data_field, position + ivec3(0, 0, direction.z), 0).xyz);

    // corners
    if (abs(direction.x) == abs(direction.y)){
        data  = texelFetch(data_field, position + ivec3 (direction.x, 0, 0), 0).xyz;
        data += texelFetch(data_field, position + ivec3 (0, direction.y, 0), 0).xyz;
        data += texelFetch(data_field, position + ivec3 (0, 0, direction.z), 0).xyz;
        data *= scale * 1.0f/3.0f;
    }
    outData = vec3(0.0f);
}