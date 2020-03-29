//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_RENDERER_H
#define DATX02_20_21_RENDERER_H

#include <jni.h>
#include <GLES3/gl32.h>

#include <android/asset_manager.h>

#include "ray_renderer.h"

class Renderer{
    int window_width, window_height;
    RayRenderer rayRenderer;
public:
    void init(AAssetManager* assetManager);
    void resize(int width, int height);
    void update();

    void scale(float scaleFactor, double scaleX, double scaleY);
    void touch(double dx, double dy);

    void setData(GLuint density, GLuint temperature, int width, int height, int depth);
};

#endif //DATX02_20_21_RENDERER_H
