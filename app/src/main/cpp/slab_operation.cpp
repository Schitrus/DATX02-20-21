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
#include "Shader.h"

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace slab;

using namespace glm;

#define PI 3.14159265359f

int screen_width;
int screen_height;

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_SlabRenderer_init(JNIEnv *env, jobject, jobject mgr) {
    init(env, mgr);
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_SlabRenderer_resize(JNIEnv *env, jobject, jint width, jint height) {
    screen_width = width;
    screen_height = height;
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_SlabRenderer_step(JNIEnv *env, jobject) {
    step();
}

}

// fbo
GLuint slabFBO = UINT32_MAX;
GLuint resultTarget;

// result // todo remove
GLuint resultShaderProgram;
GLuint texcoordsBuffer;

// matrices
GLuint dataMatrix;
GLuint ResultMatrix;

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

int grid_width, grid_height, grid_depth;

void slab::init(JNIEnv *env, jobject assetManager) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    grid_width = 512;
    grid_height = 512;
    grid_depth = 512;
    createMatrixFBO(grid_width, grid_height, &slabFBO, &resultTarget);
    initData();

    initQuad();
    initLine();
    initProgram();
}


void slab::initData() {

    int size = grid_width * grid_height * grid_depth;
    float* data = new float[size];
    int b = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < size; ++i) { // todo fix crash on large arrays
        data[i] = 0.16666f;
    }

    create3DTexture(&dataMatrix, grid_width, grid_height, grid_depth, data);

    create3DTexture(&ResultMatrix, grid_width, grid_height, grid_depth, NULL);

    delete[] data;
}

void slab::initLine() {
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

void slab::initQuad() {

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

void slab::initProgram() {

    interiorShaderProgram = createProgram(SLAB_VERTEX_SHADER, INTERIOR_FRAGMENT_SHADER);
    boundaryShaderProgram = createProgram(SLAB_VERTEX_SHADER, BOUNDARY_FRAGMENT_SHADER);

    frontAndBackInteriorShaderProgram = createProgram(SLAB_VERTEX_SHADER,
                                                      FRONT_AND_BACK_INTERIOR_FRAGMENT_SHADER);
    frontAndBackBoundaryShaderProgram = createProgram(SLAB_VERTEX_SHADER,
                                                      FRONT_AND_BACK_BOUNDARY_FRAGMENT_SHADER);

    resultShaderProgram = createProgram(RESULTS_VERTEX_SHADER,
                                        RESULTS_FRAGMENT_SHADER); // todo remove

}

void slab::display_results();

void slab::step() {
    //slabOperation();
    //display_results(); // todo remove
}


void slab::slabOperation() {

    glBindFramebuffer(GL_FRAMEBUFFER, slabFBO);

    for (int current_depth = 1; current_depth < grid_depth - 1; ++current_depth) {

        slabOperation(interiorShaderProgram, boundaryShaderProgram, current_depth,
                      1.0f); // todo fix so they are done at the same time

    }

    slabOperation(frontAndBackInteriorShaderProgram, frontAndBackBoundaryShaderProgram, 0, 1.0f);

    slabOperation(frontAndBackInteriorShaderProgram, frontAndBackBoundaryShaderProgram,
                  grid_depth - 1, 1.0f);

}

void slab::slabOperation(GLuint interiorProgram, GLuint boundariesProgram, int layer, float scale) {

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ResultMatrix, 0, layer);

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
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);


    //interior
    glViewport(1, 1, grid_width - 2, grid_height - 2);
    glBindVertexArray(interiorVAO);
    glUseProgram(interiorProgram);
    glUniform1i(glGetUniformLocation(interiorProgram, "depth"), layer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dataMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void slab::display_results() {
    // display result
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, screen_width, screen_height);
    glUseProgram(resultShaderProgram);
    glBindVertexArray(interiorVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, ResultMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

