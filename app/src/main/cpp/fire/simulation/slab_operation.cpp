//
// Created by Anton Forsberg on 18/02/2020.
//
#include "slab_operation.h"

#include <jni.h>
#include <time.h>
#include <math.h>
#include <string>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "fire/util/helper.h"

#define LOG_TAG "Slab operation"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace glm;

#define PI 3.14159265359f

int SlabOperation::init() {

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    FBO = new SimpleFramebuffer();
    FBO->init();

    initQuad();
    initLine();

    if(!initShaders()) {
        LOG_ERROR("Failed to compile slab_operation shaders");
        return 0;
    }
    return 1;

}

void SlabOperation::initLine() {
    glGenVertexArrays(1, &boundaryVAO);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(boundaryVAO);

    ///////////////////////////////////
    // Define the positions
    const float positions[] = {
            // X      Y       Z
            -1.0f, -1.0f, 0.0f,  // v0
            -1.0f,  1.0f, 0.0f, // v1
             1.0f,  1.0f, 0.0f, // v2
             1.0f, -1.0f, 0.0f   // v3
    };

    // Create a handle for the position vertex buffer object
    glGenBuffers(1, &boundaryPositionBuffer);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, boundaryPositionBuffer);
    // Send the vertex position data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // Attaches positionBuffer to vertexArrayObject, in the 0th attribute location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex position attribute
    glEnableVertexAttribArray(0);

    const GLuint indices[] = {
            0, 1, // Line 1
            2, 3, // Line 2
    };

    glGenBuffers(1, &interiorIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, interiorIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void SlabOperation::initQuad() {

    glGenVertexArrays(1, &interiorVAO);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(interiorVAO);

    ///////////////////////////////////
    // Define the positions
    const float positions[] = {
            // X      Y       Z
            -1.0f, -1.0f, 0.0f,  // v0
            -1.0f, 1.0f, 0.0f, // v1
            1.0f, 1.0f, 0.0f, // v2
            1.0f, -1.0f, 0.0f   // v3
    };

    // Create a handle for the position vertex buffer object
    glGenBuffers(1, &interiorPositionBuffer);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, interiorPositionBuffer);
    // Send the vertex position data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // Attaches positionBuffer to vertexArrayObject, in the 0th attribute location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex position attribute
    glEnableVertexAttribArray(0);

    ///////////////////////////////////


    float texcoords[] = {
            0.0f, 0.0f, // (u,v) for v0
            0.0f, 1.0f, // (u,v) for v1
            1.0f, 1.0f, // (u,v) for v2
            1.0f, 0.0f // (u,v) for v3
    };

    glGenBuffers(1, &texcoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);

    // Enable the attribute
    glEnableVertexAttribArray(2);

    ///////////////////////////////////
    const GLuint indices[] = {
            0, 1, 3, // Triangle 1
            1, 2, 3  // Triangle 2
    };

    glGenBuffers(1, &interiorIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, interiorIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

int SlabOperation::initShaders() {
    bool success = true;
    // Boundaries
    success &= boundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/boundary.frag");
    success &= FABInteriorShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_interior.frag");
    success &= FABBoundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_boundary.frag");
    // Utilities
    success &= copyShader.load("shaders/simulation/slab.vert", "shaders/simulation/copy.frag");
    return success;
}

void SlabOperation::setBoundary(DataTexturePair* data, int scale) {
    // Input data used in all steps
    data->bindData(GL_TEXTURE0);
    ivec3 gridSize = data->getSize();
    boundaryShader.use();
    boundaryShader.uniform3f("gridSize", gridSize);
    boundaryShader.uniform1f("scale", scale);

    //Apply boundary operation
    for(int depth = 1; depth < gridSize.z - 1; depth++){
        data->bindToFramebuffer(depth);

        if(!drawBoundaryToTexture(boundaryShader, depth, gridSize))
            return;
    }

    //Copy over the inside
    copyShader.use();
    for(int depth = 1; depth < gridSize.z - 1; depth++){
        data->bindToFramebuffer(depth);

        if(!drawInteriorToTexture(copyShader, depth, gridSize))
            return;
    }

    // Front
    if(!drawFrontOrBackBoundary(data, scale, 0))
        return;

    //Back
    if(!drawFrontOrBackBoundary(data, scale, gridSize.z - 1))
        return;

    data->operationFinished();
}

bool SlabOperation::drawFrontOrBackBoundary(DataTexturePair* data, int scale, int depth){
    data->bindToFramebuffer(depth);

    ivec3 gridSize = data->getSize();
    FABBoundaryShader.use();
    FABBoundaryShader.uniform3f("gridSize", gridSize);
    FABBoundaryShader.uniform1f("scale", scale);
    if(!drawBoundaryToTexture(FABBoundaryShader,  depth, gridSize))
        return false;

    FABInteriorShader.use();
    FABInteriorShader.uniform1f("scale", scale);
    return drawInteriorToTexture(FABInteriorShader, depth, gridSize);
}

void SlabOperation::prepare() {

    // Setup GPU
    FBO->bind();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void SlabOperation::finish() {
    FBO->unbind();
}

void SlabOperation::interiorOperation(Shader shader, DataTexturePair* data, int boundaryScale) {
    ivec3 size = data->getSize();
    for(int depth = 1; depth < size.z - 1; depth++) {

        data->bindToFramebuffer(depth);
        if(!drawInteriorToTexture(shader, depth, size))
            return;
    }
    data->operationFinished();

    setBoundary(data, boundaryScale);
}

void SlabOperation::fullOperation(Shader shader, DataTexturePair* data) {
    ivec3 size = data->getSize();
    for(int depth = 0; depth < size.z; depth++) {

        data->bindToFramebuffer(depth);
        if(!drawAllToTexture(shader, depth, size))
            return;
    }
    data->operationFinished();
}

void SlabOperation::copy(DataTexturePair *source, GLuint target) {
    copyShader.use();
    source->bindData(GL_TEXTURE0);

    ivec3 size = source->getSize();
    for(int depth = 0; depth < size.z; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, 0, depth);

        if(!drawAllToTexture(copyShader, depth, size))
            return;
    }
}

bool SlabOperation::drawAllToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "fire.simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(0, 0, size.x, size.y);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);
    //shader.use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}

bool SlabOperation::drawInteriorToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "fire.simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(1, 1, size.x - 2, size.y - 2);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}

bool SlabOperation::drawBoundaryToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "fire.simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(0, 0, size.x, size.y);
    glBindVertexArray(boundaryVAO);
    glLineWidth(1.99f);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}