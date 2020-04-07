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

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace glm;

#define PI 3.14159265359f

class WaveletTurbulence {
    Shader turbulenceShader;
    Shader synthesisShader;
    Shader energyShader;
    Shader textureCoordShader;

    DataTexturePair* wavelet_turbulence;
    DataTexturePair* energy;
    DataTexturePair* texture_coord;

    unsigned int band_min, band_max;
    GLuint VAO;
    vec3 lowerResolution, higherResolution;
public:
    int init(vec3 lowerResolution, vec3 higherResolution, GLuint VAO);

    void advection(DataTexturePair* lowerVelocity, float dt);

    void calcEnergy(DataTexturePair* lowerVelocity);

    void fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity);
    void turbulence();

    double perlin(vec3 position);
    double* generateTurbulence(vec3 size);
    void generateWavelet();
private:
    int initShaders();
};


#endif //DATX02_20_21_WAVELET_TURBULENCE_H
