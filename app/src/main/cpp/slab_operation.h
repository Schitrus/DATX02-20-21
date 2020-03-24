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

    GLuint dataMatrix, velocityMatrix, densityMatrix, pressureMatrix, temperatureMatrix, tempSourceMatrix, velSourceMatrix, divMatrix, sourcePMatrix;
    // Result textures. They are only used temporarily during simulation steps to store the result, and only exist here for reuse-ability
    GLuint scalarResultMatrix, vectorResultMatrix;

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
    void advection(GLuint &data, bool isVectorField, float dh, float dt);

    void divergence();
    void jacobi();
    void proj();

    void addition(GLuint &target, GLuint &source, bool isVectorField, float dt);

    void dissipate(GLuint &target, float dissipationRate, float dt);

    void velocityStep(float dh, float dt);

    // Performs the usual steps for moving substances using the fluid velocity field
    // It will not perform the "add force" step, as that depends entirely on the individual substance
    void substanceMovementStep(GLuint &target, float dissipationRate, float dh, float dt);

    // Performs the operation with the set shader over the entirety of the given data.
    // You must set the shader program, along with any uniform input or textures needed by the shader beforehand.
    void performOperation(Shader shader, GLuint &target, bool isVectorField, int boundaryScale);

    // Binds the given 3d texture to slot 0
    // Note that the active texture is left at slot 0 after this!
    void bind3DTexture0(GLuint texture);

    // Binds the given 3d texture to slot 1
    // Note that the active texture is left at slot 1 after this!
    void bind3DTexture1(GLuint texture);

    void setBoundary(GLuint data, GLuint result, int scale);

    void setFrontOrBackBoundary(GLuint data, GLuint result, int scale, int depth);

    void prepareResult(GLuint result, int depth);

    // Sets the depth uniform on the shader and then draws the interior
    // Should be called after the relevant call to prepareResult()
    void drawInteriorToTexture(Shader shader, int depth);

    // Sets the depth uniform on the shader and then draws the boundary
    // Should be called after the relevant call to prepareResult()
    void drawBoundaryToTexture(Shader shader, int depth);
};

#endif //DATX02_20_21_SLAB_OPERATION_H
