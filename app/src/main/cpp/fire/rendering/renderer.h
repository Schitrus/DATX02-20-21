//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_RENDERER_H
#define DATX02_20_21_RENDERER_H

#include <jni.h>
#include <GLES3/gl31.h>

#include <android/asset_manager.h>
#include <fire/settings.h>

#include "fire/rendering/ray_renderer.h"

class Renderer{
    int window_width, window_height;
    RayRenderer rayRenderer;
    Settings* settings;
public:
    int init(Settings* settings);

    int changeSettings(Settings* settings);

    void resize(int width, int height);
    void update(GLuint density, GLuint temperature, ivec3 size);

    void scale(float scaleFactor, double scaleX, double scaleY);
    void touch(double dx, double dy);
};

#endif //DATX02_20_21_RENDERER_H
