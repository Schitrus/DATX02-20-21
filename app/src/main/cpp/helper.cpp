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

#include "file_loader.h"

#define LOG_TAG "helper"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


bool checkGlError(const char *funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

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
        ALOGE("Failed to load image: ");
    }

    return fileContent;
}

void create3DTexture(GLuint *id, int width, int height, int depth, float *data){

    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_3D, *id);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED,
                 GL_FLOAT, data); //GL_FLOAT

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
