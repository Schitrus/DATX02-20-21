//
// Created by Anton Forsberg on 18/02/2020.
//

#include "helper.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include <stdlib.h>
#include <stdio.h>

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "file_loader.h"

#define LOG_TAG "helper"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Debugging variable to enable or disable OpenGL error and status checking
// Apparently the checks are very costly for performance, disabling should increase FPS
// Enable/disable by commenting/uncommenting the line below
//#define ENABLE_OPENGL_ERROR_CHECKING


using namespace glm;

char *loadFileToMemory(AAssetManager *mgr, const char *filename) {

    // Open your file
    AAsset *file = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    // Get the file length
    off_t fileLength = AAsset_getLength(file);

    // Allocate memory to read your file
    char *fileContent = new char[fileLength];

    // Read your file
    float error = AAsset_read(file, fileContent, fileLength);

    if (error < fileLength || error == 0) {
        std::cout << "Failed to load image: " << filename << ".\n";
        LOG_ERROR("Failed to load image: ");
    }

    return fileContent;
}

void createScalar3DTexture(GLuint& id, ivec3 size, float* data){

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, size.x, size.y, size.z, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void createVector3DTexture(GLuint& id, ivec3 size, vec3* data){

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);  // todo RGB16F is not considered color-renderable in the gles 3.2 specification. Consider switching to RGBA16F
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, size.x, size.y, size.z, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void load3DTexture(AAssetManager *mgr, const char *filename, GLsizei width, GLsizei height,
                   GLsizei depth,GLuint *volumeTexID) {
   const char *fileContent = loadFileToMemory(mgr, filename);

    if (*volumeTexID == UINT32_MAX) {
        glGenTextures(1, volumeTexID);
    }

    glBindTexture(GL_TEXTURE_3D, *volumeTexID);

    glTexImage3D(GL_TEXTURE_3D,
                 0,
                 GL_R8,
                 width,
                 height,
                 depth,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 fileContent);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glGenerateMipmap( GL_TEXTURE_3D );

    // Free the memoery you allocated earlier
    delete[] fileContent;
}

void bindData(GLuint dataTexture, GLenum textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, dataTexture);
}

void clearGLErrors(const char* tag) {
#ifndef ENABLE_OPENGL_ERROR_CHECKING
    return;
#endif

    int count = 0;
    GLenum error = glGetError();
    while(error != GL_NO_ERROR)
    {
        count++;
        error = glGetError();
    }
    if(count != 0)
        LOG_INFO("Cleared out %d unchecked gl errors in preparation for %s\n", count, tag);
}

bool checkGLError(const char* function) {
#ifndef ENABLE_OPENGL_ERROR_CHECKING
    return true;
#endif

    GLenum error = glGetError();
    if(error == GL_NO_ERROR)
        return true;
    else if(error == GL_INVALID_ENUM)
        LOG_ERROR("GL error during %s(): Used an invalid gl enum\n", function);
    else if(error == GL_INVALID_VALUE)
        LOG_ERROR("GL error during %s(): Used an invalid value for a gl operation\n", function);
    else if(error == GL_INVALID_OPERATION)
        LOG_ERROR("GL error during %s(): Tried to use a gl operation at an invalid time\n", function);
    else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
        LOG_ERROR("GL error during %s(): Tried to use an incomplete framebuffer\n", function);
    else if(error == GL_OUT_OF_MEMORY)
        LOG_ERROR("GL error during %s(): GL ran out of memory. This is not good!\n", function);
    else LOG_ERROR("Unknown GL error during %s(): 0x%08x\n", function, error);
    return false;
}

bool checkFramebufferStatus(GLenum target, const char* tag) {
#ifndef ENABLE_OPENGL_ERROR_CHECKING
    return true;
#endif

    GLenum status = glCheckFramebufferStatus(target);
    if(status == 0)
        LOG_ERROR("%s used an invalid framebuffer type: %d", tag, target);
    else if(status == GL_FRAMEBUFFER_COMPLETE)
        return true;
    else if(status == GL_FRAMEBUFFER_UNDEFINED)
        LOG_ERROR("%s is using the default framebuffer, but the default framebuffer does not exist", tag);
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        LOG_ERROR("Framebuffer used by %s has one or more incomplete attachments", tag);
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        LOG_ERROR("Framebuffer used by %s is missing image attachments", tag);
    else LOG_ERROR("Framebuffer used by %s has incomplete status %d", tag, status);
    return false;
}
