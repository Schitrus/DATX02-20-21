//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_FIRE_RENDERER_H
#define DATX02_20_21_FIRE_RENDERER_H

#include <jni.h>
#include <GLES3/gl31.h>

namespace fire{

    void initGraphics(int width, int height);

    void updateGraphics();

    void scale(float scaleFactor, double scaleX, double scaleY);

    void touch(double dx, double dy);

}

#endif //DATX02_20_21_FIRE_RENDERER_H
