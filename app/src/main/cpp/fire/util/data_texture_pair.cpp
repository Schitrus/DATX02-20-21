//
// Created by kirderf1 on 2020-03-29.
//

#include "data_texture_pair.h"

#include <GLES3/gl31.h>

#include <glm/glm.hpp>

#include "helper.h"
#include "fire/simulation/simulator.h"

using namespace glm;

DataTexturePair::~DataTexturePair() {
    glDeleteTextures(1, &dataTexture);
    glDeleteTextures(1, &resultTexture);
}

void DataTexturePair::initScalarData(bool isHighRes, float* data) {
    this->isHighRes = isHighRes;
    ivec3 size = getSize();
    createScalar3DTexture(&dataTexture, size, data);
    createScalar3DTexture(&resultTexture, size, (float*)nullptr);
}

void DataTexturePair::initVectorData(bool isHighRes, vec3* data) {
    this->isHighRes = isHighRes;
    ivec3 size = getSize();
    createVector3DTexture(&dataTexture, size, data);
    createVector3DTexture(&resultTexture, size, (vec3*)nullptr);
}

void DataTexturePair::bindData(GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}

void DataTexturePair::bindToFramebuffer(int depth) {
    // attach result texture to framebuffer
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTexture, 0, depth);
    /* okay, maybe not considering border fire.rendering
    // since we are essentially overwriting the result texture, we should probably clear it (or does it actually matter?)
    glClear(GL_COLOR_BUFFER_BIT);
     */
}

void DataTexturePair::operationFinished() {
    GLuint tmp = dataTexture;
    dataTexture = resultTexture;
    resultTexture = tmp;
}

GLuint DataTexturePair::getDataTexture() {
    return dataTexture;
}

GLuint DataTexturePair::getResultTexture() {
    return resultTexture;
}

bool DataTexturePair::isUsingHighRes() {
    return isHighRes;
}

ivec3 DataTexturePair::getSize() {
    return isHighRes ? highResSize : lowResSize;
}

float DataTexturePair::toVoxelScaleFactor() {
    return (isHighRes ? highResScale : lowResScale)/simulationScale;
}

DataTexturePair* createScalarDataPair(bool isHighRes, float* data) {
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initScalarData(isHighRes, data);
    return texturePair;
}

DataTexturePair* createVectorDataPair(bool isHighRes, vec3* data) {
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initVectorData(isHighRes, data);
    return texturePair;
}