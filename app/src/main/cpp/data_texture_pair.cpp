//
// Created by kirderf1 on 2020-03-29.
//

#include "data_texture_pair.h"

#include <gles3/gl31.h>

#include <glm/glm.hpp>

#include "helper.h"

using namespace glm;

void DataTexturePair::initScalarData(int width, int height, int depth, float* data) {
    createScalar3DTexture(&dataTexture, width, height, depth, data);
    createScalar3DTexture(&resultTexture, width, height, depth, (float*)nullptr);
}

void DataTexturePair::initVectorData(int width, int height, int depth, vec3* data) {
    createVector3DTexture(&dataTexture, width, height, depth, data);
    createVector3DTexture(&resultTexture, width, height, depth, (vec3*)nullptr);
}

void DataTexturePair::bindData(GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}

void DataTexturePair::bindToFramebuffer(int depth) {
    // attach result texture to framebuffer
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTexture, 0, depth);
    /* okay, maybe not considering border rendering
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

DataTexturePair* createScalarDataPair(vec3 size, float* data) {
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initScalarData(size.x, size.y, size.z, data);
    return texturePair;
}

DataTexturePair* createVectorDataPair(vec3 size, vec3* data) {
    DataTexturePair* texturePair = new DataTexturePair();
    texturePair->initVectorData(size.x, size.y, size.z, data);
    return texturePair;
}