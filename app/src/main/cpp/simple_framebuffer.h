//
// Created by kirderf1 on 2020-04-03.
//

#ifndef DATX02_20_21_SIMPLE_FRAMEBUFFER_H
#define DATX02_20_21_SIMPLE_FRAMEBUFFER_H

#include <gles3/gl31.h>

class SimpleFramebuffer {
    GLuint FBO;
public:

    void init();

    void clear();

    void bind();

    void unbind();
};

#endif //DATX02_20_21_SIMPLE_FRAMEBUFFER_H