//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_HELPER_H
#define DATX02_20_21_HELPER_H

#include <GLES3/gl31.h>
#include <android/asset_manager.h>

#include <glm/glm.hpp>

using namespace glm;

void createScalar3DTexture(GLuint *id, ivec3 size, float* data);
void createVector3DTexture(GLuint *id, ivec3 size, vec3* data);

void load3DTexture(AAssetManager *mgr, const char *filename, GLsizei width, GLsizei height,
                   GLsizei depth,GLuint *volumeTexID);

// Binds the given data texture to the given slot
// The slot should be GL_TEXTURE0 or any larger number, depending on where you need the texture
// Note that the active texture is left at the given slot after this!
void bindData(GLuint dataTexture, GLenum textureSlot);

// Clears out any gl errors and logs how many that was cleared (if any)
// Should normally be paired with checkGLError
// The tag is a name to attribute potential logging to
void clearGLErrors(const char* tag);

// Checks one gl error and logs a message if there is an error
// Returns true if no error was found, and false otherwise
// The tag is a name to attribute potential logging to
bool checkGLError(const char* function);

// Checks the framebuffer status and logs a message if it is incomplete
// Returns true if the framebuffer is complete
// The tag is a name to attribute potential logging to
bool checkFramebufferStatus(GLenum target, const char* tag);

#endif //DATX02_20_21_HELPER_H
