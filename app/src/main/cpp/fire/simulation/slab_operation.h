//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <GLES3/gl31.h>

#include "fire/util/shader.h"
#include "fire/util/simple_framebuffer.h"
#include "fire/util/data_texture_pair.h"

class SlabOperation {

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
    Shader copyShader;

public:
    int init();

    // Called at the beginning of a series of operations to prepare opengl
    void prepare();

    // Called at the end of a series of operations to unbind the framebuffer
    void finish();

    // Performs the operation with the set shader over the entirety of the given data.
    // You must set the shader program, along with any uniform input or textures needed by the shader beforehand.
    void fullOperation(Shader shader, DataTexturePair* data);

    // Performs the operation with the set shader over the interior of the given data.
    // You must set the shader program, along with any uniform input or textures needed by the shader beforehand.
    void interiorOperation(Shader shader, DataTexturePair* data, int boundaryScale);

    // Target texture is assumed to be of the same size as source
    void copy(DataTexturePair* source, GLuint target);

private:
    void initLine();
    void initQuad();
    int initShaders();

    void setBoundary(DataTexturePair* data, int scale);

    bool drawFrontOrBackBoundary(DataTexturePair* data, int scale, int depth);

    // Sets the depth uniform on the shader and then draws both the interior and boundary
    // Returns true if the operation succeeded without an error
    bool drawAllToTexture(Shader shader, int depth, ivec3 size);

    // Sets the depth uniform on the shader and then draws the interior
    // Returns true if the operation succeeded without an error
    bool drawInteriorToTexture(Shader shader, int depth, ivec3 size);

    // Sets the depth uniform on the shader and then draws the boundary
    // Returns true if the operation succeeded without an error
    bool drawBoundaryToTexture(Shader shader, int depth, ivec3 size);
};

#endif //DATX02_20_21_SLAB_OPERATION_H
