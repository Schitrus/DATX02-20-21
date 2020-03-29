//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_SIMULATOR_H
#define DATX02_20_21_SIMULATOR_H

#include <jni.h>
#include <GLES3/gl32.h>

#include "slab_operation.h"

class Simulator{
    SlabOperator slab;
public:
    void init();
    void resize(int width, int height, int depth);
    void update();

    void getData(GLuint& pressure, GLuint& temperature, int& width, int& height, int& depth);

};

#endif //DATX02_20_21_SIMULATOR_H
