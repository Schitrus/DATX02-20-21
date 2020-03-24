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
    initJacobiMatrix();
    initDensity(data);
    initTemperature(data);
    initSources();

    createScalar3DTexture(&scalarResultMatrix, grid_width, grid_height, grid_depth, NULL);
    createVector3DTexture(&vectorResultMatrix, grid_width, grid_height, grid_depth, NULL);

    createScalar3DTexture(&divMatrix, grid_width, grid_height, grid_depth, NULL);

    delete[] data;
}

void SlabOperator::initVelocity(int size){
    float* data = new float[size*3];
    for (int i = 0; i < size; ++i) { // todo fix crash on large arrays
        data[i*3] = 0.0f;
        data[i*3+1] = 0.0f;
        data[i*3+2] = 1.0f;
    }
    createVector3DTexture(&velocityMatrix, grid_width, grid_height, grid_depth, data);
    delete[] data;
}

void SlabOperator::initJacobiMatrix(){
    float* data = new float[grid_width*grid_height*grid_depth];
    //int b = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < grid_width*grid_height*grid_depth; i++) {
        data[i] = 0.25f;
    }

    createScalar3DTexture(&jacobiMatrix, grid_width, grid_height, grid_depth, data);
}

void SlabOperator::initDensity(float* data){
    createScalar3DTexture(&densityMatrix, grid_width, grid_height, grid_depth, data);
}

void SlabOperator::initTemperature(float* data){
    createScalar3DTexture(&temperatureMatrix, grid_width, grid_height, grid_depth, data);
}

void SlabOperator::initSources(){
    int size = grid_width * grid_height * grid_depth;

    float* tempSource = new float[size];
    float* velSource = new float[3*size];
    float* centerSource = new float[size];

    for(int i = 0; i < size; i++) {
        tempSource[i] = 0.0f;
        centerSource[i] = 0.0f;

        velSource[3*i] = 0.0f;
        velSource[3*i + 1] = 0.0f;
        velSource[3*i + 2] = 0.0f;
    }

    centerSource[size / 2] = 1.0f;

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

    //createScalar3DTexture(&tempSourceMatrix, grid_width, grid_height, grid_depth, centerSource);
    createScalar3DTexture(&tempSourceMatrix, grid_width, grid_height, grid_depth, tempSource);
    createVector3DTexture(&velSourceMatrix, grid_width, grid_height, grid_depth, velSource);

    delete[] tempSource;
    delete[] velSource;
    delete[] centerSource;
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
    projectionShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/gradient_subtraction.frag");
    // Temperature Shaders
    temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
}

void SlabOperator::getData(GLuint& data, int& width, int& height, int& depth) {
    data = jacobiMatrix;
    width = grid_width;
    height = grid_height;
    depth = grid_depth;
}

void SlabOperator::swapData(GLuint& d1, GLuint& d2){
    GLuint tmp = d1;
    d1 = d2;
    d2 = tmp;
}

void SlabOperator::update() {
    // Setup GPU
    FBO->use();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // Do Operations
    float dt = 1.0f/30.0f;
    float dh = 1.0f/16.0f;

    //Simulate changes in velocity
    velocityStep(dh, dt);

    temperatureStep(dh, dt);

    //Move any other substances along the velocity field

    FBO->null();
}

void SlabOperator::setBoundary(GLuint data, GLuint result, int scale){

    boundaryShader.use();
    boundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    boundaryShader.uniform1f("scale", scale);
    bind3DTexture0(data);

    for(int depth = 1; depth < grid_depth - 1; depth++){
        prepareResult(result, depth);
        drawBoundaryToTexture(boundaryShader, depth);
    }

    // Front
    setFrontOrBackBoundary(data, result, scale, 0);

    //Back
    setFrontOrBackBoundary(data, result, scale, grid_depth - 1);
}

void SlabOperator::setFrontOrBackBoundary(GLuint data, GLuint result, int scale, int depth){
    bind3DTexture0(data);

    prepareResult(result, depth);

    FABBoundaryShader.use();
    FABBoundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    FABBoundaryShader.uniform1f("scale", scale);
    drawBoundaryToTexture(FABBoundaryShader,  depth);

    FABInteriorShader.use();
    drawInteriorToTexture(FABInteriorShader, depth);
}

void SlabOperator::buoyancy(float dt){
    buoyancyShader.use();
    buoyancyShader.uniform1f("dt", dt);

    bind3DTexture0(temperatureMatrix);
    bind3DTexture1(velocityMatrix);

    performOperation(buoyancyShader, velocityMatrix, true, 1);
}

void SlabOperator::advection(GLuint &data, bool isVectorField, float dh, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform1f("dh", dh);
    advectionShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);

    bind3DTexture0(velocityMatrix);
    bind3DTexture1(data);

    performOperation(advectionShader, data, isVectorField, 0);
}

void SlabOperator::projection(GLuint &target) {
    divergence(target, divMatrix);
    jacobi(divMatrix, jacobiMatrix);
    subtractGradient(target, jacobiMatrix);
}

void SlabOperator::divergence(GLuint target, GLuint &result){
    divergenceShader.use();

    bind3DTexture0(target);

    performOperation(divergenceShader, result, false, 0);
}

void SlabOperator::jacobi(GLuint divergence, GLuint &jacobi){
    jacobiShader.use();

    bind3DTexture0(jacobi);
    bind3DTexture1(divergence);

    for(int i = 0; i < 20; i++){
        performOperation(jacobiShader, jacobi, false, 0);
    }
}

void SlabOperator::subtractGradient(GLuint &target, GLuint scalarField){

    projectionShader.use();

    bind3DTexture0(scalarField);
    bind3DTexture1(target);

    performOperation(projectionShader, target, true, -1);
}

void SlabOperator::velocityStep(float dh, float dt){
    // Force
    buoyancy(dt);
    // Transport
    advection(velocityMatrix, true, dh, dt);
    // Project
    projection(velocityMatrix);
}

void SlabOperator::temperatureStep(float dh, float dt) {
    //Temperature source
    addition(temperatureMatrix, tempSourceMatrix, false, dt);

    // todo is it reasonable to perform heat dissipation through our dissipate shader?
    substanceMovementStep(temperatureMatrix, 0, dh, dt);
}

void SlabOperator::substanceMovementStep(GLuint &target, float dissipationRate, float dh, float dt){

    advection(target, false, dh, dt);

    // Usually there is also a diffusion step for fluid simulation here.
    // However we assume that all fluids we simulate has a diffusion term of zero,
    // removing the need of this simulation step

    if(dissipationRate != 0)
        dissipate(target, dissipationRate, dt);
}

void SlabOperator::addition(GLuint &target, GLuint &source, bool isVectorField, float dt){
    additionShader.use();
    additionShader.uniform1f("dt", dt);

    bind3DTexture0(target);
    bind3DTexture1(source);

    performOperation(additionShader, target, isVectorField, 0);
}

void SlabOperator::dissipate(GLuint &target, float dissipationRate, float dt){
    dissipateShader.use();
    dissipateShader.uniform1f("dt", dt);
    dissipateShader.uniform1f("dissipation_rate", dissipationRate);

    bind3DTexture0(target);

    performOperation(dissipateShader, target, false, 0);
}

void SlabOperator::performOperation(Shader shader, GLuint &target, bool isVectorField, int boundaryScale) {

    GLuint &result = isVectorField ? vectorResultMatrix : scalarResultMatrix;

    for(int depth = 1; depth < grid_depth - 1; depth++){
        prepareResult(result, depth);
        drawInteriorToTexture(shader, depth);
    }

    setBoundary(target, result, boundaryScale);
    swapData(target, result);
}

void SlabOperator::bind3DTexture0(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);
}

void SlabOperator::bind3DTexture1(GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, texture);
}

void SlabOperator::prepareResult(GLuint result, int depth) {
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);
    glClear(GL_COLOR_BUFFER_BIT);
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
    glLineWidth(10000.0f);

    shader.uniform1i("depth", depth);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
}