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

#define LOG_TAG "Simulator"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int Simulator::init(Settings settings) {

    slab = new SlabOperator();
    operations = new SimulationOperations();
    wavelet = new WaveletTurbulence();

    if (!slab->init())
        return 0;

    if(!operations->init(slab, settings))
        return 0;

    if(!wavelet->init(slab, settings))
        return 0;

    this->settings = settings;
    initData();
    start_time = NOW;
    last_time = start_time;
    return 1;
}

int Simulator::changeSettings(Settings settings) {

    clearData();

    this->settings = settings;
    initData();
    start_time = NOW;
    last_time = start_time;

    return operations->changeSettings(settings) && wavelet->changeSettings(settings);
}

void Simulator::update(){
    // todo maybe put a cap on the delta time to not get too big time steps during lag?
    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;
    if(settings.getDeltaTime() != 0.0f)
        delta_time = settings.getDeltaTime();

    slab->prepare();

    velocityStep(delta_time);

    waveletStep(delta_time);

    densityStep(delta_time);

    temperatureStep(delta_time);

    slab->finish();

}

void Simulator::getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth){
    temperatureData = temperature->getDataTexture();
    densityData = density->getDataTexture();
    ivec3 highResSize = settings.getSize(Resolution::substance);
    width = highResSize.x;
    height = highResSize.y;
    depth = highResSize.z;
}

void Simulator::initData() {
    ivec3 lowResSize = settings.getSize(Resolution::velocity);
    ivec3 highResSize = settings.getSize(Resolution::substance);

    float* density_field = createScalarField(0.0f, highResSize);
    float* density_source = createScalarField(0.0f, highResSize);
    float* temperature_field = createScalarField(0.0f, highResSize);
    float* temperature_source = createScalarField(0.0f, highResSize);
    vec3* velocity_field = createVectorField(vec3(0.0f), lowResSize);
    vec3* velocity_source = createVectorField(vec3(0.0f), lowResSize);

    initSourceField(density_source, settings.getSmokeSourceDensity(), Resolution::substance, settings);
    initSourceField(temperature_source, settings.getTempSourceDensity(), Resolution::substance, settings);

    density = createScalarDataPair(density_field, Resolution::substance, settings);
    createScalar3DTexture(&densitySource, highResSize, density_source);

    temperature = createScalarDataPair(temperature_field, Resolution::substance, settings);
    createScalar3DTexture(&temperatureSource, highResSize, temperature_source);

    lowerVelocity = createVectorDataPair(velocity_field, Resolution::velocity, settings);
    higherVelocity = createVectorDataPair(nullptr, Resolution::substance, settings);
    createVector3DTexture(&velocitySource, lowResSize, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::clearData() {
    delete density, delete temperature, delete lowerVelocity, delete higherVelocity;
    glDeleteTextures(1, &densitySource);
    glDeleteTextures(1, &temperatureSource);
    glDeleteTextures(1, &velocitySource);
}

void Simulator::velocityStep(float dt){
    // Source
    if(settings.getBuoyancyScale() != 0.0f)
        operations->buoyancy(lowerVelocity, temperature, settings.getBuoyancyScale(), dt);

    if(settings.getWindScale() != 0.0f)
        updateAndApplyWind(settings.getWindScale(), dt);

    // Advect
    operations->advection(lowerVelocity, lowerVelocity, dt);

    // Diffuse
    if(settings.getVelKinematicViscosity() != 0.0f)
        operations->velocityDiffusion(lowerVelocity, settings.getVelDiffusionIterations(), settings.getVelKinematicViscosity(), dt);

    // Vorticity
    if(settings.getVorticityScale() != 0.0f)
        operations->vorticity(lowerVelocity, settings.getVorticityScale(), dt);
  
    // Project
    if(settings.getProjectionIterations() != 0)
        operations->projection(lowerVelocity, settings.getProjectionIterations());
}

void Simulator::waveletStep(float dt){
    // Advect texture coordinates
    wavelet->advection(lowerVelocity, dt);

    wavelet->calcEnergy(lowerVelocity);

    wavelet->fluidSynthesis(lowerVelocity, higherVelocity);
}

void Simulator::updateAndApplyWind(float scale, float dt) {

    windAngle += dt*0.5f;

    float windStrength = scale*(12.0f + 11*sin(windAngle*2.14f + 123));
    LOG_INFO("Angle: %f, Wind: %f", windAngle, windStrength);
    operations->addWind(lowerVelocity, windAngle, windStrength, dt);
}

void Simulator::temperatureStep(float dt) {
    // Force
    handleSource(temperature, temperatureSource, dt);

    // Advection
    operations->fulladvection(higherVelocity, temperature, dt);

    // Diffusion
    if(settings.getTempKinematicViscosity() != 0.0f)
        operations->substanceDiffusion(temperature, settings.getTempDiffusionIterations(), settings.getTempKinematicViscosity(), dt);

    // Dissipation
    operations->heatDissipation(temperature, dt);

}

void Simulator::densityStep(float dt){
    // addForce
    handleSource(density, densitySource, dt);

    // Advect
    operations->fulladvection(higherVelocity, density, dt);

    // Diffuse
    if(settings.getSmokeKinematicViscosity() != 0.0f)
        operations->substanceDiffusion(density, settings.getSmokeDiffusionIterations(), settings.getSmokeKinematicViscosity(), dt);

    // Dissipate
    if(settings.getSmokeDissipation() != 0.0f)
        operations->dissipate(density, settings.getSmokeDissipation(), dt);
}

void Simulator::handleSource(DataTexturePair *substance, GLuint source, float dt) {
    if(settings.getSourceMode() == SourceMode::set)
        operations->setSource(substance, source, dt);
    else operations->addSource(substance, source, dt);
}
