//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"
#include "simulator.h"

#include <stdio.h>

#include <stdlib.h>

#include "Eigen/Dense"

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
    return success;
}

void WaveletTurbulence::advection(DataTexturePair* lowerVelocity, float dt){
    textureCoordShader.use();

    textureCoordShader.uniform3f("gridSize", lowResSize);
    textureCoordShader.uniform1f("dt", dt);
    textureCoordShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());

    lowerVelocity->bindData(GL_TEXTURE0);

    //slab->fullOperation(textureCoordShader, texture_coord);

    vec3* data = new vec3[lowResSize.x * lowResSize.y];

    for (int i = 0; i < lowResSize.x * lowResSize.y; ++i) {
        data[i] = vec3(0.0f,0.0f,0.0f);
    }

    for (int d = 0; d < texture_coord->getSize().z; d++) {
        texture_coord->bindToFramebuffer(d);
        if(!slab->drawAllToTexture(textureCoordShader, d, texture_coord->getSize()))
            return;
        glReadPixels(0, 0, texture_coord->getSize().x, texture_coord->getSize().y, GL_RGBA, GL_FLOAT, data);

        for (int i = 0; i < lowResSize.x * lowResSize.y; ++i) {
            advPos[lowResSize.x * lowResSize.y * d + i] = data[i];
        }
    }
    texture_coord->operationFinished();
    calcScattering();
}

void WaveletTurbulence::calcEnergy(DataTexturePair* lowerVelocity){
    energyShader.use();
    energyShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());
    lowerVelocity->bindData(GL_TEXTURE0);
    slab->fullOperation(energyShader, energy);
}

vec3 WaveletTurbulence::calcEigen(vec3 x, vec3 y, vec3 z){
    //create a jacobian matrix with the eigen library
    Eigen::Matrix3f jacobian(3,3);
    vec3 tmp;
    vec3 v[] = {x,y,z};
    for (int i = 0; i < 3; i++) {
        tmp = v[i];
        jacobian(i,0) = tmp.x;
        jacobian(i,1) = tmp.y;
        jacobian(i,2) = tmp.z;
    }

    Eigen::EigenSolver<Eigen::Matrix3f> eig(jacobian);

    std::complex<float> eig1 = eig.eigenvalues()[0];
    std::complex<float> eig2 = eig.eigenvalues()[1];
    std::complex<float> eig3 = eig.eigenvalues()[2];

    vec3 eigenReal = vec3(eig1.real(), eig2.real(), eig3.real());
    vec3 eigenImag = vec3(eig1.imag(), eig2.imag(), eig3.imag());

    vec3 eigenValues = sqrt(eigenReal * eigenReal + eigenImag * eigenImag);

    return eigenValues;
}

vec3 WaveletTurbulence::calcPartialD(int index, int step, int axisSize){
    // inspired by Theodore Kim & Nils Thürey, 2 of the authors of the paper "Wavelet Turbulence for Fluid Simulation"
    // https://www.cs.cornell.edu/~tedkim/WTURB/source.html
    vec3 center = advPos[index];
    vec3 prev = (center - advPos[index - step]) * (float)axisSize;
    vec3 next = (advPos[index + step] - center) * (float)axisSize;
    vec3 dcenter = (next - prev) * (float)axisSize * 0.5f;

    // calculate the smallest values of each vector, and return a new vector with these values
    float d0 = (fabs(dcenter.x) < fabs(next.x)) ? dcenter.x : next.x;
    d0 = (fabs(d0) < fabs(prev.x)) ? d0 : prev.x;

    float d1 = (fabs(dcenter.y) < fabs(next.y)) ? dcenter.y : next.y;
    d1 = (fabs(d1) < fabs(prev.y)) ? d1 : prev.y;

    float d2 = (fabs(dcenter.z) < fabs(next.z)) ? dcenter.z : next.z;
    d2 = (fabs(d2) < fabs(prev.z)) ? d2 : prev.z;

    return vec3(d0, d1, d2);
}

void WaveletTurbulence::calcScattering() {
    //calculate the jacobian for each position in the grid
    //the jacobian is structured as following:
    // (newx, newy, newz)
    for (int d = 1; d < lowResSize.z-1; d++) {
        for (int h = 1; h < lowResSize.y-1; h++) {
            for (int w = 1; w < lowResSize.x-1; w++) {
                int index = lowResSize.x * (lowResSize.y * d + h) + w;
                vec3 newx = calcPartialD(index, 1, lowResSize.x);
                vec3 newy = calcPartialD(index, lowResSize.x, lowResSize.y);
                vec3 newz = calcPartialD(index, lowResSize.x * lowResSize.y, lowResSize.z);
                eigenValues[index] = calcEigen(newx, newy, newz);
                mat3 jacobianI = inverse(mat3(newx, newy, newz));
                jacobianX[index] = jacobianI[0];
                jacobianY[index] = jacobianI[1];
                jacobianZ[index] = jacobianI[2];
            }
        }
    }
    createVector3DTexture(&eigenTexture, lowResSize, eigenValues);
    createVector3DTexture(&jacobianXTexture, lowResSize, jacobianX);
    createVector3DTexture(&jacobianYTexture, lowResSize, jacobianY);
    createVector3DTexture(&jacobianZTexture, lowResSize, jacobianZ);
}

void WaveletTurbulence::regenerate(DataTexturePair *lowerVelocity) {
    regenerateShader.use();

    regenerateShader.uniform3f("gridSize", lowResSize);
    regenerateShader.uniform1f("meterToVoxels", lowerVelocity->toVoxelScaleFactor());

    texture_coord->bindData(GL_TEXTURE0);
    bindData(eigenTexture, GL_TEXTURE1);

    slab->fullOperation(regenerateShader, texture_coord);
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    synthesisShader.uniform3f("gridSize", highResSize);

    lowerVelocity->bindData(GL_TEXTURE0);
    wavelet_turbulence->bindData(GL_TEXTURE1);
    texture_coord->bindData(GL_TEXTURE2);
    energy->bindData(GL_TEXTURE3);
    bindData(jacobianXTexture, GL_TEXTURE4);
    bindData(jacobianYTexture, GL_TEXTURE5);
    bindData(jacobianZTexture, GL_TEXTURE6);

    slab->fullOperation(synthesisShader, higherVelocity);
}