//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_RAY_RENDERER_H
#define DATX02_20_21_RAY_RENDERER_H

#include <jni.h>
#include <GLES3/gl32.h>
#include <glm/glm.hpp>
#include <chrono>

#include <android/asset_manager.h>

#include "shader.h"
#include "framebuffer.h"

using namespace glm;

using std::chrono::time_point;
using std::chrono::system_clock;

class RayRenderer{
    int window_width, window_height;
    int texture_width, texture_height, texture_depth;
    AAssetManager* assetManager;

    // Framebuffers
    Framebuffer* FBO;

    // Cube Buffers
    GLuint VAO;      // Vertex Array Object
    GLuint VBO, EBO; // Vertex Buffer Object && Element Buffer Object
    vec3 boundingScale;

    // 3D texture
    GLuint volumeTexID;

    // Shaders
    Shader frontFaceShader, backFaceShader;

    // Time
    time_point<system_clock> start_time, last_time;

    vec3 worldUp = {0.0f, 1.0f, 0.0f};
public:
    void init(AAssetManager* assetManager);

    void resize(int width, int height);

    void step();

    void setData(GLuint data, int width, int height, int depth);
    void getData(GLuint& data, int& width, int& height, int& depth);

private:

    void load3DTexture(const char *fileName);

    void initCube(GLuint &VAO, GLuint &VBO, GLuint &EBO);

    void initProgram();

    void loadMVP(Shader shader, float current_time);

};

#endif //DATX02_20_21_RAY_RENDERER_H
