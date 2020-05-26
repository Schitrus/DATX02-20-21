//
// Created by kirderf1 on 2020-04-03.
//

#include "simple_framebuffer.h"

void SimpleFramebuffer::init() {
    glGenFramebuffers(1, &FBO);
}

void SimpleFramebuffer::clear() {
    glDeleteFramebuffers(1, &FBO);
    FBO = 0;
}

void SimpleFramebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void SimpleFramebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint SimpleFramebuffer::getFBO(){
    return FBO;
};