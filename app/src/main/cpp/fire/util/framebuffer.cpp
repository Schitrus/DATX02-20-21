//
// Created by Kalle on 2020-03-10.
//

#include <android/log.h>
#include "framebuffer.h"
#include "helper.h"

#define LOG_TAG "framebuffer"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void Framebuffer::create(int width, int height, GLuint inFormat ,GLuint format){
    LOG_INFO("Creating a complete framebuffer with size %d x %d", width, height);
    clearGLErrors("framebuffer creation");

    this->width = width;
    this->height = height;
    this -> inFormat = inFormat;
    this -> format = format;
    // framebuffer configuration
    // -------------------------
    FBO.init();
    FBO.bind();

    // create a color attachment texture
    glGenTextures(1, &colorTextureTarget);
    glBindTexture(GL_TEXTURE_2D, colorTextureTarget);

    glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, GL_RGBA, format, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureTarget, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    // use a single renderbuffer object for both a depth AND stencil buffer.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // now actually attach it
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    checkGLError("framebuffer creation");

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    unbind();
}

void Framebuffer::create(int width, int height) {
    create(width, height,GL_RGB8 ,GL_UNSIGNED_BYTE);
}

void Framebuffer::clear() {
    FBO.clear();
    glDeleteTextures(1, &colorTextureTarget);
    colorTextureTarget = 0;
    glDeleteRenderbuffers(1, &RBO);
    RBO = 0;
}

void Framebuffer::resize(int width, int height) {
    if(this->width != width || this->height != height) {
        this->width = width;
        this->height = height;
        FBO.bind();
        // Allocate a texture
        glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
        glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, GL_RGBA, format, NULL);

        // Allocate for renderBuffer
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        FBO.unbind();
    }
}

GLuint Framebuffer::texture(){
    return colorTextureTarget;
}

bool Framebuffer::bind(const char *tag) {
    FBO.bind();
    return checkFramebufferStatus(GL_FRAMEBUFFER, tag);
}

void Framebuffer::unbind() {
    FBO.unbind();
}