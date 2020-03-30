//
// Created by kirde on 2020-03-29.
//

#ifndef DATX02_20_21_DATA_TEXTURE_PAIR_H
#define DATX02_20_21_DATA_TEXTURE_PAIR_H

#include <GLES3/gl32.h>

#include <glm/glm.hpp>

using namespace glm;

class data_texture_pair {
    GLuint data, result;

public:
    // initiates the textures as scalar fields with the given data
    // it ignores any previous textures, so only call init once per pair!
    void initScalarData(int width, int height, int depth, float* data);

    // initiates the textures as vector fields with the given data
    // it ignores any previous textures, so only call init once per pair!
    void initVectorData(int width, int height, int depth, vec3* data);

    // binds the data to the provided slot
    // if operationFinished() is called and this data is expected to be used again, bindData() must be called again
    // (for example during iterative operations where the result of last iteration is used as data for the next iteration)
    void bindData(GLenum textureSlot);

    // binds the result texture to the currently bound framebuffer so that the result is rendered to
    void bindToFramebuffer(int depth);

    // signifies that the caller has finished operation step, such that the data and result should swap
    void operationFinished();

    // returns the data texture. Try to avoid usage of this by calling bindData() instead
    GLuint getDataTexture();

    // returns the result texture. Try to avoid usage of this by calling bindToFramebuffer() instead
    GLuint getResultTexture();
};


#endif //DATX02_20_21_DATA_TEXTURE_PAIR_H
