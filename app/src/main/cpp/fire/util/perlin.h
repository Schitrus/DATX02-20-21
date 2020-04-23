//
// Created by Kalle on 2020-04-23.
//

#ifndef DATX02_20_21_PERLIN_H
#define DATX02_20_21_PERLIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#define PI 3.14159265359f

extern ivec3 seed;

extern ivec3 corners[8];

extern int num_grads;

extern vec3* grads;


void initPerlin();

double fade(double x);

double perlin(vec3 position);

#endif //DATX02_20_21_PERLIN_H
