//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <GLES3/gl31.h>

#include "shader.h"
#include "framebuffer.h"

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

    GLuint dataMatrix, velocityMatrix, densityMatrix, pressureMatrix, temperatureMatrix, tempSourceMatrix, velSourceMatrix;
    GLuint resultMatrix, resultVMatrix, resultDMatrix, resultPMatrix, resultTMatrix, divMatrix, sourcePMatrix;

    Shader temperatureShader;
    Shader buoyancyShader, jacobiShader, projectionShader;
    Shader additionShader, divergenceShader, advectionShader, boundaryShader, dissipateShader;

public:
    void init();

    void resize(int width, int height, int depth);

    void update();

    void setData(GLuint data, int width, int height, int depth);
    void getData(GLuint& data, int& width, int& height, int& depth);

    void swapData(GLuint& d1, GLuint& d2);
private:
    void initData();
    void initVelocity(int size);

    void initPressure();

    void initDensity(float* data);

    void initTemperature(float* data);

    void initSources();

    void initLine();

    void initQuad();

    void initShaders();

    void buoyancy(float dt);
    void advection(GLuint& data, GLuint& result, float dt);

    void divergence();
    void jacobi();
    void proj();

    void addition(GLuint& data, GLuint& result, GLuint& source, float dt);

    void dissipate(float dt);

    void velocityStep(float dt);
    void pressureStep(float dt);

    void setBoundary(GLuint data, GLuint result, int scale);

};

#endif //DATX02_20_21_SLAB_OPERATION_H
