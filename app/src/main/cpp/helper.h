//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_HELPER_H
#define DATX02_20_21_HELPER_H

#include <gles3/gl31.h>
#include <android/asset_manager.h>

#include <glm/glm.hpp>

using namespace glm;

bool checkGlError(const char *funcName);

void createScalar3DTexture(GLuint *id, int width, int height, int depth, float* data);
void createVector3DTexture(GLuint *id, int width, int height, int depth, vec3* data);

void load3DTexture(AAssetManager *mgr, const char *filename, GLsizei width, GLsizei height,
                   GLsizei depth,GLuint *volumeTexID);

#endif //DATX02_20_21_HELPER_H
