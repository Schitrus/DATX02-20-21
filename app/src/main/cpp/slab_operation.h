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
    //Textures for sources
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

    // Applies buoyancy forces to velocity, based on the temperature
    void buoyancy(float dt);
    // Performs advection on the given data
    void advection(GLuint& data, GLuint& result, float dt);
    void fulladvection(GLuint& data, GLuint& result, float dt);

    // Projects the given *vector* field texture
    void projection(GLuint &target);

    // Calculates the divergence of the vector field "target" and stores it in result
    void divergence(GLuint target, GLuint &result);

    // Performs a number of jacobi iterations of the scalar "divergence" into jacobi
    // Beware that the it will use the jacobi field passed as the starting value for the iterations
    void jacobi(GLuint divergence, GLuint &jacobi);

    // Subtracts the gradient of the given scalar field from the target vector field
    void subtractGradient(GLuint &target, GLuint scalarField);

    void temperature(float dt);

    // Adds the given source field multiplied by dt to the target field
    void addition(GLuint& data, GLuint& result, GLuint& source, float dt);
    void constadd(GLuint& data, GLuint& result, GLuint& source, float dt);

    void dissipate(GLuint& data, GLuint& result, float dissipationRate, float dt);

    void diffuse(GLuint& data, GLuint& result, float dt);

    void project();

    void setBoundary(GLuint& data, GLuint& result, int scale);

    // Performs one simulation step for velocity
    void velocityStep(float dh, float dt);

    void temperatureStep(float dh, float dt);

    void densityStep(float dt);

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

    void setFrontOrBackBoundary(GLuint data, GLuint result, int scale, int depth);

    // Prepares the given texture and layer to be rendered to
    // Intended to be called before drawInteriorToTexture() or drawBoundaryToTexture()
    void prepareResult(GLuint result, int depth);

    // Sets the depth uniform on the shader and then draws the interior
    // Should be called after the relevant call to prepareResult()
    void drawInteriorToTexture(Shader shader, int depth);

    // Sets the depth uniform on the shader and then draws the boundary
    // Should be called after the relevant call to prepareResult()
    void drawBoundaryToTexture(Shader shader, int depth);
};

#endif //DATX02_20_21_SLAB_OPERATION_H
