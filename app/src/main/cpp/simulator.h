//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_SIMULATOR_H
#define DATX02_20_21_SIMULATOR_H

#include <jni.h>
#include <GLES3/gl32.h>
#include <chrono>

#include "slab_operation.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class Simulator{
    int grid_width, grid_height, grid_depth;
    SlabOperator slab;
    DataTexturePair* density;
    DataTexturePair* temperature;
    DataTexturePair* velocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    // Time
    time_point<system_clock> start_time, last_time;

public:
    void init();

    void resize(int width, int height, int depth);

    void update();

    void getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth);

private:

    void initData();

    // Performs one simulation step for velocity
    void velocityStep(float dt);

    void temperatureStep( float dt);

    void densityStep(float dt);

    // Performs the usual steps for moving substances using the fluid velocity field
    // It will not perform the "add force" step, as that depends entirely on the individual substance
    void substanceMovementStep(DataTexturePair *data, float dissipationRate, float dt);

};

#endif //DATX02_20_21_SIMULATOR_H
