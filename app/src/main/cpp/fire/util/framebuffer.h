//
// Created by Kalle on 2020-03-10.
//

#ifndef DATX02_20_21_FRAMEBUFFER_H
#define DATX02_20_21_FRAMEBUFFER_H

#include <jni.h>
#include <GLES3/gl31.h>

#include "simple_framebuffer.h"

class Framebuffer {
    int width, height;
    GLuint format,inFormat;

    SimpleFramebuffer FBO;
    GLuint RBO;
    GLuint colorTextureTarget;
public:
    void create(int width, int height);
    void create(int width, int height, GLuint outFormat, GLuint inFormat);

    void clear();

    void resize(int width, int height);

    GLuint texture();

    bool bind(const char *tag);

    void unbind();
private:
};


#endif //DATX02_20_21_FRAMEBUFFER_H
