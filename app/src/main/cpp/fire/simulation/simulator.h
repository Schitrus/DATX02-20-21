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

class Simulator {
    SlabOperation* slab;
    SimulationOperations* operations;
    WaveletTurbulence* wavelet;

    DataTexturePair* smokeDensity;
    DataTexturePair* temperature;
    DataTexturePair* lowerVelocity;
    DataTexturePair* higherVelocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    //External force
    GLuint force;
    vec3* force_field;
    bool externalForceReady;

    mat3 deviceRotationMatrix;
    vec3 buoyancy_direction;

    float dt;

    float buoyancyScale;

    float velKinematicViscosity;
    int velDiffusionIterations;

    float vorticityScale;

    int projectionIterations;

    SourceMode sourceMode;

    float tempKinematicViscosity;
    int tempDiffusionIterations;

    float smokeKinematicViscosity;
    int smokeDiffusionIterations;

    float smokeDissipation;

    float windScale;
    float windAngle = 0.0f;
    bool rotatingWindAngle = true;

    bool orientationMode;

    float rotation;

    // Time
    time_point<system_clock> start_time, last_time;

public:

    int init(Settings* settings);

    int changeSettings(Settings* settings, bool shouldRegenFields);

    void update(GLuint& densityData, GLuint& temperatureData, ivec3& size);

    void addExternalForce(vec3 position, vec3 vector, Settings* settings);

    void updateDeviceRotationMatrix(float *rotationMatrix);

    void setRotation(float rotation);

private:

    void initData(Settings* settings);

    void clearData();

    void getData(GLuint& densityData, GLuint& temperatureData, ivec3& size);

    // Performs one fire.simulation step for velocity
    void velocityStep(float delta_time);

    void updateAndApplyWind(float scale, float delta_time);

    void temperatureStep( float delta_time);

    void smokeDensityStep(float delta_time);
};

#endif //DATX02_20_21_SIMULATOR_H
