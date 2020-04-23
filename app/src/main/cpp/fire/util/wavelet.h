//
// Created by Kalle on 2020-04-23.
//

#ifndef DATX02_20_21_WAVELET_H
#define DATX02_20_21_WAVELET_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "turbulence.h"

using namespace glm;

vec3* wavelet(ivec3 size, float min_band, float max_band);

#endif //DATX02_20_21_WAVELET_H
