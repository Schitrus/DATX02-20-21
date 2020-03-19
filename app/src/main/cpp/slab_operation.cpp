//
// Created by Anton Forsberg on 18/02/2020.
//
#include "slab_operation.h"

#include <jni.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <string>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "helper.h"

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace glm;

#define PI 3.14159265359f

int screen_width;
int screen_height;

// fbo
GLuint slabFBO = UINT32_MAX;
GLuint resultTarget;

// result // todo remove
GLuint resultShaderProgram;
GLuint texcoordsBuffer;

// matrices
GLuint dataMatrix, velocityMatrix, densityMatrix, pressureMatrix, temperatureMatrix;
GLuint resultMatrix, resultVMatrix, resultDMatrix, resultPMatrix, resultTMatrix, divMatrix;

// interior
GLuint interiorShaderProgram;
GLuint interiorVAO;
GLuint interiorPositionBuffer;
GLuint interiorIndexBuffer;

// boundary
GLuint boundaryShaderProgram;
GLuint boundaryVAO;
GLuint boundaryPositionBuffer;

// front and back face
GLuint frontAndBackInteriorShaderProgram;
GLuint frontAndBackBoundaryShaderProgram;

// simulation
GLuint dissipateShaderProgram, divergenceShaderProgram, jacobiShaderProgram, projectionShaderProgram;

void SlabOperator::init() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    resize(16, 16, 16);

    createMatrixFBO(grid_width, grid_height, &slabFBO, &resultTarget);
    initData();

    initQuad();
    initLine();
    initProgram();
}

void SlabOperator::resize(int width, int height, int depth){
    grid_width = width;
    grid_height = height;
    grid_depth = depth;
}

void SlabOperator::initData() {
    int size = grid_width * grid_height * grid_depth;
    initVelocity(size);
    // todo remove ordinary data array
    float* data = new float[size];
    //int b = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < size; ++i) { // todo fix crash on large arrays
        data[i] = 0.0f;
    }
    initPressure(data);
    initDensity(data);
    initTemperature(data);

    create3DTexture(&dataMatrix, grid_width, grid_height, grid_depth, data);

    create3DTexture(&resultMatrix, grid_width, grid_height, grid_depth, NULL);
    create3DTexture(&divMatrix, grid_width, grid_height, grid_depth, NULL);

    delete[] data;
}

void SlabOperator::initVelocity(int size){
    float* data = new float[size*3];
    for (int i = 0; i < size; ++i) { // todo fix crash on large arrays
        data[i] = 0.0f;
    }
    create3DTextureV(&velocityMatrix, grid_width, grid_height, grid_depth, data);
    create3DTextureV(&resultVMatrix, grid_width, grid_height, grid_depth, NULL);
    delete[] data;
}

void SlabOperator::initPressure(float* data){
    create3DTexture(&pressureMatrix, grid_width, grid_height, grid_depth, data);
    create3DTexture(&resultPMatrix, grid_width, grid_height, grid_depth, NULL);
}

void SlabOperator::initDensity(float* data){
    create3DTexture(&densityMatrix, grid_width, grid_height, grid_depth, data);
    create3DTexture(&resultDMatrix, grid_width, grid_height, grid_depth, NULL);
}

void SlabOperator::initTemperature(float* data){
    create3DTexture(&temperatureMatrix, grid_width, grid_height, grid_depth, data);
    create3DTexture(&resultTMatrix, grid_width, grid_height, grid_depth, NULL);
}


void SlabOperator::initLine() {
    glGenVertexArrays(1, &boundaryVAO);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(boundaryVAO);

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

void SlabOperator::initQuad() {

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

void SlabOperator::initProgram() {

    interiorShaderProgram = createProgram("shaders/simulation/advection/advection.vert", "shaders/simulation/advection/advection.frag");
    boundaryShaderProgram = createProgram("shaders/simulation/advection/advection.vert", "shaders/simulation/advection/boundary.frag");
    frontAndBackInteriorShaderProgram = createProgram("shaders/simulation/advection/advection.vert", "shaders/simulation/advection/front_and_back_interior.frag");
    frontAndBackBoundaryShaderProgram = createProgram("shaders/simulation/advection/advection.vert", "shaders/simulation/advection/front_and_back_boundary.frag");
    dissipateShaderProgram = createProgram("shaders/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    divergenceShaderProgram = createProgram("shaders/slab.vert", "shaders/simulation/projection/divergence.frag");
    jacobiShaderProgram = createProgram("shaders/slab.vert", "shaders/simulation/projection/jacobi.frag");
    projectionShaderProgram = createProgram("shaders/slab.vert", "shaders/simulation/projection/projection.frag");

    //frontAndBackInteriorShaderProgram = createProgram("shaders/slab.vert",
    //                                                  "shaders/front_and_back_interior.frag");
    //frontAndBackBoundaryShaderProgram = createProgram("shaders/slab.vert",
    //                                                  "shaders/front_and_back_boundary.frag");

    resultShaderProgram = createProgram("shaders/results.vert",
                                        "shaders/results.frag"); // todo remove

}

void SlabOperator::update() {
    slabOperation();
    //display_results(); // todo remove
}


void SlabOperator::slabOperation() {

    glBindFramebuffer(GL_FRAMEBUFFER, slabFBO);

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(interiorShaderProgram, boundaryShaderProgram, current_depth, 1.0f);
    }

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(divergenceShaderProgram, boundaryShaderProgram, current_depth, 1.0f);
    }
    for(int i = 0; i < 20; i++) {
        for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
            // todo fix so they are done at the same time
            slabOperation(jacobiShaderProgram, boundaryShaderProgram, current_depth, 1.0f);
        }
    }
    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(projectionShaderProgram, boundaryShaderProgram, current_depth, 1.0f);
    }
    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(dissipateShaderProgram, boundaryShaderProgram, current_depth, 1.0f);
    }

    slabOperation(frontAndBackInteriorShaderProgram, frontAndBackBoundaryShaderProgram, 0, 1.0f);

    slabOperation(frontAndBackInteriorShaderProgram, frontAndBackBoundaryShaderProgram, grid_depth - 1, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SlabOperator::slabOperation(GLuint interiorProgram, GLuint boundariesProgram, int layer, float scale) {
    GLuint temp = 0;

    if(interiorProgram == interiorShaderProgram) {
        resultMatrix = resultVMatrix;
        temp = dataMatrix;
        dataMatrix = velocityMatrix;
    } else if(frontAndBackInteriorShaderProgram) {
        resultMatrix = resultVMatrix;
        dataMatrix = velocityMatrix;
    } else if(divergenceShaderProgram) {
        resultMatrix = divMatrix;
        dataMatrix = velocityMatrix;
    } else if(jacobiShaderProgram) {
        resultMatrix = resultPMatrix;
        dataMatrix = pressureMatrix;
        temp = divMatrix;
    } else if(projectionShaderProgram) {
        resultMatrix = resultVMatrix;
        dataMatrix = velocityMatrix;
        temp = velocityMatrix;
    } else if(dissipateShaderProgram) {
        resultMatrix = resultPMatrix;
        dataMatrix = pressureMatrix;
    }

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultMatrix, 0, layer);

    glClear(GL_COLOR_BUFFER_BIT);

    //boundaries
    glViewport(0, 0, grid_width, grid_height);
    glUseProgram(boundariesProgram);
    glBindVertexArray(boundaryVAO);
    glLineWidth(10000.0f);
    glUniform1i(glGetUniformLocation(boundariesProgram, "depth"), layer);
    glUniform1i(glGetUniformLocation(boundariesProgram, "width"), grid_width);
    glUniform1i(glGetUniformLocation(boundariesProgram, "height"), grid_height);
    glUniform1f(glGetUniformLocation(boundariesProgram, "scale"), scale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    if(temp != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, temp);
    }
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);


    //interior
    glViewport(1, 1, grid_width - 2, grid_height - 2);
    glBindVertexArray(interiorVAO);
    glUseProgram(interiorProgram);
    // standard
    glUniform1i(glGetUniformLocation(interiorProgram, "depth"), layer);
    // advection
    glUniform1f(glGetUniformLocation(interiorProgram, "dt"), 1.0f/60.0f);
    glUniform1f(glGetUniformLocation(interiorProgram, "dh"), 1.0f);
    glUniform3f(glGetUniformLocation(interiorProgram, "gridSize"), grid_width, grid_height, grid_depth);
    // dissipate
    glUniform1f(glGetUniformLocation(interiorProgram, "aS"), 0.15f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    if(temp != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, temp);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SlabOperator::display_results() {
    // display result
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, screen_width, screen_height);
    glUseProgram(resultShaderProgram);
    glBindVertexArray(interiorVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, resultMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

