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

void SlabOperator::init() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    resize(16, 16, 16);
    FBO = new Framebuffer();
    FBO->create(grid_width, grid_height, true);
    initData();

    initQuad();
    initLine();
    initShaders();
}

void SlabOperator::resize(int width, int height, int depth){
    grid_width = width;
    grid_height = height;
    grid_depth = depth;
    FBO = new Framebuffer();
    FBO->create(grid_width, grid_height, true);
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
    initSources();

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

void SlabOperator::initSources(){
    int size = grid_width * grid_height * grid_depth;

    float* tempSource = new float[size];
    float* velSource = new float[3*size];

    int radius = grid_width/10;
    for(int x = - radius; x <= radius; x++){
        int dz = (int) std::round(sqrt(radius * radius - x * x));
        for(int z = -dz; z <= dz; z++){
            //I have not verified that this is the correct way to calculate the index
            int index = x + grid_width*(0 + grid_height*z);

            tempSource[index] = 1;
            velSource[3*index + 1] = 1;
        }
    }

    create3DTexture(&tempSourceMatrix, grid_width, grid_height, grid_depth, tempSource);
    create3DTextureV(&velSourceMatrix, grid_width, grid_height, grid_depth, velSource);

    delete[] tempSource;
    delete[] velSource;
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

void SlabOperator::initShaders() {
    // Advection Shaders
    advectionShader.load("shaders/slab.vert", "shaders/simulation/advection/advection.frag");
    boundaryShader.load("shaders/slab.vert", "shaders/simulation/advection/boundary.frag");
    FABInteriorShader.load("shaders/slab.vert", "shaders/simulation/advection/front_and_back_interior.frag");
    FABBoundaryShader.load("shaders/slab.vert", "shaders/simulation/advection/front_and_back_boundary.frag");
    // Dissipate Shaders
    dissipateShader.load("shaders/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    additionShader.load("shaders/slab.vert", "shaders/simulation/force/addition.frag");
    buoyancyShader.load("shaders/slab.vert", "shaders/simulation/force/buoyancy.frag");
    // Projection Shaders
    divergenceShader.load("shaders/slab.vert", "shaders/simulation/projection/divergence.frag");
    jacobiShader.load("shaders/slab.vert", "shaders/simulation/projection/jacobi.frag");
    projectionShader.load("shaders/slab.vert", "shaders/simulation/projection/projection.frag");
    // Temperature Shaders
    temperatureShader.load("shaders/slab.vert", "shaders/simulation/temperature/temperature.frag");
}

void SlabOperator::getData(GLuint& data, int& width, int& height, int& depth) {
    data = pressureMatrix;
    width = grid_width;
    height = grid_height;
    depth = grid_depth;
}

void SlabOperator::swapData(GLuint& d1, GLuint& d2){
    GLuint tmp = d1;
    d1 = d2;
    d2 = tmp;
}

void SlabOperator::setData(GLuint data, int width, int height, int depth){
    resize(width, height, depth);
    dataMatrix = data;
    create3DTexture(&resultMatrix, width, height, depth, NULL);
}

void SlabOperator::update() {
    // Setup GPU
    FBO->use();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // Do Operations
    float dt = 1.0f/30.0f;

    velocityStep(dt);

    pressureStep(dt);


    FBO->null();
}

void SlabOperator::slabOperation(Shader interiorProgram, Shader boundariesProgram, int layer, float scale) {
    GLuint tmp = 0;

    if(interiorProgram.program() == advectionShader.program()) {
        resultMatrix = resultVMatrix;
        tmp = dataMatrix;
        dataMatrix = velocityMatrix;
    } else if(interiorProgram.program() == FABInteriorShader.program()) {
        resultMatrix = resultVMatrix;
        dataMatrix = velocityMatrix;
    } else if(interiorProgram.program() == divergenceShader.program()) {
        resultMatrix = divMatrix;
        dataMatrix = velocityMatrix;
    } else if(interiorProgram.program() == jacobiShader.program()) {
        resultMatrix = resultPMatrix;
        dataMatrix = pressureMatrix;
        tmp = divMatrix;
    } else if(interiorProgram.program() == projectionShader.program()) {
        resultMatrix = resultVMatrix;
        dataMatrix = velocityMatrix;
        tmp = velocityMatrix;
    } else if(interiorProgram.program() == dissipateShader.program()) {
        resultMatrix = resultPMatrix;
        dataMatrix = pressureMatrix;
    }

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultMatrix, 0, layer);

    glClear(GL_COLOR_BUFFER_BIT);

    if(scale != 0.0f) {
        //boundaries
        glViewport(0, 0, grid_width, grid_height);
        boundariesProgram.use();
        glBindVertexArray(boundaryVAO);
        glLineWidth(10000.0f);
        glUniform1i(glGetUniformLocation(boundariesProgram.program(), "depth"), layer);
        glUniform1i(glGetUniformLocation(boundariesProgram.program(), "width"), grid_width);
        glUniform1i(glGetUniformLocation(boundariesProgram.program(), "height"), grid_height);
        glUniform1i(glGetUniformLocation(boundariesProgram.program(), "max_depth"), grid_depth);
        glUniform1f(glGetUniformLocation(boundariesProgram.program(), "scale"), scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, dataMatrix);
        if (tmp != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, tmp);
        }
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    }

    //interior
    glViewport(1, 1, grid_width - 2, grid_height - 2);
    glBindVertexArray(interiorVAO);
    interiorProgram.use();
    // standard
    glUniform1i(glGetUniformLocation(interiorProgram.program(), "depth"), layer);
    glUniform1i(glGetUniformLocation(interiorProgram.program(), "max_depth"), grid_depth);
    // advection
    glUniform1f(glGetUniformLocation(interiorProgram.program(), "dt"), 1.0f/60.0f);
    glUniform1f(glGetUniformLocation(interiorProgram.program(), "dh"), 1.0f/16.0f);
    glUniform3f(glGetUniformLocation(interiorProgram.program(), "gridSize"), grid_width, grid_height, grid_depth);
    // dissipate
    glUniform1f(glGetUniformLocation(interiorProgram.program(), "aS"), 0.15f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    if(tmp != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, tmp);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SlabOperator::buoyancy(float dt){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultVMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Exterior
        glViewport(0, 0, grid_width, grid_height);
        buoyancyShader.use();
        glBindVertexArray(boundaryVAO);

        glLineWidth(10000.0f);
        glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), depth);
        glUniform3f(glGetUniformLocation(boundaryShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
        glUniform1f(glGetUniformLocation(boundaryShader.program(), "scale"), 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        buoyancyShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(buoyancyShader.program(), "depth"), depth);
        glUniform1f(glGetUniformLocation(buoyancyShader.program(), "dt"), dt);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, temperatureMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    swapData(velocityMatrix, resultVMatrix);
}

void SlabOperator::advection(float dt) {
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultVMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Exterior
        glViewport(0, 0, grid_width, grid_height);
        boundaryShader.use();
        glBindVertexArray(boundaryVAO);

        glLineWidth(10000.0f);
        glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), depth);
        glUniform3f(glGetUniformLocation(boundaryShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
        glUniform1f(glGetUniformLocation(boundaryShader.program(), "scale"), 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        buoyancyShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(buoyancyShader.program(), "depth"), depth);
        glUniform1f(glGetUniformLocation(buoyancyShader.program(), "dt"), dt);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, temperatureMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    slabFABOperation(1.0f);

    swapData(velocityMatrix, resultVMatrix);
}

void SlabOperator::velocityStep(float dt){

    buoyancy(dt);

    advection(dt);

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(advectionShader, boundaryShader, current_depth, 1.0f);
    }
    slabFABOperation(1.0f);
    swapData(velocityMatrix, resultVMatrix);


    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(divergenceShader, boundaryShader, current_depth, 1.0f);
    }
    swapData(velocityMatrix, resultVMatrix);

    for(int i = 0; i < 20; i++) {
        for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
            // todo fix so they are done at the same time
            slabOperation(jacobiShader, boundaryShader, current_depth, 0.0f);
        }
        swapData(velocityMatrix, resultVMatrix);
    }

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(projectionShader, boundaryShader, current_depth, 0.0f);
    }
    slabFABOperation(1.0f);
    swapData(velocityMatrix, resultVMatrix);

}

void SlabOperator::pressureStep(float dt){
    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(additionShader, boundaryShader, current_depth, -1.0f);
    }
    slabFABOperation(-1.0f);
    swapData(pressureMatrix,resultPMatrix);

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(advectionShader, boundaryShader, current_depth, -1.0f);
    }
    slabFABOperation(-1.0f);
    swapData(pressureMatrix,resultPMatrix);

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {
        // todo fix so they are done at the same time
        slabOperation(dissipateShader, boundaryShader, current_depth, -1.0f);
    }
    slabFABOperation(-1.0f);
    swapData(pressureMatrix,resultPMatrix);
}

void SlabOperator::slabFABOperation(float scale){
    slabOperation(FABInteriorShader, FABBoundaryShader, 0, scale);
    slabOperation(FABInteriorShader, FABBoundaryShader, grid_depth - 1, scale);
}



