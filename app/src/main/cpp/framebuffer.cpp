//
// Created by Kalle on 2020-03-10.
//

#include <android/log.h>
#include "framebuffer.h"

#define LOG_TAG "framebuffer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void Framebuffer::create(int width, int height, bool simple) {
    this->simple = simple;
    this->width = width;
    this->height = height;

    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &FBO);
    use();

    if(simple){
        null();
        return;
    }

    // create a color attachment texture
    glGenTextures(1, &colorTextureTarget);
    glBindTexture(GL_TEXTURE_2D, colorTextureTarget);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

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


    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ALOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    null();

}

void Framebuffer::resize(int width, int height){
    this->width = width;
    this->height = height;
    if(simple)
        return;
    use();
    // Allocate a texture
    glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // Allocate for renderBuffer
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    null();
}

GLuint Framebuffer::texture(){
    return colorTextureTarget;
}

void Framebuffer::use() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::null() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}