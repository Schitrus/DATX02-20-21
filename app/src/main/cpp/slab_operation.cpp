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
#include "simulator.h"

#define LOG_TAG "Renderer"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

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
        LOG_ERROR("Failed to compile slab_operation shaders");
        return 0;
    }
    return 1;

}

void SlabOperator::initData() {
    createVector3DTexture(&diffusionBHRTexture, highResSize, (vec3*)nullptr);
    createVector3DTexture(&diffusionBLRTexture, lowResSize, (vec3*)nullptr);

    divergence = createScalarDataPair(false, (float*)nullptr);

    jacobi = createScalarDataPair(false, (float*)nullptr);
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
    success &= dissipateShader.load("shaders/simulation/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    success &= addSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_source.frag");
    success &= setSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/set_source.frag");
    success &= buoyancyShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/buoyancy.frag");
    success &= windShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_wind.frag");
    // Projection Shaders
    success &= divergenceShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/divergence.frag");
    success &= jacobiShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/jacobi.frag");
    success &= gradientShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/gradient_subtraction.frag");
    // Vorticity Shaders
    success &= vorticityShader.load("shaders/simulation/slab.vert", "shaders/simulation/vorticity/vorticity.frag");
    // Temperature Shaders
    success &= temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
    // Utilities
    success &= copyShader.load("shaders/simulation/slab.vert", "shaders/simulation/copy.frag");
    return success;
}

void SlabOperator::setBoundary(DataTexturePair* data, int scale) {
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
    for(int depth = 1; depth < grid_depth - 1; depth++){
        data->bindToFramebuffer(depth);

        if(!drawInteriorToTexture(copyShader, depth))
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

bool SlabOperator::drawFrontOrBackBoundary(DataTexturePair* data, int scale, int depth){
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

void SlabOperator::heatDissipation(DataTexturePair* temperature, float dt){
    temperatureShader.use();
    temperatureShader.uniform1f("dt", dt);
    temperature->bindData(GL_TEXTURE0);

    fullOperation(temperatureShader, temperature);
}

// todo how (if in any way) should the two source functions apply border restrictions
void SlabOperator::addSource(DataTexturePair* data, GLuint& source, float dt) {
    addSourceShader.use();
    addSourceShader.uniform1f("dt", dt);
    data->bindData(GL_TEXTURE0);
    bindData(source, GL_TEXTURE1);

    fullOperation(addSourceShader, data);
}

void SlabOperator::setSource(DataTexturePair* data, GLuint& source, float dt) {
    setSourceShader.use();
    setSourceShader.uniform1f("dt", dt);
    data->bindData(GL_TEXTURE0);
    bindData(source, GL_TEXTURE1);

    fullOperation(setSourceShader, data);
}

void SlabOperator::buoyancy(DataTexturePair* velocity, DataTexturePair* temperature, float dt, float scale){
    buoyancyShader.use();
    buoyancyShader.uniform1f("dt", dt);
    buoyancyShader.uniform1f("scale", scale);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
    buoyancyShader.uniform3f("temp_border_width", vec3(1)/vec3(temperature->getSize()));
#pragma clang diagnostic pop
    buoyancyShader.uniform3f("gridSize", velocity->getSize());
    temperature->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    interiorOperation(buoyancyShader, velocity, -1);
}

void SlabOperator::diffuse(DataTexturePair* velocity, int iterationCount, float kinematicViscosity, float dt) {

    GLuint diffusionBTexture = velocity->isUsingHighRes() ? diffusionBHRTexture : diffusionBLRTexture;
    copy(velocity, diffusionBTexture);

    float dx = 1.0f / velocity->toVoxelScaleFactor();
    float alpha = (dx*dx) / (kinematicViscosity * dt);
    float beta = 6.0f + alpha; // For 3D grids

    jacobiIteration(velocity, diffusionBTexture, iterationCount, alpha, beta, -1);
}

void SlabOperator::dissipate(DataTexturePair* data, float dissipationRate, float dt){

    dissipateShader.use();
    dissipateShader.uniform1f("dt", dt);
    dissipateShader.uniform1f("dissipation_rate", dissipationRate);
    data->bindData(GL_TEXTURE0);

    interiorOperation(dissipateShader, data, 0);
}

void SlabOperator::advection(DataTexturePair* velocity, DataTexturePair* data, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform1f("meterToVoxels", velocity->toVoxelScaleFactor());
    advectionShader.uniform3f("gridSize", velocity->getSize());
    velocity->bindData(GL_TEXTURE0);
    data->bindData(GL_TEXTURE1);

    interiorOperation(advectionShader, data, -1);
}
void SlabOperator::fulladvection(DataTexturePair* velocity, DataTexturePair* data, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform1f("meterToVoxels", velocity->toVoxelScaleFactor());
    advectionShader.uniform3f("gridSize", velocity->getSize());
    velocity->bindData(GL_TEXTURE0);
    data->bindData(GL_TEXTURE1);

    interiorOperation(advectionShader, data);
}
void SlabOperator::projection(DataTexturePair* velocity, int iterationCount){
    float dx = 1.0f/velocity->toVoxelScaleFactor();
    float alpha = -(dx*dx);
    float beta = 6.0f;

    int zSize = velocity->getSize().z;
    // Clear gradient texture, unsure if needed?
    for(int depth = 0; depth < zSize; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, jacobi->getDataTexture(), 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    createDivergence(velocity, dx);
    jacobiIteration(jacobi, divergence->getDataTexture(), iterationCount, alpha, beta, 1);
    subtractGradient(velocity, dx);
}

void SlabOperator::vorticity(DataTexturePair *velocity, float vorticityScale, float dt) {
    vorticityShader.use();
    vorticityShader.uniform1f("dt", dt);
    vorticityShader.uniform1f("vorticityScale", vorticityScale);
    velocity->bindData(GL_TEXTURE0);

    interiorOperation(vorticityShader, velocity, -1);
}

void SlabOperator::createDivergence(DataTexturePair* vectorData, float dx) {
    divergenceShader.use();
    divergenceShader.uniform1f("dh", dx);
    vectorData->bindData(GL_TEXTURE0);

    interiorOperation(divergenceShader, divergence, 1);
}

void SlabOperator::jacobiIteration(DataTexturePair *xTexturePair, GLuint bTexture,
                            int iterationCount, float alpha, float beta, int scale){

    bindData(bTexture, GL_TEXTURE1);
    for(int i = 0; i < iterationCount; i++){
        jacobiShader.use();
        jacobiShader.uniform1f("alpha", alpha);
        jacobiShader.uniform1f("beta", beta);
        xTexturePair->bindData(GL_TEXTURE0);

        interiorOperation(jacobiShader, xTexturePair, scale);
    }
}

void SlabOperator::subtractGradient(DataTexturePair* velocity, float dx){
    gradientShader.use();
    gradientShader.uniform1f("dh", dx);
    jacobi->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    interiorOperation(gradientShader, velocity, -1);
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

void SlabOperator::addEdgeWind(DataTexturePair* velocity, float wind, float dt) {
    //Step one: apply wind to depth layer 1 while rendering to the result texture
    windShader.use();
    windShader.uniform1f("dt", dt);
    windShader.uniform1f("wind", wind);
    velocity->bindData(GL_TEXTURE0);
    velocity->bindToFramebuffer(1);
    if(!drawInteriorToTexture(windShader, 1, velocity->getSize()))
        return;
    //Step two: instead of copying the rest of the data to the result texture, let's copy the depth layer with wind back to the data
    velocity->operationFinished();
    copyShader.use();
    velocity->bindData(GL_TEXTURE0);
    velocity->bindToFramebuffer(1);
    drawInteriorToTexture(windShader, 1, velocity->getSize());
    //The current result is the original data, so even if rendering failed or not, we should switch back
    velocity->operationFinished();
}

void SlabOperator::addWind(DataTexturePair* velocity, float wind_angle, float wind_strength, float dt) {
    windShader.use();
    windShader.uniform1f("dt", dt);
    windShader.uniform1f("wind_angle", wind_angle);
    windShader.uniform1f("wind_strength", wind_strength);
    velocity->bindData(GL_TEXTURE0);

    interiorOperation(windShader, velocity, -1);
}

void SlabOperator::interiorOperation(Shader shader, DataTexturePair* data, int boundaryScale) {
    ivec3 size = data->getSize();
    for(int depth = 1; depth < size.z - 1; depth++) {

        data->bindToFramebuffer(depth);
        if(!drawInteriorToTexture(shader, depth, size))
            return;
    }
    data->operationFinished();

    setBoundary(data, boundaryScale);
}

void SlabOperator::fullOperation(Shader shader, DataTexturePair* data) {
    ivec3 size = data->getSize();
    for(int depth = 0; depth < size.z; depth++) {

        data->bindToFramebuffer(depth);
        if(!drawAllToTexture(shader, depth, size))
            return;
    }
    data->operationFinished();
}

void SlabOperator::copy(DataTexturePair *source, GLuint target) {
    copyShader.use();
    source->bindData(GL_TEXTURE0);

    ivec3 size = source->getSize();
    for(int depth = 0; depth < size.z; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, 0, depth);

        if(!drawAllToTexture(copyShader, depth, size))
            return;
    }
}

void SlabOperator::bindData(GLuint dataTexture, GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}

bool SlabOperator::drawAllToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(0, 0, size.x, size.y);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);
    //shader.use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}

bool SlabOperator::drawInteriorToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(1, 1, size.x - 2, size.y - 2);
    glBindVertexArray(interiorVAO);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}

bool SlabOperator::drawBoundaryToTexture(Shader shader, int depth, ivec3 size) {
    if(!checkFramebufferStatus(GL_FRAMEBUFFER, "simulation"))
        return false;
    clearGLErrors("slab operation");
    glViewport(0, 0, size.x, size.y);
    glBindVertexArray(boundaryVAO);
    glLineWidth(1.99f);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    return checkGLError("slab operation");
}