//
// Created by Kalle on 2020-04-01.
//

#ifndef DATX02_20_21_WAVELET_TURBULENCE_H
#define DATX02_20_21_WAVELET_TURBULENCE_H

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <android/log.h>
#include "fire/util/shader.h"
#include "fire/util/data_texture_pair.h"
#include "slab_operation.h"

using namespace glm;

#define PI 3.14159265359f

class WaveletTurbulence {

    SlabOperator* slab;

    vec3* advPos;
    vec3* eigenValues;
    vec3* jacobianX;
    vec3* jacobianY;
    vec3* jacobianZ;

    GLuint eigenTexture;
    GLuint jacobianXTexture;
    GLuint jacobianYTexture;
    GLuint jacobianZTexture;

    Shader turbulenceShader;
    Shader synthesisShader;
    Shader energyShader;
    Shader textureCoordShader;
    Shader regenerateShader;

    DataTexturePair* wavelet_turbulence;
    DataTexturePair* energy;
    DataTexturePair* texture_coord;

    float band_min, band_max;

public:
    int init(SlabOperator* slab);

    void advection(DataTexturePair* lowerVelocity, float dt);

    void calcEnergy(DataTexturePair* lowerVelocity);

    void regenerate(DataTexturePair* lowerVelocity);

    void fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity);

    double turbulence(vec3 position, vec3 offset, vec3 size);

    void calcScattering();

private:
    int initShaders();

    vec3 calcEigen(vec3 x, vec3 y, vec3 z);

    void generateWavelet();
    double* generateTurbulence(vec3 size);

};

#endif //DATX02_20_21_WAVELET_TURBULENCE_H
