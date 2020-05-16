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

    SlabOperation slab;

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

    DataTexturePair* noiseTexture1;
    DataTexturePair* noiseTexture2;
    DataTexturePair* noiseTexture3;

    DataTexturePair* eigenTexture;
    DataTexturePair* jacobianXTexture;
    DataTexturePair* jacobianYTexture;
    DataTexturePair* jacobianZTexture;

    float band_min, band_max;

public:
    int init(SlabOperation slab, Settings settings);

    int changeSettings(Settings settings);

    void waveletStep(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity, float dt);

private:
    int initShaders();

    void calcJacobianCol(int axis, DataTexturePair* colTexture);

    void initTextures(Settings settings);

    void clearTextures();

    void advection(DataTexturePair* lowerVelocity, float dt);

    void calcEnergy(DataTexturePair* lowerVelocity);

    void regenerate(DataTexturePair* lowerVelocity);

    void fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity);

    vec3* generateGradients(int num_gradients);

    void GenerateWavelet();

    void noise(DataTexturePair* noiseTexture, float band_min, float band_max);

    void calcScattering();

};

#endif //DATX02_20_21_WAVELET_TURBULENCE_H
