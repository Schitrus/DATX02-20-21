//
// Created by kirderf1 on 2020-03-29.
//

#ifndef DATX02_20_21_DATA_TEXTURE_PAIR_H
#define DATX02_20_21_DATA_TEXTURE_PAIR_H

#include <GLES3/gl31.h>

#include <glm/glm.hpp>
#include <fire/settings.h>

using namespace glm;

enum TextureType { SCALAR, VECTOR };

class DataTexturePair {
    float scaleFactor;
    ivec3 size;
    GLuint dataTexture, resultTexture;

    TextureType type;

public:
    ~DataTexturePair();

    // Clears the data in the textures to zero values;
    void clearData();

    // initiates the textures as scalar fields with the given data
    // it ignores any previous textures, so only call init once per pair!
    void initScalarData(float scaleFactor, ivec3 size, float* data);

    // initiates the textures as vector fields with the given data
    // it ignores any previous textures, so only call init once per pair!
    void initVectorData(float scaleFactor, ivec3 size, vec3* data);

    // binds the data to the provided slot
    // The slot should be GL_TEXTURE0 or any larger number, depending on where you need the texture
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

    ivec3 getSize();

    float toVoxelScaleFactor();
};

// creates a scalar data pair with the given data
DataTexturePair* createScalarDataPair(float* data, Resolution res, Settings settings);

// create a vector data pair with the given data
DataTexturePair* createVectorDataPair(vec3* data, Resolution res, Settings settings);

#endif //DATX02_20_21_DATA_TEXTURE_PAIR_H
