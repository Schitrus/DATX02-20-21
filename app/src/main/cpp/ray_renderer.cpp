//
// Created by Anton Forsberg on 18/02/2020.
//
#include "ray_renderer.h"

#include <jni.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <string>

#include <gles3/gl31.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "helper.h"

#define LOG_TAG "Renderer"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define NOW std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::now())
#define DURATION(a, b) (std::chrono::duration_cast<std::chrono::milliseconds>(a - b)).count() / 1000.0f;

using namespace glm;

#define PI 3.14159265359f

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

int RayRenderer::init(AAssetManager* assetManager) {

    start_time = NOW;
    last_time = start_time;

    this->assetManager = assetManager;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    FBO = nullptr;
    densityTexID = UINT32_MAX;
    temperatureTexID = UINT32_MAX;

    //load3DTexture("BostonTeapot.raw");
    initCube(VAO, VBO, EBO);
    if(!initProgram()){
        LOG_ERROR("Failed to compile ray_renderer shaders");
        return 0;
    }
    return 1;
}

void RayRenderer::resize(int width, int height) {

    window_width = width;
    window_height = height;

    glViewport(0, 0, window_width, window_height);

    if (FBO != nullptr) {
        FBO->resize(window_width, window_height);
    } else {
        FBO = new Framebuffer();
        FBO->create(window_width, window_height);
    }

}

void RayRenderer::setData(GLuint density, GLuint temperature, int width, int height, int depth){
    densityTexID = density;
    temperatureTexID = temperature;
    texture_width = width;
    texture_height = height;
    texture_depth = depth;
    float m = max(max(texture_width, texture_height), texture_depth);
    vec3 tex = vec3(texture_width, texture_height, texture_depth)/m;
    boundingScale = tex;
}

void RayRenderer::load3DTexture(const char *fileName) {
    ::load3DTexture(assetManager, fileName, 256, 256, 178, &densityTexID);
    boundingScale = vec3(1, 1, 0.7);
}

void RayRenderer::initCube(GLuint &VAO, GLuint &VBO, GLuint &EBO) {

    glGenVertexArrays(1, &VAO);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(VAO);

    ///////////////////////////////////
    // Define the positions
    constexpr GLfloat positions[] = {
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
    glGenBuffers(1, &VBO);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Send the vertex position data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // Attaches positionBuffer to vertexArrayObject, in the 0th attribute location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex position attribute
    glEnableVertexAttribArray(0);

    ///////////////////////////////////

    constexpr GLuint indices[] = {
                              0, 1, 2, 0, 2, 3,    //front
                              4, 7, 6, 4, 6, 5,    //back
                              4, 0, 3, 4, 3, 7,    //left
                              1, 5, 6, 1, 6, 2,    //right
                              3, 2, 6, 3, 6, 7,    //top
                              4, 5, 1, 4, 1, 0,    //bottom
    };

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Unbind VAO
    glBindVertexArray(0);
}

int RayRenderer::initProgram() {
    bool success = true;
    success &= backFaceShader.load("shaders/render/ray.vert", "shaders/render/back_face.frag");
    success &= frontFaceShader.load("shaders/render/ray.vert", "shaders/render/front_face.frag");
    return success;
}

void RayRenderer::step() {

    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, window_width, window_height);

    glBindVertexArray(VAO);

    // back
    FBO->bind();
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    backFaceShader.use();
    loadMVP(backFaceShader, current_time);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // front
    FBO->unbind();
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    frontFaceShader.use();
    loadMVP(frontFaceShader, current_time);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, FBO->texture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, densityTexID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, temperatureTexID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void RayRenderer::loadMVP(Shader shader, float current_time) {

    float p = current_time / 10.0f;
    float rot = 2 * PI * p;

    // Set up a projection matrix
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    float fovy = radians(60.0f);
    float aspectRatio = (float)window_width / window_height;

    vec3 modelPos(0, 0, -1.0);

    mat4 modelMatrix = translate(mat4(1.0f), modelPos)
                     //* rotate(mat4(1.0f), rot, vec3(0,1,0))
                     * scale(mat4(1.0f), boundingScale)
                     * translate(mat4(1.0f), vec3(-0.5f, -0.5f, -0.5f));

    mat4 viewMatrix = lookAt(vec3(0), modelPos, worldUp);
    mat4 projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);

    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shader.program(), "mvp"), 1, GL_FALSE, &mvp[0].x);

}

#pragma clang diagnostic pop

