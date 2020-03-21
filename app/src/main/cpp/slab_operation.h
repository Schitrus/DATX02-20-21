//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <GLES3/gl31.h>

class SlabOperator{
    int grid_width, grid_height, grid_depth;
public:
    void init();

    void resize(int width, int height, int depth);

    void update();
private:
    void initData();
    void initVelocity(int size);

    void initPressure(float* data);

    void initDensity(float* data);

    void initTemperature(float* data);

    void initSources();

    void initLine();

    void initQuad();

    void initProgram();

    void slabOperation();
    void slabOperation(GLuint interiorProgram, GLuint boundariesProgram, int layer, float scale);

    void display_results();
};

#endif //DATX02_20_21_SLAB_OPERATION_H
