#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D velocity;
uniform int depth;
out float outColor;

void main() {
    ivec2 tcoord = ivec2(gl_FragCoord.xy);

    float x = texelFetch(velocity ,ivec3( tcoord.x + 1, tcoord.y, depth), 0).x - texelFetch(velocity ,ivec3( tcoord.x - 1, tcoord.y, depth), 0).x;
    float y = texelFetch(velocity ,ivec3( tcoord.x, tcoord.y + 1, depth), 0).y - texelFetch(velocity ,ivec3( tcoord.x, tcoord.y -1, depth), 0).y;
    float z = texelFetch(velocity ,ivec3( tcoord.x, tcoord.y, depth +1 ), 0).z - texelFetch(velocity ,ivec3( tcoord.x, tcoord.y, depth -1 ), 0).z;

    outColor = 0.5f * (x + y + z);

}
