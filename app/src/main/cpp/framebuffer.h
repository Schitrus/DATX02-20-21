//
// Created by Kalle on 2020-03-10.
//

#ifndef DATX02_20_21_FRAMEBUFFER_H
#define DATX02_20_21_FRAMEBUFFER_H

#include <jni.h>
#include <GLES3/gl32.h>

class Framebuffer {
    int width, height;
    GLuint FBO, RBO;
    GLuint colorTextureTarget;
    bool simple;
public:
    void create(int width, int height, bool simple=false);

    void resize(int width, int height);

    GLuint texture();

    void bind();

    void unbind();
private:
};


#endif //DATX02_20_21_FRAMEBUFFER_H
