//
// Created by Anton Forsberg on 18/02/2020.
//
#include <jni.h>

#include <time.h>
#include <math.h>
#include <chrono>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "helper.h"
#include "Shader.h"

using namespace glm;

void init();

void initCube();

void initProgram();

void resize(int width, int height);

void step();

void loadMVP(GLuint shaderProgram);

void display();

extern "C" {

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_RayRenderer_init(JNIEnv *env, jobject) {
    init();
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_RayRenderer_resize(JNIEnv *env, jobject, jint width, jint height) {
    resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_RayRenderer_step(JNIEnv *env, jobject /* this */) {
    step();
}

}
// fbo
GLuint framebufferId, colorTextureTarget, renderBuffer;

// shaders
GLuint frontFaceShaderProgram, backFaceShaderProgram;

// cube
GLuint vertexArrayObject;
GLuint positionBuffer, indexBuffer;

int w, h = 0;
vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

void init() {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    initCube();
    initProgram();
}

void resize(int width, int height) {

    glViewport(0, 0, width, height);

    w = width;
    h = height;

    if (framebufferId != 0) {
        resizeFBO(width, height, &colorTextureTarget, &renderBuffer);
    } else {
        createFbo(width, height, &framebufferId, &colorTextureTarget, &renderBuffer);
    }

}

void initCube() {

    glGenVertexArrays(1, &vertexArrayObject);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(vertexArrayObject);

    ///////////////////////////////////
    // Define the positions
    const GLfloat positions[] = {
            0.0f, 0.0f, 0.0f,    //v0
            1.0f, 0.0f, 0.0f,    //v1
            1.0f, 1.0f, 0.0f,    //v2
            0.0f, 1.0f, 0.0f,    //v3
            0.0f, 0.0f, 1.0f,    //v4
            1.0f, 0.0f, 1.0f,    //v5
            1.0f, 1.0f, 1.0f,    //v6
            0.0f, 1.0f, 1.0f,    //v7
    };

    // Create a handle for the position vertex buffer object
    glGenBuffers(1, &positionBuffer);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    // Send the vertex position data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // Attaches positionBuffer to vertexArrayObject, in the 0th attribute location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex position attribute
    glEnableVertexAttribArray(0);

    ///////////////////////////////////

    const GLuint indices[] = {0, 1, 2, 0, 2, 3,    //front
                              4, 7, 6, 4, 6, 5,    //back
                              4, 0, 3, 4, 3, 7,    //left
                              1, 5, 6, 1, 6, 2,    //right
                              3, 2, 6, 3, 6, 7,    //top
                              4, 5, 1, 4, 1, 0,    //bottom
    };

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void initProgram() {

    backFaceShaderProgram = createProgram(VERTEX_SHADER, BACK_FACE_FRAGMENT_SHADER);
    frontFaceShaderProgram = createProgram(VERTEX_SHADER, FRONT_FACE_FRAGMENT_SHADER);
}

void step() {

    display();

}


void display() {

    glBindVertexArray(vertexArrayObject);

    // back
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glUseProgram(backFaceShaderProgram);
    loadMVP(backFaceShaderProgram);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // front
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    glUseProgram(frontFaceShaderProgram);
    loadMVP(frontFaceShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

#define PI 3.14159265359f

void loadMVP(GLuint shaderProgram) {

    mat4 modelMatrix = mat4(1.0);
    mat4 viewMatrix = mat4(1.0);
    mat4 projectionMatrix = mat4(1.0);
    mat4 MVP = mat4(1.0);

    // Set up a projection matrix
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    float fovy = radians(60.0f);
    float aspectRatio = w * 1.0f / h;

    projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);

    viewMatrix = lookAt(vec3(0), vec3(0, 0, -1), worldUp);

    modelMatrix = translate(modelMatrix, vec3(0, 0, -4));
    modelMatrix = rotate(modelMatrix, 0.8f * PI, vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, 0.1f * PI, vec3(1, 0, 0));
    modelMatrix = translate(modelMatrix, vec3(-0.5, -0.5, -0.5));

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
#pragma clang diagnostic pop

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, &mvp[0].x);

}
