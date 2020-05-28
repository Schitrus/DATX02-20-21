//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"

#include <stdio.h>

#include <stdlib.h>
#include <android/log.h>

#include <fire/util/helper.h>
#include <cstdlib>

#define LOG_TAG "wavelet"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


int WaveletTurbulence::init(SlabOperation* slab, Settings* settings) {

    srand(42);

    this->slab = slab;

    if(!initShaders())
        return 0;

    initTextures(settings);

    LOG_INFO("Finished initializing  wavelet turbulence");

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

void WaveletTurbulence::initTextures(Settings* settings) {
    ivec3 lowResSize = settings->getSize(Resolution::velocity);
    ivec3 highResSize = settings->getSize(Resolution::substance);
    band_min = glm::log2(min(min((float)lowResSize.x, (float)lowResSize.y), (float)lowResSize.z));
    band_max = glm::log2(max(max((float)highResSize.x, (float)highResSize.y), (float)highResSize.z)/2);

    texture_coord = createVectorDataPair(nullptr, Resolution::velocity, settings);
    energy = createScalarDataPair(nullptr, Resolution::velocity, settings);

    wavelet_turbulence = createVectorDataPair(nullptr, Resolution::substance, settings);
    noiseTexture1 = createScalarDataPair(nullptr, Resolution::substance, settings);
    noiseTexture2 = createScalarDataPair(nullptr, Resolution::substance, settings);
    noiseTexture3 = createScalarDataPair(nullptr, Resolution::substance, settings);

    jacobianXTexture = createVectorDataPair(nullptr, Resolution::velocity, settings);
    jacobianYTexture = createVectorDataPair(nullptr, Resolution::velocity, settings);
    jacobianZTexture = createVectorDataPair(nullptr, Resolution::velocity, settings);
    eigenTexture = createVectorDataPair(nullptr, Resolution::velocity, settings);

    GenerateWavelet();
}

void WaveletTurbulence::clearTextures() {
    delete texture_coord, delete energy, delete wavelet_turbulence,
    delete noiseTexture1, delete noiseTexture2, delete noiseTexture3,
    delete jacobianXTexture, delete jacobianYTexture, delete jacobianZTexture,
    delete eigenTexture;

}

int WaveletTurbulence::changeSettings(Settings* settings, bool shouldRegenFields) {

    if(shouldRegenFields) {
        //clearTextures();
        initTextures(settings);
    }
    return 1;
}

void WaveletTurbulence::GenerateWavelet(){

    slab->prepare();

    LOG_INFO("band_min: %f, band_max: %f", band_min, band_max);

    noise(noiseTexture1, band_min, band_max);
    noise(noiseTexture2, band_min, band_max);
    noise(noiseTexture3, band_min, band_max);

    waveletShader.use();

    waveletShader.uniform3f("gridSize", wavelet_turbulence->getSize());

    noiseTexture1->bindData(GL_TEXTURE0);
    noiseTexture2->bindData(GL_TEXTURE1);
    noiseTexture3->bindData(GL_TEXTURE2);

    slab->interiorOperation(waveletShader, wavelet_turbulence, 0);

    slab->finish();

}

void WaveletTurbulence::noise(DataTexturePair* noiseTexture, float band_min, float band_max){

    turbulenceShader.use();

    turbulenceShader.uniform3f("gridSize", noiseTexture->getSize());
    int num_gradients = 1024;
    turbulenceShader.uniform1i("num_gradients", num_gradients);

    for(float band = band_min; band <= band_max; band += 1.0) {
        ivec3 seed = ivec3(rand(), rand(), rand());
        vec3* gradients = generateGradients(num_gradients);

        GLuint gradient_texture;
        createVector3DTexture(gradient_texture, ivec3(num_gradients, 1, 1), gradients);

        turbulenceShader.uniform1i("seed1", seed.x);
        turbulenceShader.uniform1i("seed2", seed.y);
        turbulenceShader.uniform1i("seed3", seed.z);
        turbulenceShader.uniform1f("band", band);
        turbulenceShader.uniform1f("min_band", band_min);

        noiseTexture->bindData(GL_TEXTURE0);
        bindData(gradient_texture, GL_TEXTURE1);

        slab->fullOperation(turbulenceShader, noiseTexture);

        glDeleteTextures(1, &gradient_texture);
    }
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

void WaveletTurbulence::waveletStep(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity, float dt) {
    // Advect texture coordinates
    advection(lowerVelocity, dt);

    calcEnergy(lowerVelocity);

    calcScattering();

    regenerate(lowerVelocity);

    fluidSynthesis(lowerVelocity, higherVelocity);
}

void WaveletTurbulence::advection(DataTexturePair* lowerVelocity, float dt){
    textureCoordShader.use();

    textureCoordShader.uniform3f("gridSize", lowerVelocity->getSize());
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
    jacobianShader.uniform3f("gridSize", colTexture->getSize());
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
    eigenShader.uniform3f("gridSize", jacobianXTexture->getSize());
    eigenShader.uniform1i("maxIterations", 20);

    jacobianXTexture->bindData(GL_TEXTURE0);
    jacobianYTexture->bindData(GL_TEXTURE1);
    jacobianZTexture->bindData(GL_TEXTURE2);

    slab->fullOperation(eigenShader, eigenTexture);
}

void WaveletTurbulence::regenerate(DataTexturePair *lowerVelocity) {
    regenerateShader.use();

    regenerateShader.uniform3f("gridSize", lowerVelocity->getSize());
    regenerateShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());

    texture_coord->bindData(GL_TEXTURE0);
    eigenTexture->bindData(GL_TEXTURE1);


    slab->fullOperation(regenerateShader, texture_coord);
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    synthesisShader.uniform3f("gridSize", higherVelocity->getSize());

    lowerVelocity->bindData(GL_TEXTURE0);
    wavelet_turbulence->bindData(GL_TEXTURE1);
    texture_coord->bindData(GL_TEXTURE2);
    energy->bindData(GL_TEXTURE3);
    jacobianXTexture->bindData(GL_TEXTURE4);
    jacobianYTexture->bindData(GL_TEXTURE5);
    jacobianZTexture->bindData(GL_TEXTURE6);

    slab->fullOperation(synthesisShader, higherVelocity);
}