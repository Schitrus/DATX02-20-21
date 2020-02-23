//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_HELPER_H
#define DATX02_20_21_HELPER_H

#include <GLES3/gl31.h>
#include <android/asset_manager.h>

bool checkGlError(const char *funcName);

GLuint createShader(GLenum shaderType, const char *src);

GLuint createProgram(const char *vtxSrc, const char *fragSrc);

void createMatrixFBO(int width, int height, GLuint *framebufferId, GLuint *colorTextureTarget);

#endif //DATX02_20_21_HELPER_H
