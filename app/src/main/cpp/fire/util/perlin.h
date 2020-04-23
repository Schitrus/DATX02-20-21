//
// Created by Kalle on 2020-04-23.
//

#ifndef DATX02_20_21_PERLIN_H
#define DATX02_20_21_PERLIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

using namespace glm;

#define PI 3.14159265359f

#define LOG_TAG "perlin"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern ivec3 seed;

extern ivec3 corners[8];

extern int num_grads;

extern vec3* grads;


void initPerlin();

double fade(double x);

double perlin(vec3 position);

#endif //DATX02_20_21_PERLIN_H
