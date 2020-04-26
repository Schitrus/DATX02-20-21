//
// Created by Kalle on 2020-04-23.
//

#include "perlin.h"

ivec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
                    {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

ivec3 seed;

int num_grads;

vec3* grads;

void initPerlin() {
    seed = ivec3(rand(), rand(), rand());
    num_grads = 1024;
    grads = new vec3[num_grads];
    for(int i = 0; i < num_grads; i++) {
        float a1 = rand()%360 / 180.0f * PI;
        float a2 = rand()%360 / 180.0f * PI;
        float a3 = rand()%360 / 180.0f * PI;
        grads[i] = normalize(vec3(sin(a1)*sin(a2)*a3 + cos(a1)*a2, cos(a1)*sin(a2)*a3 - sin(a1)*a2, cos(a2)*a3 + a1));
    }
}

double fade(double x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}
//linear interpolation with smoothstep

//Get perlin noise from 3d value
double perlin(vec3 position){
    vec3 internal_position = fract(position);
    ivec3 external_position = ivec3(position) + seed;
    double dots[8] = {0};
    //Loop through all corners
    for (int c = 0; c < 8; c++){
        ivec3 external_corner = seed * (external_position + corners[c]);
        vec3 grad = grads[abs(external_corner.x*external_corner.x + external_corner.y*external_corner.y + external_corner.z*external_corner.z)%num_grads];
        dots[c] = dot(grad, internal_position - vec3(corners[c]));
    }
    //interpolate the values
    //interpolate the values
    double y0, y1, z0, z1, p;
    y0  =  mix(dots[0], dots[1], fade(internal_position.x));
    y1  =  mix(dots[2], dots[3], fade(internal_position.x));
    z0  =  mix(y0, y1, fade(internal_position.y));
    y0  =  mix(dots[4], dots[5], fade(internal_position.x));
    y1  =  mix(dots[6], dots[7], fade(internal_position.x));
    z1  =  mix(y0, y1, fade(internal_position.y));
    p   =  mix(z0, z1, fade(internal_position.z));
    return p;
}