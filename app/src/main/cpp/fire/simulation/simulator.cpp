//
// Created by Kalle on 2020-03-04.
//
#include "simulator.h"
#include "field_initialization.h"
#include "fire/util/helper.h"

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#include <chrono>
#include <cstdlib>

#define NOW std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::now())
#define DURATION(a, b) (std::chrono::duration_cast<std::chrono::milliseconds>(a - b)).count() / 1000.0f;

#define LOG_TAG "Simulators"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int Simulator::init(Settings* settings) {

    slab = new SlabOperation();
    if (!slab->init())
        return 0;

    operations = new SimulationOperations();
    if(!operations->init(slab, settings))
        return 0;

    wavelet = new WaveletTurbulence();
    if(!wavelet->init(slab, settings))
        return 0;

    initData(settings);

    dt = settings->getDeltaTime();
    buoyancyScale = settings->getBuoyancyScale();
    windScale = settings->getWindScale();
    velKinematicViscosity = settings->getVelKinematicViscosity();
    velDiffusionIterations = settings->getVelDiffusionIterations();
    vorticityScale = settings->getVorticityScale();
    projectionIterations = settings->getProjectionIterations();
    sourceMode = settings->getSourceMode();
    tempKinematicViscosity = settings->getTempKinematicViscosity();
    tempDiffusionIterations = settings->getTempDiffusionIterations();
    smokeKinematicViscosity = settings->getSmokeKinematicViscosity();
    smokeDiffusionIterations = settings->getSmokeDiffusionIterations();
    smokeDissipation = settings->getSmokeDissipation();

    start_time = NOW;
    last_time = start_time;

    LOG_INFO("Finished initializing simulator");

    return 1;
}

int Simulator::changeSettings(Settings* settings, bool shouldRegenFields) {

    dt = settings->getDeltaTime();
    buoyancyScale = settings->getBuoyancyScale();
    windScale = settings->getWindScale();
    velKinematicViscosity = settings->getVelKinematicViscosity();
    velDiffusionIterations = settings->getVelDiffusionIterations();
    vorticityScale = settings->getVorticityScale();
    projectionIterations = settings->getProjectionIterations();
    sourceMode = settings->getSourceMode();
    tempKinematicViscosity = settings->getTempKinematicViscosity();
    tempDiffusionIterations = settings->getTempDiffusionIterations();
    smokeKinematicViscosity = settings->getSmokeKinematicViscosity();
    smokeDiffusionIterations = settings->getSmokeDiffusionIterations();
    smokeDissipation = settings->getSmokeDissipation();


    if(shouldRegenFields) {
        //clearData();
        initData(settings);
    }

    return operations->changeSettings(settings, shouldRegenFields) && wavelet->changeSettings(settings, shouldRegenFields);
}

void Simulator::update(GLuint& densityData, GLuint& temperatureData, ivec3& size) {
    // todo maybe put a cap on the delta time to not get too big time steps during lag?
    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;
    if(dt != 0.0f)
        delta_time = dt;

    slab->prepare();

    velocityStep(delta_time);

    smokeDensityStep(delta_time);

    temperatureStep(delta_time);

    slab->finish();

    getData(densityData, temperatureData, size);

}

void Simulator::getData(GLuint& densityData, GLuint& temperatureData, ivec3& size) {
    temperatureData = temperature->getDataTexture();
    densityData = smokeDensity->getDataTexture();
    ivec3 highResSize = temperature->getSize();

    size = highResSize;
}

void Simulator::initData(Settings* settings) {

    ivec3 lowResSize = settings->getSize(Resolution::velocity);
    ivec3 highResSize = settings->getSize(Resolution::substance);

    float* density_field = createScalarField(0.0f, highResSize);
    float* density_source = createScalarField(0.0f, highResSize);
    float* temperature_field = createScalarField(0.0f, highResSize);
    float* temperature_source = createScalarField(0.0f, highResSize);
    vec3* velocity_field = createVectorField(vec3(0.0f), lowResSize);
    vec3* velocity_source = createVectorField(vec3(0.0f), lowResSize);

    initSourceField(density_source, settings->getSmokeSourceDensity(), Resolution::substance, settings);
    initSourceField(temperature_source, settings->getTempSourceDensity(), Resolution::substance, settings);

    smokeDensity = createScalarDataPair(density_field, Resolution::substance, settings);
    createScalar3DTexture(densitySource, highResSize, density_source);

    temperature = createScalarDataPair(temperature_field, Resolution::substance, settings);
    createScalar3DTexture(temperatureSource, highResSize, temperature_source);

    lowerVelocity = createVectorDataPair(velocity_field, Resolution::velocity, settings);
    higherVelocity = createVectorDataPair(nullptr, Resolution::substance, settings);
    createVector3DTexture(velocitySource, lowResSize, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::clearData() {
    delete smokeDensity, delete temperature, delete lowerVelocity, delete higherVelocity;
    glDeleteTextures(1, &densitySource);
    glDeleteTextures(1, &temperatureSource);
    glDeleteTextures(1, &velocitySource);
}

void Simulator::velocityStep(float dt){
    // Source
    if(buoyancyScale != 0.0f)
        operations->buoyancy(lowerVelocity, temperature, buoyancyScale, dt);

    if(windScale != 0.0f)
        updateAndApplyWind(windScale, dt);

    // Advect
    operations->advect(lowerVelocity, lowerVelocity, true, dt);

    // Diffuse
    if(velKinematicViscosity != 0.0f)
        operations->diffuse(lowerVelocity, Resolution::velocity,
                velDiffusionIterations, velKinematicViscosity, dt);

    // Vorticity
    if(vorticityScale != 0.0f)
        operations->createVorticity(lowerVelocity, vorticityScale, dt);
  
    // Project
    if(projectionIterations != 0)
        operations->project(lowerVelocity, projectionIterations);

    // Go from low-res velocity to high-res velocity using Wavelet
    wavelet->waveletStep(lowerVelocity, higherVelocity, dt);
}

void Simulator::updateAndApplyWind(float scale, float dt) {

    windAngle += dt*0.5f;

    float windStrength = scale;
    operations->addWind(lowerVelocity, windAngle, windStrength, dt);
}

void Simulator::temperatureStep(float dt) {
    // Force
    operations->addSource(temperature, temperatureSource, sourceMode, dt);

    // Advection
    operations->advect(higherVelocity, temperature, false, dt);

    // Diffusion
    if(tempKinematicViscosity != 0.0f)
        operations->diffuse(temperature, Resolution::substance,
                            tempDiffusionIterations, tempKinematicViscosity, dt);

    // Dissipation
    operations->heatDissipation(temperature, dt);

}

void Simulator::smokeDensityStep(float dt) {

    // addForce
    operations->addSource(smokeDensity, densitySource, sourceMode, dt);

    // Advect
    operations->advect(higherVelocity, smokeDensity, false, dt);

    // Diffuse
    if(smokeKinematicViscosity != 0.0f)
        operations->diffuse(smokeDensity, Resolution::substance,
                smokeDiffusionIterations, smokeKinematicViscosity, dt);

    // Dissipate
    if(smokeDissipation != 0.0f)
        operations->dissipate(smokeDensity, smokeDissipation, dt);

}
