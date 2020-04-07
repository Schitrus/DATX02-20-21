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

extern const ivec3 sizeRatio = ivec3(1, 4, 1);
extern const int lowResScale = 12;
extern const int highResScale = 60;
extern const float simulationScale = 12.0f;
// size of low resolution textures. This also includes the border of the texture
extern const ivec3 lowResSize = sizeRatio * lowResScale + ivec3(2, 2, 2);
// size of high resolution textures. This also includes the border of the texture
extern const ivec3 highResSize = sizeRatio * highResScale + ivec3(2, 2, 2);
// size of simulation space in meters. This does not include the border that is included in the resolution sizes
extern const vec3 simulationSize = sizeRatio * simulationScale;

class Simulator{
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

};

#endif //DATX02_20_21_SIMULATOR_H
