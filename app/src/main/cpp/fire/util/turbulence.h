//
// Created by Kalle on 2020-04-23.
//

#ifndef DATX02_20_21_TURBULENCE_H
#define DATX02_20_21_TURBULENCE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#include "perlin.h"

#define LOG_TAG "Turbulence"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace glm;

double* turbulence(ivec3 size, int min_band, int max_band);

#endif //DATX02_20_21_TURBULENCE_H
