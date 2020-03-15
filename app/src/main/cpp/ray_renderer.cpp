//
// Created by Anton Forsberg on 18/02/2020.
//
#include "ray_renderer.h"

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
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "helper.h"

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace glm;

#define PI 3.14159265359f

// fbo
GLuint framebufferId = UINT32_MAX;
GLuint colorTextureTarget, renderBuffer ;

// shaders
GLuint frontFaceShaderProgram, backFaceShaderProgram;

// 3D texture
GLuint volumeTexID = UINT32_MAX;
AAssetManager *mgr;

// cube
GLuint vertexArrayObject;
GLuint positionBuffer, indexBuffer;
float scaleX, scaleY, scaleZ;

int w, h = 0;
vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

void RayRenderer::init(AAssetManager* assetManager) {

    mgr = assetManager;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    framebufferId = UINT32_MAX;

    load3DTexture("BostonTeapot.raw");
    initCube();
    initProgram();
}

void RayRenderer::resize(int width, int height) {

    glViewport(0, 0, width, height);

    w = width;
    h = height;

    if (framebufferId != UINT32_MAX) {
        resizeFBO(width, height, &colorTextureTarget, &renderBuffer);
    } else {
        createFbo(width, height, &framebufferId, &colorTextureTarget, &renderBuffer);
    }

}

void scaleBoundingBox(float x, float y, float z) {
    scaleX = x;
    scaleY = y;
    scaleZ = z;
}

void RayRenderer::load3DTexture(const char *fileName) {
    ::load3DTexture(mgr, fileName, 256, 256, 178, &volumeTexID);
    scaleBoundingBox(1, 1, 0.7);
}

void RayRenderer::initCube() {

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

void RayRenderer::initProgram() {

    backFaceShaderProgram = createProgram("shaders/ray.vert", "shaders/back_face.frag");
    frontFaceShaderProgram = createProgram("shaders/ray.vert", "shaders/front_face.frag");
}

void RayRenderer::step() {
    display();
}

void RayRenderer::display() {

    glViewport(0, 0, w, h);

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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    // temp solution to fix flickering
    float tmpScaleX = scaleX;
    float tmpScaleY = scaleY;
    float tmpScaleZ = scaleZ;
    scaleX = scaleX * 0.99f;
    scaleY = scaleY * 0.99f;
    scaleZ = scaleZ * 0.99f;

    glUseProgram(frontFaceShaderProgram);
    loadMVP(frontFaceShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, volumeTexID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    scaleX = tmpScaleX;
    scaleY = tmpScaleY;
    scaleZ = tmpScaleZ;

}

float start_sec = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()/1000.0;
float dt;
float rotTime = 10;
float rot = 0;


void RayRenderer::loadMVP(GLuint shaderProgram) {

    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
    float currentTime = ms.count()/1000.0 - start_sec;
    float p = currentTime / 10.0f;
    p = p - ((int)p);
    float rot = 2 * PI * p;


    mat4 modelMatrix(1.0);
    mat4 viewMatrix(1.0);
    mat4 projectionMatrix(1.0);
    mat4 MVP(1.0);

    // Set up a projection matrix
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    float fovy = radians(60.0f);
    float aspectRatio = w * 1.0f / h;

    projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);

    viewMatrix = lookAt(vec3(0), vec3(0, 0, -1), worldUp);

    modelMatrix = translate(modelMatrix, vec3(0, 0, -2.5));
   // modelMatrix = rotate(modelMatrix, -0.8f * PI, vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, rot, vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, PI, vec3(1, 0, 0));
    modelMatrix = scale(modelMatrix, vec3(scaleX, scaleY, scaleZ));
    modelMatrix = translate(modelMatrix, vec3(-0.5, -0.5, -0.5));

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
#pragma clang diagnostic pop

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, &mvp[0].x);

}

