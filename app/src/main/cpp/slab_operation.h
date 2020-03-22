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
    Shader resultShader;
    GLuint texcoordsBuffer;

    // matrices
    GLuint dataMatrix;
    GLuint ResultMatrix;

    // interior
    Shader interiorShader;
    GLuint interiorVAO;
    GLuint interiorPositionBuffer;
    GLuint interiorIndexBuffer;

    // boundary
    Shader boundaryShader;
    GLuint boundaryVAO;
    GLuint boundaryPositionBuffer;

    // front and back face
    Shader FABInteriorShader;
    Shader FABBoundaryShader;
public:
    void init();

    void resize(int width, int height, int depth);

    void update();

    void setData(GLuint data, int width, int height, int depth);
    void getData(GLuint& data, int& width, int& height, int& depth);

    void swapData();
private:
    void initData();
    void initVelocity(int size);

    void initPressure(float* data);

    void initDensity(float* data);

    void initTemperature(float* data);

    void initSources();

    void initLine();

    void initQuad();

    void initProgram();

    void slabOperation();
    void slabOperation(Shader interiorProgram, Shader boundariesProgram, int layer, float scale);

    void display_results();
};

#endif //DATX02_20_21_SLAB_OPERATION_H
