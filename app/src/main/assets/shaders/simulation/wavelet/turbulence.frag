#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;
layout(binding = 1) uniform sampler3D gradient_field;

uniform int seed1;
uniform int seed2;
uniform int seed3;

uniform int num_gradients;

uniform vec3 gridSize;
uniform int depth;
uniform float band;
uniform float min_band;

out float outData;

const vec3 dx = vec3(1.0f, 0.0f, 0.0f);
const vec3 dy = vec3(0.0f, 1.0f, 0.0f);
const vec3 dz = vec3(0.0f, 0.0f, 1.0f);

float fade(float x){
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

vec3 getGradient(ivec3 position){
    int index = position.x*position.x + position.y*position.y + position.z*position.z;
    return texelFetch(gradient_field, ivec3(index % num_gradients, 0, 0), 0).xyz;
}

void main() {

    ivec3 uvw = ivec3(gl_FragCoord.xy, depth);

    float len = max(max(gridSize.x, gridSize.y), gridSize.z);

    vec3 position = pow(2.0, band) * (vec3(uvw) + vec3(0.5f)) / len;

    ivec3 seed = ivec3(seed1, seed2, seed3);

    vec3 internal_position = fract(position);
    ivec3 external_position = ivec3(position) + seed;

    ivec3 corners[8];
    corners[0] = ivec3(0, 0, 0);
    corners[1] = ivec3(1, 0, 0);
    corners[2] = ivec3(0, 1, 0);
    corners[3] = ivec3(1, 1, 0);
    corners[4] = ivec3(0, 0, 1);
    corners[5] = ivec3(1, 0, 1);
    corners[6] = ivec3(0, 1, 1);
    corners[7] = ivec3(1, 1, 1);

    float dots[8];
    for (int c = 0; c < 8; c++){
        ivec3 external_corner = seed * (external_position + corners[c]);
        vec3 gradient = getGradient(external_corner);
        dots[c] = dot(gradient, internal_position - vec3(corners[c]));
    }

    //interpolate the values
    float y0, y1, z0, z1, p;
    y0  =  mix(dots[0], dots[1], fade(internal_position.x));
    y1  =  mix(dots[2], dots[3], fade(internal_position.x));
    z0  =  mix(y0, y1, fade(internal_position.y));
    y0  =  mix(dots[4], dots[5], fade(internal_position.x));
    y1  =  mix(dots[6], dots[7], fade(internal_position.x));
    z1  =  mix(y0, y1, fade(internal_position.y));
    p   =  mix(z0, z1, fade(internal_position.z));

    outData = texelFetch(data_field, uvw, 0).x + abs(p) * pow(2.0, -(band-min_band));
}