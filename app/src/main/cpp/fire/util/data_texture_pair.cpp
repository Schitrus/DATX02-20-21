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

void DataTexturePair::initScalarData(float scaleFactor, ivec3 size, float* data) {
    this->scaleFactor = scaleFactor;
    this->size = size;
    createScalar3DTexture(&dataTexture, size, data);
    createScalar3DTexture(&resultTexture, size, (float*)nullptr);
}

void DataTexturePair::initVectorData(float scaleFactor, ivec3 size, vec3* data) {
    this->scaleFactor = scaleFactor;
    this->size = size;
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

ivec3 DataTexturePair::getSize() {
    return size;
}

float DataTexturePair::toVoxelScaleFactor() {
    return scaleFactor;
}

DataTexturePair* createScalarDataPair(Settings settings, bool isHighRes, float* data) {
    float scaleFactor = (isHighRes ? highResScale : lowResScale)/simulationScale;
    ivec3 size = isHighRes ? highResSize : lowResSize;
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initScalarData(scaleFactor, size, data);
    return texturePair;
}

DataTexturePair* createVectorDataPair(Settings settings, bool isHighRes, vec3* data) {
    float scaleFactor = (isHighRes ? highResScale : lowResScale)/simulationScale;
    ivec3 size = isHighRes ? highResSize : lowResSize;
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initVectorData(scaleFactor, size, data);
    return texturePair;
}