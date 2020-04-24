//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_SIMULATOR_H
#define DATX02_20_21_SIMULATOR_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <chrono>
#include <fire/settings.h>

#include "simulation_operations.h"
#include "wavelet_turbulence.h"

using std::chrono::time_point;
using std::chrono::system_clock;

extern const ivec3 sizeRatio;
extern const int lowResScale;
extern const int highResScale;
extern const float simulationScale;
// size of low resolution textures. This also includes the border of the texture
extern const ivec3 lowResSize;
// size of high resolution textures. This also includes the border of the texture
extern const ivec3 highResSize;
// size of fire.simulation space in meters. This does not include the border that is included in the resolution sizes
extern const vec3 simulationSize;

class Simulator {
    SlabOperator* slab;
    SimulationOperations* operations;
    WaveletTurbulence* wavelet;

    Settings settings;

    DataTexturePair* density;
    DataTexturePair* temperature;
    DataTexturePair* lowerVelocity;
    DataTexturePair* higherVelocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    float windAngle = 3.14f;

    // Time
    time_point<system_clock> start_time, last_time;

public:

    int init(Settings settings);

    int changeSettings(Settings settings);

    void update();

    void getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth);

private:

    void initData();

    void clearData();

    // Performs one fire.simulation step for velocity
    void velocityStep(float dt);

    void waveletStep(float dt);

    void updateAndApplyWind(float dt);

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

    void fillSphere(float* field, float value, vec3 center, float radius, vec3 size);
    void fillSphere(vec3* field, vec3 value, vec3 center, float radius, vec3 size);

    // fills the field with vectors pointing outward from the center,
    // and that scale with the distance from the center
    // scale is unit/meter from center
    void fillOutgoingVector(vec3* field, float scale, vec3 minPos, vec3 maxPos, ivec3 gridSize);

    bool hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

    // might return negative values if there is no overlap
    float getOverlapVolume(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

};

#endif //DATX02_20_21_SIMULATOR_H
