//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <chrono>

#include "shader.h"
#include "framebuffer.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class SlabOperator{
    int grid_width, grid_height, grid_depth;

    // Framebuffer
    Framebuffer* FBO;

    // result // todo remove
    GLuint texcoordsBuffer;

    // interior
    GLuint interiorVAO;
    GLuint interiorPositionBuffer;
    GLuint interiorIndexBuffer;

    // boundary
    GLuint boundaryVAO;
    GLuint boundaryPositionBuffer;

    // front and back face
    Shader FABInteriorShader;
    Shader FABBoundaryShader;
    Shader boundaryShader;

    GLuint densityData, temperatureData, velocityData, divergenceData;
    GLuint densitySource, temperatureSource, velocitySource;
    GLuint densityResult, temperatureResult, velocityResult, divergenceResult;
    GLuint gradientData, gradientResult;

    Shader temperatureShader;
    Shader divergenceShader, jacobiShader, gradientShader;
    Shader additionShader, buoyancyShader, advectionShader;
    Shader diffuseShader, dissipateShader, constShader;

    // Time
    time_point<system_clock> start_time, last_time;

public:
    void init();

    void resize(int width, int height, int depth);

    void update();

    void getData(GLuint& pressure, GLuint& temperature, int& width, int& height, int& depth);

    void swapData(GLuint& d1, GLuint& d2);
private:
    void initData();

    void initLine();
    void initQuad();
    void initShaders();

    void buoyancy(float dt);
    void advection(GLuint& data, GLuint& result, float dt);
    void fulladvection(GLuint& data, GLuint& result, float dt);

    void divergence();
    void jacobi();
    void gradient();

    void temperature(float dt);

    void addition(GLuint& data, GLuint& result, GLuint& source, float dt);
    void constadd(GLuint& data, GLuint& result, GLuint& source, float dt);

    void dissipate(GLuint& data, GLuint& result, float dt);

    void diffuse(GLuint& data, GLuint& result, float dt);

    void project();

    void setBoundary(GLuint& data, GLuint& result, int scale);

    void velocityStep(float dt);

    void densityStep(float dt);

};

#endif //DATX02_20_21_SLAB_OPERATION_H
