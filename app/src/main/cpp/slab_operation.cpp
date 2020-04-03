//
// Created by Anton Forsberg on 18/02/2020.
//
#include "slab_operation.h"

#include <jni.h>
#include <time.h>
#include <math.h>
#include <string>

#include <gles3/gl31.h>
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

int SlabOperator::init() {

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    FBO = new SimpleFramebuffer();
    FBO->init();

    initData();

    initQuad();
    initLine();

    if(!initShaders()) {
        ALOGE("Failed to compile slab_operation shaders");
        return 0;
    }
    return 1;

}

void SlabOperator::initSize(int width, int height, int depth){
    grid_width = width + 2;
    grid_height = height + 2;
    grid_depth = depth + 2;
}

void SlabOperator::initData() {
    divergence = createScalarDataPair(grid_width, grid_height, grid_depth, (float*)nullptr);

    jacobi = createScalarDataPair(grid_width, grid_height, grid_depth, (float*)nullptr);
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

int SlabOperator::initShaders() {
    bool success = true;
    // Boundaries
    success &= boundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/boundary.frag");
    success &= FABInteriorShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_interior.frag");
    success &= FABBoundaryShader.load("shaders/simulation/slab.vert", "shaders/simulation/front_and_back_boundary.frag");
    // Advection Shaders
    success &= advectionShader.load("shaders/simulation/slab.vert", "shaders/simulation/advection/advection.frag");
    // Dissipate Shaders
    success &= diffuseShader.load(  "shaders/simulation/slab.vert", "shaders/simulation/diffuse/diffuse.frag");
    success &= dissipateShader.load("shaders/simulation/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    success &= addSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_source.frag");
    success &= setSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/set_source.frag");
    success &= buoyancyShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/buoyancy.frag");
    // Projection Shaders
    success &= divergenceShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/divergence.frag");
    success &= jacobiShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/jacobi.frag");
    success &= gradientShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/gradient_subtraction.frag");
    // Temperature Shaders
    success &= temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
    return success;
}

void SlabOperator::setBoundary(DataTexturePair* data, int scale) {
    // Input data used in all steps
    data->bindData(GL_TEXTURE0);

    boundaryShader.use();
    boundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    boundaryShader.uniform1f("scale", scale);

    for(int depth = 1; depth < grid_depth - 1; depth++){
        data->bindToFramebuffer(depth);

        drawBoundaryToTexture(boundaryShader, depth);
    }
    // Front
    drawFrontOrBackBoundary(data, scale, 0);

    //Back
    drawFrontOrBackBoundary(data, scale, grid_depth - 1);

    data->operationFinished();
}

void SlabOperator::drawFrontOrBackBoundary(DataTexturePair* data, int scale, int depth){
    data->bindToFramebuffer(depth);

    FABBoundaryShader.use();
    FABBoundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    FABBoundaryShader.uniform1f("scale", scale);
    drawBoundaryToTexture(FABBoundaryShader,  depth);

    FABInteriorShader.use();
    drawInteriorToTexture(FABInteriorShader, depth);
}

void SlabOperator::prepare() {

    // Setup GPU
    FBO->bind();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void SlabOperator::finish() {
    FBO->unbind();
}

void SlabOperator::temperatureOperation(DataTexturePair* temperature, DataTexturePair* velocity, float dt){
    temperatureShader.use();
    temperatureShader.uniform1f("dt", dt);
    temperatureShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    velocity->bindData(GL_TEXTURE0);
    temperature->bindData(GL_TEXTURE1);

    fullOperation(temperatureShader, temperature);

    //setBoundary(temperature, 0);
}

// todo how (if in any way) should the two source functions apply border restrictions
void SlabOperator::addSource(DataTexturePair* data, GLuint& source, float dt) {
    addSourceShader.use();
    addSourceShader.uniform1f("dt", dt);
    data->bindData(GL_TEXTURE0);
    bindData(source, GL_TEXTURE1);

    interiorOperation(addSourceShader, data);

    //setBoundary(data, 0);
}

void SlabOperator::setSource(DataTexturePair* data, GLuint& source, float dt) {
    setSourceShader.use();
    setSourceShader.uniform1f("dt", dt);
    data->bindData(GL_TEXTURE0);
    bindData(source, GL_TEXTURE1);

    fullOperation(setSourceShader, data);

    //setBoundary(data, 0);
}

void SlabOperator::buoyancy(DataTexturePair* velocity, DataTexturePair* temperature, float dt, float scale){
    buoyancyShader.use();
    buoyancyShader.uniform1f("dt", dt);
    buoyancyShader.uniform1f("scale", scale);
    temperature->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    interiorOperation(buoyancyShader, velocity);

    //setBoundary(velocity, 1);
}

void SlabOperator::diffuse(DataTexturePair* data, int iterationCount, float diffusionConstant, float dt) {
    diffuseShader.use();
    diffuseShader.uniform1f("dt", dt);
    diffuseShader.uniform1f("diffusion_constant", diffusionConstant);

    for(int i = 0; i < iterationCount; i++) {
        /*bind3DTexture0(data);
        bind3DTexture1(result); // todo fix bad data. Should not use a texture as both input and output*/
        interiorOperation(diffuseShader, data);

        //setBoundary(data, 0);
    }
}

void SlabOperator::dissipate(DataTexturePair* data, float dissipationRate, float dt){

    dissipateShader.use();
    dissipateShader.uniform1f("dt", dt);
    dissipateShader.uniform1f("dissipation_rate", dissipationRate);
    data->bindData(GL_TEXTURE0);

    interiorOperation(dissipateShader, data);

    //setBoundary(data, 0);
}

void SlabOperator::advection(DataTexturePair* velocity, DataTexturePair* data, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    velocity->bindData(GL_TEXTURE0);
    data->bindData(GL_TEXTURE1);

    interiorOperation(advectionShader, data);

    //setBoundary(data, 0);
}
void SlabOperator::fulladvection(DataTexturePair* velocity, DataTexturePair* data, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    velocity->bindData(GL_TEXTURE0);
    data->bindData(GL_TEXTURE1);

    fullOperation(advectionShader, data);
}
void SlabOperator::projection(DataTexturePair* velocity){
    createDivergence(velocity);
    jacobiIteration(20);
    subtractGradient(velocity);
}

void SlabOperator::createDivergence(DataTexturePair* vectorData) {
    divergenceShader.use();
    vectorData->bindData(GL_TEXTURE0);

    interiorOperation(divergenceShader, divergence);

    //setBoundary(divergence, 0);
}

void SlabOperator::jacobiIteration(int iterationCount) {

    for(int depth = 0; depth < grid_depth; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, jacobi->getDataTexture(), 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    jacobiShader.use();
    divergence->bindData(GL_TEXTURE1);

    for(int i = 0; i < iterationCount; i++) {
        jacobi->bindData(GL_TEXTURE0);
        interiorOperation(jacobiShader, jacobi);

        //setBoundary(gradient, 0);
    }
}

void SlabOperator::subtractGradient(DataTexturePair* velocity){
    gradientShader.use();
    jacobi->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    interiorOperation(gradientShader, velocity);

  //  setBoundary(velocity, 0);
}
/*
void SlabOperator::substanceMovementStep(GLuint &target, GLuint& result, float dissipationRate, float dh, float dt){

    advection(target, result, dt);

    // Usually there is also a diffusion step for fluid simulation here.
    // However we assume that all fluids we simulate has a diffusion term of zero,
    // removing the need of this simulation step

    if(dissipationRate != 0)
        dissipate(target, result, dissipationRate, dt);
}
*/


void SlabOperator::interiorOperation(Shader shader, DataTexturePair* data) {
    for(int depth = 1; depth < grid_depth - 1; depth++){
        data->bindToFramebuffer(depth);

        // Interior
        drawInteriorToTexture(shader, depth);
    }
    data->operationFinished();
}

void SlabOperator::fullOperation(Shader shader, DataTexturePair* data) {
    for(int depth = 0; depth < grid_depth; depth++){
        data->bindToFramebuffer(depth);

        // Interior
        drawAllToTexture(shader, depth);
    }
    data->operationFinished();
}

void SlabOperator::bindData(GLuint dataTexture, GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}


void SlabOperator::drawAllToTexture(Shader shader, int depth) {
    glViewport(0, 0, grid_width, grid_height);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SlabOperator::drawInteriorToTexture(Shader shader, int depth) {
    glViewport(1, 1, grid_width - 2, grid_height - 2);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SlabOperator::drawBoundaryToTexture(Shader shader, int depth) {
    glViewport(0, 0, grid_width, grid_height);
    glBindVertexArray(boundaryVAO);
    glLineWidth(1.99f);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
}