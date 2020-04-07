//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_SIMULATOR_H
#define DATX02_20_21_SIMULATOR_H

#include <jni.h>
#include <gles3/gl31.h>
#include <chrono>

#include "slab_operation.h"
#include "wavelet_turbulence.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class Simulator{
    ivec3 sizeRatio;
    int lowerResolution, higherResolution;  //Simulation resolutions is sizeRatio * ___Resolution
    float simulationScale;   //simulation size in meters is sizeRatio * meterScale

    SlabOperator slab;
    WaveletTurbulence wavelet;

    DataTexturePair* density;
    DataTexturePair* temperature;
    DataTexturePair* lowerVelocity;
    DataTexturePair* higherVelocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    // Time
    time_point<system_clock> start_time, last_time;

public:
    int init();

    void update();

    void getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth);

private:

    void initSize(ivec3 ratio, int lowerResolution, int higherResolution, float simulationScale);

    void initData();

    // Performs one simulation step for velocity
    void velocityStep(float dt);

    void waveletStep(float dt);

    void temperatureStep( float dt);

    void densityStep(float dt);

    // Performs the usual steps for moving substances using the fluid velocity field
    // It will not perform the "add force" step, as that depends entirely on the individual substance
    void substanceMovementStep(DataTexturePair *data, float dissipationRate, float dt);

    void clearField(float* field, float value, ivec3 gridSize);

    void clearField(vec3* field, vec3 value, ivec3 gridSize);

    // density is in unit/m^3
    void fillExtensive(float* field, float density, vec3 minPos, vec3 maxPos, ivec3 gridSize);

    // value is in unit
    void fillIntensive(float* field, float value, vec3 minPos, vec3 maxPos, ivec3 gridSize);

    // fills the field with vectors pointing outward from the center,
    // and that scale with the distance from the center
    // scale is unit/meter from center
    void fillOutgoingVector(vec3* field, float scale, vec3 minPos, vec3 maxPos, ivec3 gridSize);

    bool hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

    // might return negative values if there is no overlap
    float getOverlapArea(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

    ivec3 lowerResolutionSize();

    ivec3 higherResolutionSize();

};

#endif //DATX02_20_21_SIMULATOR_H
