//
// Created by Anton Forsberg on 18/02/2020.
//
#include "slab_operation.h"

#include <jni.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <string>

#include <GLES3/gl32.h>
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

#define NOW std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::now())
#define DURATION(a, b) (std::chrono::duration_cast<std::chrono::milliseconds>(a - b)).count() / 1000.0f;

using namespace glm;

#define PI 3.14159265359f

void SlabOperator::init() {

    start_time = NOW;
    last_time = start_time;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    resize(12, 48, 12);
    initData();

    initQuad();
    initLine();
    initShaders();
}

void SlabOperator::resize(int width, int height, int depth){
    grid_width = width + 2;
    grid_height = height + 2;
    grid_depth = depth + 2;
    FBO = new Framebuffer();
    FBO->create(grid_width, grid_height, true);
}

void SlabOperator::initData() {
    int size = grid_width * grid_height * grid_depth;

    float* density_field = new float[size];
    float* density_source = new float[size];
    float* temperature_field = new float[size];
    float* temperature_source = new float[size];
    vec3* velocity_field = new vec3[size];
    vec3* velocity_source = new vec3[size];

    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                density_field[index]      = 0.0f;
                density_source[index]     = 0.0f;
                temperature_field[index]  = 0.0f;
                temperature_source[index] = 0.0f;
                velocity_field[index]     = vec3(0, 0, 0);
                velocity_source[index]    = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    for (int z = 1; z < grid_depth - 1; z++) {
        for (int y = 1; y < grid_height - 1; y++) {
            for (int x = 1; x < grid_width - 1; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                velocity_source[index] = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    int z = grid_depth/2-2, y = 2, x = grid_width/2-2;

    for(int zz = z; zz < z + 4; zz++) {
        for (int yy = y; yy < y + 4; yy++) {
            for (int xx = x; xx < x + 4; xx++) {
                int index = grid_width * (grid_height * (zz) + (yy)) + (xx);
                density_source[index] = 1.0f;
                temperature_source[index] = 1500.0f;
                velocity_source[index] = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    createScalar3DTexture(&densityData,   grid_width, grid_height, grid_depth, density_field);
    createScalar3DTexture(&densitySource, grid_width, grid_height, grid_depth, density_source);
    createScalar3DTexture(&densityResult, grid_width, grid_height, grid_depth, (float*)nullptr);

    createScalar3DTexture(&temperatureData,   grid_width, grid_height, grid_depth, temperature_field);
    createScalar3DTexture(&temperatureSource, grid_width, grid_height, grid_depth, temperature_source);
    createScalar3DTexture(&temperatureResult, grid_width, grid_height, grid_depth, (float*)nullptr);

    createVector3DTexture(&velocityData,   grid_width, grid_height, grid_depth, velocity_field);
    createVector3DTexture(&velocitySource, grid_width, grid_height, grid_depth, velocity_source);
    createVector3DTexture(&velocityResult, grid_width, grid_height, grid_depth, (vec3*)nullptr);

    createScalar3DTexture(&divergenceData,   grid_width, grid_height, grid_depth, (float*)nullptr);
    createScalar3DTexture(&divergenceResult, grid_width, grid_height, grid_depth, (float*)nullptr);

    createScalar3DTexture(&gradientData,   grid_width, grid_height, grid_depth, (float*)nullptr);
    createScalar3DTexture(&gradientResult, grid_width, grid_height, grid_depth, (float*)nullptr);

    delete[] density_field, density_source, temperature_field, temperature_source, velocity_field, velocity_source;
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
    diffuseShader.load(  "shaders/simulation/slab.vert", "shaders/simulation/diffuse/diffuse.frag");
    dissipateShader.load("shaders/simulation/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    addSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_source.frag");
    setSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/set_source.frag");
    buoyancyShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/buoyancy.frag");
    // Projection Shaders
    divergenceShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/divergence.frag");
    jacobiShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/jacobi.frag");
    gradientShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/gradient_subtraction.frag");
    // Temperature Shaders
    temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
}

void SlabOperator::getData(GLuint& density, GLuint& temperature, int& width, int& height, int& depth) {
    temperature = temperatureData;
    density = densityData;
    width = grid_width;
    height = grid_height;
    depth = grid_depth;
}

void SlabOperator::swapData(GLuint& d1, GLuint& d2){
    GLuint tmp = d1;
    d1 = d2;
    d2 = tmp;
}

void SlabOperator::setBoundary(GLuint& data, GLuint& result, int scale) {
    // Input data used in all steps
    bind3DTexture0(data);

    boundaryShader.use();
    boundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    boundaryShader.uniform1f("scale", scale);

    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);

        drawBoundaryToTexture(boundaryShader, depth);
    }
    // Front
    drawFrontOrBackBoundary(result, scale, 0);

    //Back
    drawFrontOrBackBoundary(result, scale, grid_depth - 1);
}

void SlabOperator::drawFrontOrBackBoundary(GLuint result, int scale, int depth){
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);

    FABBoundaryShader.use();
    FABBoundaryShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    FABBoundaryShader.uniform1f("scale", scale);
    drawBoundaryToTexture(FABBoundaryShader,  depth);

    FABInteriorShader.use();
    drawInteriorToTexture(FABInteriorShader, depth);
}

void SlabOperator::update() {
    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;

    // Setup GPU
    FBO->use();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    float dt = 1.0f/6.0f;

   velocityStep(delta_time);

   densityStep(delta_time);

    FBO->null();
}

void SlabOperator::velocityStep(float dt){
    // Source
    buoyancy(dt);
   // addSource(velocityData, velocitySource, velocityResult, dt);
    // Advect
    advection(velocityData, velocityResult, dt);
    // Project
    //diffuse(velocityData, velocityResult, dt);
    //dissipate(velocityData, velocityResult, dt);
    project();

}

void SlabOperator::densityStep(float dt){
    // addForce
    setSource(densityData, densitySource, densityResult, dt);
    setSource(temperatureData, temperatureSource, temperatureResult, dt);
    dissipate(densityData, densityResult, 0.9f, dt);
    // Advect
    fulladvection(densityData, densityResult, dt);
    temperature(dt);

    // Diffuse
    //diffuse(densityData, densityResult, dt);

}

void SlabOperator::temperature(float dt){
    temperatureShader.use();
    temperatureShader.uniform1f("dt", dt);
    temperatureShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    bind3DTexture0(velocityData);
    bind3DTexture1(temperatureData);

    fullOperation(temperatureShader, temperatureResult);
    swapData(temperatureData, temperatureResult);

    //setBoundary(temperatureData, temperatureResult, 0);
    //swapData(temperatureData, temperatureResult);
}

// todo how (if in any way) should the two source functions apply border restrictions
void SlabOperator::addSource(GLuint& data, GLuint& source, GLuint& result, float dt) {
    addSourceShader.use();
    addSourceShader.uniform1f("dt", dt);
    bind3DTexture0(data);
    bind3DTexture1(source);

    interiorOperation(addSourceShader, result);
    swapData(data, result);

    //setBoundary(data, result, 0);
    //swapData(data, result);
}

void SlabOperator::setSource(GLuint& data, GLuint& source, GLuint& result, float dt) {
    setSourceShader.use();
    setSourceShader.uniform1f("dt", dt);
    bind3DTexture0(data);
    bind3DTexture1(source);

    fullOperation(setSourceShader, result);
    swapData(data, result);

    //setBoundary(data, result, 0);
    //swapData(data, result);
}

void SlabOperator::buoyancy(float dt){
    buoyancyShader.use();
    buoyancyShader.uniform1f("dt", dt);
    bind3DTexture0(temperatureData);
    bind3DTexture1(velocityData);

    interiorOperation(buoyancyShader, velocityResult);
    swapData(velocityData, velocityResult);

    setBoundary(velocityData, velocityResult, 1);
    swapData(velocityData, velocityResult);
}

void SlabOperator::diffuse(GLuint& data, GLuint& result, float dt){
    diffuseShader.use();
    diffuseShader.uniform1f("dt", dt);
    diffuseShader.uniform1f("diffusion_constant", 1.0);

    for(int i = 0; i < 20; i++){
        bind3DTexture0(data);
        bind3DTexture1(result); // todo fix bad data. Should not use a texture as both input and output
        interiorOperation(diffuseShader, result);
        swapData(data, result);

        setBoundary(data, result, 0);
        swapData(data, result);
    }
}

void SlabOperator::dissipate(GLuint& data, GLuint& result, float dissipationRate, float dt){

    dissipateShader.use();
    dissipateShader.uniform1f("dt", dt);
    dissipateShader.uniform1f("dissipation_rate", dissipationRate);
    bind3DTexture0(data);

    interiorOperation(dissipateShader, result);
    swapData(data, result);

    setBoundary(data, result, 0);
    swapData(data, result);
}

void SlabOperator::advection(GLuint& data, GLuint& result, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform3f("gridSize", grid_width, grid_height, grid_depth);
    bind3DTexture0(velocityData);
    bind3DTexture1(data);

    interiorOperation(advectionShader, result);
    swapData(data, result);

    setBoundary(data, result, 0);
    swapData(data, result);
}
void SlabOperator::fulladvection(GLuint& data, GLuint& result, float dt) {
    advectionShader.use();
    glUniform1f(glGetUniformLocation(advectionShader.program(), "dt"), dt);
    glUniform3f(glGetUniformLocation(advectionShader.program(), "gridSize"), grid_width, grid_height, grid_depth);
    bind3DTexture0(velocityData);
    bind3DTexture1(data);

    fullOperation(advectionShader, result);
    swapData(data, result);
}
void SlabOperator::project(){
    divergence();
    jacobi();
    gradient();
}

void SlabOperator::divergence(){
    divergenceShader.use();
    bind3DTexture0(velocityData);

    interiorOperation(divergenceShader, divergenceResult);
    swapData(divergenceData, divergenceResult);

    setBoundary(divergenceData, divergenceResult, 0);
    swapData(divergenceData, divergenceResult);

}

void SlabOperator::jacobi(){

    for(int depth = 0; depth < grid_depth; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gradientData, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    jacobiShader.use();
    bind3DTexture1(divergenceData);

    for(int i = 0; i < 20; i++){
        bind3DTexture0(gradientData);
        interiorOperation(jacobiShader, gradientResult);
        swapData(gradientData, gradientResult);

        setBoundary(gradientData, gradientResult, 0);
        swapData(gradientData, gradientResult);

    }
}

void SlabOperator::gradient(){
    gradientShader.use();
    bind3DTexture0(gradientData);
    bind3DTexture1(velocityData);

    interiorOperation(gradientShader, velocityResult);
  //  setBoundary(velocityData, velocityResult, 0);
    swapData(velocityData, velocityResult);
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


void SlabOperator::interiorOperation(Shader shader, GLuint result) {
    for(int depth = 1; depth < grid_depth - 1; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        drawInteriorToTexture(shader, depth);
    }
}

void SlabOperator::fullOperation(Shader shader, GLuint result) {
    for(int depth = 0; depth < grid_depth; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result, 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);

        // Interior
        drawAllToTexture(shader, depth);
    }
}

void SlabOperator::bind3DTexture0(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);
}

void SlabOperator::bind3DTexture1(GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, texture);
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