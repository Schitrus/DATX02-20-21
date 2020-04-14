//
// Created by Kalle on 2020-04-01.
//

#ifndef DATX02_20_21_WAVELET_TURBULENCE_H
#define DATX02_20_21_WAVELET_TURBULENCE_H

#include <gles3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <android/log.h>
#include "shader.h"
#include "data_texture_pair.h"
#include "slab_operation.h"

using namespace glm;

#define PI 3.14159265359f

class WaveletTurbulence {

    SlabOperator* slab;

    Shader turbulenceShader;
    Shader synthesisShader;
    Shader energyShader;
    Shader textureCoordShader;

    DataTexturePair* wavelet_turbulence;
    DataTexturePair* energy;
    DataTexturePair* texture_coord;

    unsigned int band_min, band_max;

    vec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
                       {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

    int num_angles;

    double* angles;

public:
    int init(SlabOperator* slab);

    void advection(DataTexturePair* lowerVelocity, float dt);

    void calcEnergy(DataTexturePair* lowerVelocity);

    void fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity);

    double turbulence(vec3 position, vec3 offset, vec3 size);
private:
    int initShaders();

    void generateAngles();
    void generateWavelet();
    double* generateTurbulence(vec3 size);
    double perlin(vec3 position);

};


#endif //DATX02_20_21_WAVELET_TURBULENCE_H
