//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"
#include "simulator.h"

#include <stdio.h>

#include <stdlib.h>

#include "fire/util/wavelet.h"
#include <fire/util/eigen.h>

#define LOG_TAG "wavelet"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


int WaveletTurbulence::init(SlabOperator* slab) {

    //srand(42);

    this->slab = slab;
    if(!initShaders())
        return 0;

    texture_coord = createScalarDataPair(false, nullptr);
    energy = createScalarDataPair(false, nullptr);

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

void WaveletTurbulence::calcScattering() {
    Eigen::Matrix3f jacobian(3,3);
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            jacobian(i,j) = 0.0f;
        }
    }
    // as the convergence from low resolution to high resolution only scales
    jacobian(0,0) = lowResSize.x;
    jacobian(1,1) = lowResSize.y;
    jacobian(2,2) = lowResSize.z;
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    synthesisShader.uniform3f("gridSize", highResSize);

    lowerVelocity->bindData(GL_TEXTURE0);
    wavelet_turbulence->bindData(GL_TEXTURE1);
    texture_coord->bindData(GL_TEXTURE2);
    energy->bindData(GL_TEXTURE3);

    slab->interiorOperation(synthesisShader, higherVelocity, -1);

}

