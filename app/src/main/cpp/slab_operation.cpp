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
    initPressure();
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
    for(int i = 0; i < size; i++) {
                data[i * 3] = 0.0;
                data[i * 3 + 1] = 1.0;
                data[i * 3 + 2] = 0.0;
    }
    create3DTextureV(&velocityMatrix, grid_width, grid_height, grid_depth, data);
    create3DTextureV(&resultVMatrix, grid_width, grid_height, grid_depth, NULL);
    delete[] data;
}

void SlabOperator::initPressure(){
    float* data = new float[grid_width*grid_height*grid_depth];
    //int b = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < grid_width*grid_height*grid_depth; i++) {
        data[i] = 0.0;
    }

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
    float* presSource = new float[size];

    for(int i = 0; i < size; i++)
        presSource[i] = 0.0f;

    presSource[16*16*8+16*8+8] = 1.0f;

    int radius = grid_width/10;
    for(int x = - radius; x <= radius; x++){
        int dz = (int) std::round(sqrt(radius * radius - x * x));
        for(int z = -dz; z <= dz; z++){
            //I have not verified that this is the correct way to calculate the index
            int index = x + grid_width*(0 + grid_height*z);

            tempSource[index] = 0;
            velSource[3*index + 1] = 0;
        }
    }

    create3DTexture(&tempSourceMatrix, grid_width, grid_height, grid_depth, tempSource);
    create3DTextureV(&velSourceMatrix, grid_width, grid_height, grid_depth, velSource);
    create3DTexture(&sourcePMatrix, grid_width, grid_height, grid_depth, presSource);

    delete[] tempSource;
    delete[] velSource;
    delete[] presSource;
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
    // Boundaries
    boundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/boundary.frag");
    FABInteriorShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_interior.frag");
    FABBoundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_boundary.frag");
    // Advection Shaders
    advectionShader.load("shaders/simulation/slab.vert", "shaders/simulation/advection/advection.frag");
    // Dissipate Shaders
    dissipateShader.load("shaders/simulation/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    additionShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/addition.frag");
    buoyancyShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/buoyancy.frag");
    // Projection Shaders
    divergenceShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/divergence.frag");
    jacobiShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/jacobi.frag");
    projectionShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/projection.frag");
    // Temperature Shaders
    temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
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

void SlabOperator::setBoundary(GLuint data, GLuint result, int scale){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);

        glViewport(0, 0, grid_width, grid_height);
        boundaryShader.use();
        glBindVertexArray(boundaryVAO);
        glLineWidth(2.0f);
        glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), depth);
        glUniform3f(glGetUniformLocation(boundaryShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
        glUniform1f(glGetUniformLocation(boundaryShader.program(), "scale"), scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, data);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    }
    // Front
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, 0);

    glViewport(0, 0, grid_width, grid_height);
    FABBoundaryShader.use();
    glBindVertexArray(boundaryVAO);
    glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), 0);
    glUniform3f(glGetUniformLocation(boundaryShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
    glUniform1f(glGetUniformLocation(boundaryShader.program(), "scale"), scale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, data);
    glLineWidth(2.0f);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

    glViewport(1, 1, grid_width - 2, grid_height - 2);
    FABInteriorShader.use();
    glBindVertexArray(interiorVAO);
    glLineWidth(2.0f);
    glUniform1i(glGetUniformLocation(FABInteriorShader.program(), "depth"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //Back
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, grid_depth - 1);

    glViewport(0, 0, grid_width, grid_height);
    FABBoundaryShader.use();
    glBindVertexArray(boundaryVAO);
    glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), grid_depth - 1);
    glUniform3f(glGetUniformLocation(boundaryShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
    glUniform1f(glGetUniformLocation(boundaryShader.program(), "scale"), scale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, data);
    glLineWidth(2.0f);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

    glViewport(1, 1, grid_width - 2, grid_height - 2);
    FABInteriorShader.use();
    glBindVertexArray(interiorVAO);
    glUniform1i(glGetUniformLocation(boundaryShader.program(), "depth"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void SlabOperator::buoyancy(float dt){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultVMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

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
    setBoundary(velocityMatrix, resultVMatrix, 1);
    swapData(velocityMatrix, resultVMatrix);
}

void SlabOperator::advection(GLuint data, GLuint result, float dt) {
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        advectionShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(advectionShader.program(), "depth"), depth);
        glUniform1f(glGetUniformLocation(advectionShader.program(), "dt"), dt);
        glUniform1f(glGetUniformLocation(advectionShader.program(), "dh"), 1.0f/grid_width);
        glUniform3f(glGetUniformLocation(advectionShader.program(), "gridSize"), grid_width, grid_height, grid_depth);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, data);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    setBoundary(data, result, 0);
    swapData(data, result);
}

void SlabOperator::divergence(){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultDMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        divergenceShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(divergenceShader.program(), "depth"), depth);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    setBoundary(divMatrix, resultDMatrix, 0);
    swapData(divMatrix, resultDMatrix);
}

void SlabOperator::jacobi(){
    for(int i = 0; i < 20; i++){
        for(int depth = 1; depth < grid_depth - 1; depth++){
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultPMatrix, 0, depth);
            glClear(GL_COLOR_BUFFER_BIT);

            // Interior
            glViewport(1, 1, grid_width - 2, grid_height - 2);
            jacobiShader.use();
            glBindVertexArray(interiorVAO);

            glUniform1i(glGetUniformLocation(jacobiShader.program(), "depth"), depth);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, pressureMatrix);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, divMatrix);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        setBoundary(pressureMatrix, resultPMatrix, 0);
        swapData(pressureMatrix, resultPMatrix);
    }
}

void SlabOperator::proj(){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultVMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        projectionShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(projectionShader.program(), "depth"), depth);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, velocityMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    setBoundary(velocityMatrix, resultVMatrix, -1);
    swapData(velocityMatrix, resultVMatrix);
}

void SlabOperator::velocityStep(float dt){
    // Force
    //buoyancy(dt);
    // Transport
    //advection(velocityMatrix, resultVMatrix, dt);
    // Project
    //divergence();
    //jacobi();
    //proj();
}

void SlabOperator::addition(float dt){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultPMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        additionShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(additionShader.program(), "depth"), depth);
        glUniform1f(glGetUniformLocation(additionShader.program(), "dt"), dt);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, sourcePMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    setBoundary(pressureMatrix, resultPMatrix, 0);
    swapData(pressureMatrix, resultPMatrix);
}

void SlabOperator::dissipate(float dt){
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultPMatrix, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        glViewport(1, 1, grid_width - 2, grid_height - 2);
        dissipateShader.use();
        glBindVertexArray(interiorVAO);

        glUniform1i(glGetUniformLocation(dissipateShader.program(), "depth"), depth);
        glUniform1f(glGetUniformLocation(dissipateShader.program(), "dt"), dt);
        glUniform1f(glGetUniformLocation(dissipateShader.program(), "dissipation_rate"), 0.15f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    setBoundary(pressureMatrix, resultPMatrix, 0);
    swapData(pressureMatrix, resultPMatrix);
}

void SlabOperator::pressureStep(float dt){
    // Source
    addition(dt);
    // Transport
    advection(pressureMatrix, resultPMatrix, dt);
    // Dissipate
    //dissipate(dt);
}



