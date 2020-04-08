//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <gles3/gl31.h>

#include "shader.h"
#include "simple_framebuffer.h"
#include "data_texture_pair.h"

class SlabOperator {
    int grid_width, grid_height, grid_depth;
    //The number of voxels in a meter in the current resolution
    float meter_to_voxels;

    // Framebuffer
    SimpleFramebuffer* FBO;

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

    GLuint diffusionBTexture;
    DataTexturePair* divergence;
    DataTexturePair* jacobi;

    Shader temperatureShader;
    Shader divergenceShader, jacobiShader, gradientShader;
    Shader addSourceShader, buoyancyShader, advectionShader;
    Shader dissipateShader, setSourceShader;
    Shader copyShader;

public:
    int init();

    void initSize(int width, int height, int depth, float meterToVoxels);

    // Called at the beginning of a series of operations to prepare opengl
    void prepare();

    // Called at the end of a series of operations to unbind the framebuffer
    void finish();

    // Applies buoyancy forces to velocity, based on the temperature
    void buoyancy(DataTexturePair* velocity, DataTexturePair* temperature, float dt, float scale);
    // Performs advection on the given data
    void advection(DataTexturePair* velocity, DataTexturePair* data, float dt);

    void fulladvection(DataTexturePair* velocity, DataTexturePair* data, float dt);

    void temperatureOperation(DataTexturePair* temperature, DataTexturePair* velocity, float dt);

    // Adds the given source field multiplied by dt to the target field
    void addSource(DataTexturePair* data, GLuint& source, float dt);
    void setSource(DataTexturePair* data, GLuint& source, float dt);

    void dissipate(DataTexturePair* data, float dissipationRate, float dt);

    //example values: iterationCount = 20, diffusionConstant = 1.0
    void diffuse(DataTexturePair* data, int iterationCount, float kinematicViscosity, float dt);

    // Projects the given *vector* field
    void projection(DataTexturePair* velocity, int iterationCount);

private:
    void initData();

    void initLine();
    void initQuad();
    int initShaders();

    // Performs a number of jacobi iterations with two field inputs
    void jacobiIteration(DataTexturePair *xTexturePair, GLuint bTexture,
                int iterationCount, float alpha, float beta);

    // Calculates the divergence of the vector field
    void createDivergence(DataTexturePair* vectorData, float dx);

    // Subtracts the gradient of the given scalar field from the target vector field
    void subtractGradient(DataTexturePair* velocity, float dx);

    void setBoundary(DataTexturePair* data, int scale);

    // Performs the operation with the set shader over the interior of the given data.
    // You must set the shader program, along with any uniform input or textures needed by the shader beforehand.
    void interiorOperation(Shader shader, DataTexturePair* data);

    // Performs the operation with the set shader over the entirety of the given data.
    // You must set the shader program, along with any uniform input or textures needed by the shader beforehand.
    void fullOperation(Shader shader, DataTexturePair* data);

    void copy(DataTexturePair* source, GLuint target);

    // Binds the given data texture to the given slot
    // The slot should be GL_TEXTURE0 or any larger number, depending on where you need the texture
    // Note that the active texture is left at the given slot after this!
    void bindData(GLuint dataTexture, GLenum textureSlot);

    bool drawFrontOrBackBoundary(DataTexturePair* data, int scale, int depth);

    // Sets the depth uniform on the shader and then draws both the interior and boundary
    // Returns true if the operation succeeded without an error
    bool drawAllToTexture(Shader shader, int depth);

    // Sets the depth uniform on the shader and then draws the interior
    // Returns true if the operation succeeded without an error
    bool drawInteriorToTexture(Shader shader, int depth);

    // Sets the depth uniform on the shader and then draws the boundary
    // Returns true if the operation succeeded without an error
    bool drawBoundaryToTexture(Shader shader, int depth);
};

#endif //DATX02_20_21_SLAB_OPERATION_H
