//
// Created by Kalle on 2020-04-01.
//

#ifndef DATX02_20_21_WAVELET_TURBULENCE_H
#define DATX02_20_21_WAVELET_TURBULENCE_H

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    Shader turbulenceShader;
    Shader waveletShader;

    Shader synthesisShader;
    Shader energyShader;
    Shader textureCoordShader;
    Shader regenerateShader;
    Shader eigenShader;
    Shader jacobianShader;

    DataTexturePair* wavelet_turbulence;
    DataTexturePair* energy;
    DataTexturePair* texture_coord;

    DataTexturePair* eigenTexture;
    DataTexturePair* jacobianXTexture;
    DataTexturePair* jacobianYTexture;
    DataTexturePair* jacobianZTexture;

    float band_min, band_max;

public:
    int init(SlabOperator* slab, Settings settings);

    int changeSettings(Settings settings);

    void advection(DataTexturePair* lowerVelocity, float dt);

    void calcEnergy(DataTexturePair* lowerVelocity);

    void regenerate(DataTexturePair* lowerVelocity);

    void fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity);

    vec3* generateGradients(int num_gradients);

    double turbulence(vec3 position, vec3 offset, vec3 size);

    void wave();

    DataTexturePair* noise(float band_min, float band_max);

    void calcScattering();

private:
    int initShaders();

    void calcJacobianCol(int axis, DataTexturePair* colTexture);

    void initTextures(Settings settings);

    void clearTextures();

    void generateWavelet(Settings settings);

    double* generateTurbulence(vec3 size);

};

#endif //DATX02_20_21_WAVELET_TURBULENCE_H
