//
// Created by kirde on 2020-03-29.
//

#include "data_texture_pair.h"

#include <GLES3/gl32.h>

#include <glm/glm.hpp>

#include "helper.h"

using namespace glm;

void data_texture_pair::initScalarData(int width, int height, int depth, float* data) {
    createScalar3DTexture(&dataTexture, width, height, depth, data);
    createScalar3DTexture(&resultTexture, width, height, depth, NULL);
}

void data_texture_pair::initVectorData(int width, int height, int depth, vec3* data) {
    createVector3DTexture(&dataTexture, width, height, depth, data);
    createVector3DTexture(&resultTexture, width, height, depth, NULL);
}

void data_texture_pair::bindData(GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}

void data_texture_pair::bindToFramebuffer(int depth) {
    // attach result texture to framebuffer
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTexture, 0, depth);
    // since we are essentially overwriting the result texture, we should probably clear it (or does it actually matter?)
    glClear(GL_COLOR_BUFFER_BIT);
}

void data_texture_pair::operationFinished() {
    GLuint tmp = dataTexture;
    dataTexture = resultTexture;
    resultTexture = tmp;
}

GLuint data_texture_pair::getDataTexture() {
    return dataTexture;
}

GLuint data_texture_pair::getResultTexture() {
    return resultTexture;
}