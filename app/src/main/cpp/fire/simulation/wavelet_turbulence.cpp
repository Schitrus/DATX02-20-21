//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"
#include "simulator.h"

#include <stdio.h>

#include <stdlib.h>

#include "fire/util/wavelet.h"
#include <fire/util/helper.h>

#define LOG_TAG "wavelet"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


int WaveletTurbulence::init(SlabOperator* slab) {

    srand(42);

    this->slab = slab;

    if(!initShaders())
        return 0;

    texture_coord = createVectorDataPair(false, nullptr);
    energy = createScalarDataPair(false, nullptr);
    jacobianXTexture = createVectorDataPair(false, nullptr);
    jacobianYTexture = createVectorDataPair(false, nullptr);
    jacobianZTexture = createVectorDataPair(false, nullptr);
    eigenTexture = createVectorDataPair(false, nullptr);

    advPos = new vec3[lowResSize.x * lowResSize.y * lowResSize.z];
    eigenValues = new vec3[lowResSize.x * lowResSize.y * lowResSize.z];
    jacobianX = new vec3[lowResSize.x * lowResSize.y * lowResSize.z];
    jacobianY = new vec3[lowResSize.x * lowResSize.y * lowResSize.z];
    jacobianZ = new vec3[lowResSize.x * lowResSize.y * lowResSize.z];

    wave();

    return 1;
}

int WaveletTurbulence::initShaders() {
    bool success = true;
    success &= turbulenceShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/turbulence.frag");
    success &= waveletShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/wavelet.frag");
    success &= synthesisShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/fluid_synthesis.frag");
    success &= textureCoordShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/advection.frag");
    success &= energyShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/energy_spectrum.frag");
    success &= regenerateShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/regeneration.frag");
    success &= eigenShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/eigenCalculator.frag");
    success &= jacobianShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/jacobianCalculator.frag");
    return success;
}

void WaveletTurbulence::wave(){

    slab->prepare();

    band_min = glm::log2(min(min((float)lowResSize.x, (float)lowResSize.y), (float)lowResSize.z));
    band_max = glm::log2(max(max((float)highResSize.x, (float)highResSize.y), (float)highResSize.z)/2);

    LOG_INFO("band_min: %f, band_max: %f", band_min, band_max);

    DataTexturePair* w1 = noise(band_min, band_max);
    DataTexturePair* w2 = noise(band_min, band_max);
    DataTexturePair* w3 = noise(band_min, band_max);

    wavelet_turbulence = createVectorDataPair(true, nullptr);

    waveletShader.use();

    waveletShader.uniform3f("gridSize", highResSize);

    w1->bindData(GL_TEXTURE0);
    w2->bindData(GL_TEXTURE1);
    w3->bindData(GL_TEXTURE2);

    slab->interiorOperation(waveletShader, wavelet_turbulence, 0);

    slab->finish();

}

DataTexturePair* WaveletTurbulence::noise(float band_min, float band_max){
    DataTexturePair* noiseTexture = createScalarDataPair(true, nullptr);

    turbulenceShader.use();

    turbulenceShader.uniform3f("gridSize", highResSize);
    int num_gradients = 1024;
    turbulenceShader.uniform1i("num_gradients", num_gradients);

    for(float band = band_min; band <= band_max; band += 1.0) {
        ivec3 seed = ivec3(rand(), rand(), rand());
        vec3* gradients = generateGradients(num_gradients);

        GLuint gradient_texture;
        createVector3DTexture(&gradient_texture, ivec3(num_gradients, 1, 1), gradients);

        turbulenceShader.uniform1i("seed1", seed.x);
        turbulenceShader.uniform1i("seed2", seed.y);
        turbulenceShader.uniform1i("seed3", seed.z);
        turbulenceShader.uniform1f("band", band);
        turbulenceShader.uniform1f("min_band", band_min);

        noiseTexture->bindData(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gradient_texture);

        slab->fullOperation(turbulenceShader, noiseTexture);

    }

    return noiseTexture;
}

vec3* WaveletTurbulence::generateGradients(int num_gradients){
    vec3* gradients = new vec3[num_gradients];
    for(int i = 0; i < num_gradients; i++) {
        float a1 = rand()%360 / 180.0f * PI;
        float a2 = rand()%360 / 180.0f * PI;
        float a3 = rand()%360 / 180.0f * PI;
        gradients[i] = normalize(vec3(sin(a1)*sin(a2)*a3 + cos(a1)*a2, cos(a1)*sin(a2)*a3 - sin(a1)*a2, cos(a2)*a3 + a1));
    }
    return gradients;
}

void WaveletTurbulence::advection(DataTexturePair* lowerVelocity, float dt){
    textureCoordShader.use();

    textureCoordShader.uniform3f("gridSize", lowResSize);
    textureCoordShader.uniform1f("dt", dt);
    textureCoordShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());

    lowerVelocity->bindData(GL_TEXTURE0);

    slab->fullOperation(textureCoordShader, texture_coord);
}

void WaveletTurbulence::calcEnergy(DataTexturePair* lowerVelocity){
    energyShader.use();
    energyShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());
    lowerVelocity->bindData(GL_TEXTURE0);
    slab->fullOperation(energyShader, energy);
}

void WaveletTurbulence::calcJacobianCol(int axis, DataTexturePair* colTexture){
    jacobianShader.use();
    jacobianShader.uniform3f("gridSize", lowResSize);
    jacobianShader.uniform1i("axis", axis);

    texture_coord->bindData(GL_TEXTURE0);

    slab->interiorOperation(jacobianShader, colTexture, -1);
}

void WaveletTurbulence::calcScattering() {
    // calc the first column of the jacobian for each grid cell
    calcJacobianCol(0, jacobianXTexture);
    // calc the second column of the jacobian for each grid cell
    calcJacobianCol(1, jacobianYTexture);
    // calc the third column of the jacobian for each grid cell
    calcJacobianCol(2, jacobianZTexture);

    // calc the eigen value for each grid cell
    eigenShader.use();
    eigenShader.uniform3f("gridSize", lowResSize);
    eigenShader.uniform1i("maxIterations", 20);

    jacobianXTexture->bindData(GL_TEXTURE0);
    jacobianYTexture->bindData(GL_TEXTURE1);
    jacobianZTexture->bindData(GL_TEXTURE2);

    slab->fullOperation(eigenShader, eigenTexture);
}

void WaveletTurbulence::regenerate(DataTexturePair *lowerVelocity) {
    regenerateShader.use();

    regenerateShader.uniform3f("gridSize", lowResSize);
    regenerateShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());

    texture_coord->bindData(GL_TEXTURE0);
    eigenTexture->bindData(GL_TEXTURE1);


    slab->fullOperation(regenerateShader, texture_coord);
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    synthesisShader.uniform3f("gridSize", highResSize);

    lowerVelocity->bindData(GL_TEXTURE0);
    wavelet_turbulence->bindData(GL_TEXTURE1);
    texture_coord->bindData(GL_TEXTURE2);
    energy->bindData(GL_TEXTURE3);
    jacobianXTexture->bindData(GL_TEXTURE4);
    jacobianYTexture->bindData(GL_TEXTURE5);
    jacobianZTexture->bindData(GL_TEXTURE6);

    slab->fullOperation(synthesisShader, higherVelocity);
}