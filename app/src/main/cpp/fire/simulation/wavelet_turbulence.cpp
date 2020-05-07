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

    band_min = glm::log2(min(min((float)lowResSize.x, (float)lowResSize.y), (float)lowResSize.z));
    band_max = glm::log2(max(max((float)highResSize.x, (float)highResSize.y), (float)highResSize.z)/2);

    LOG_INFO("band_min: %f, band_max: %f", band_min, band_max);

    vec3* w = wavelet(highResSize, band_min, band_max);

    wavelet_turbulence = createVectorDataPair(true, w);

    delete[] w;

    return 1;
}

int WaveletTurbulence::initShaders() {
    bool success = true;
    //success &= turbulenceShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/turbulence.frag");
    success &= synthesisShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/fluid_synthesis.frag");
    success &= textureCoordShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/advection.frag");
    success &= energyShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/energy_spectrum.frag");
    success &= regenerateShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/regeneration.frag");
    success &= eigenShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/eigenCalculator.frag");
    success &= jacobianShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/jacobianCalculator.frag");
    return success;
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