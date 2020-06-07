//
// Created by Kalle on 2020-03-04.
//
#include "simulator.h"
#include "field_initialization.h"
#include "fire/util/helper.h"

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

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

    buoyancy_direction = vec3(0.0f, 1.0f, 0.0f);
    rotation = 0.0f;

    dt = settings->getDeltaTime();
    buoyancyScale = settings->getBuoyancyScale();
    windScale = settings->getWindScale();
    windAngle = settings->getWindAngle();
    rotatingWindAngle = settings->getRotatingWindAngle();
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

    slab->boundaryMode(settings->getBoundaryType());

    orientationMode = settings->getOrientationMode();

    start_time = NOW;
    last_time = start_time;

    LOG_INFO("Finished initializing simulator");

    return 1;
}

void Simulator::setRotation(float rotation){
    this->rotation = rotation;
};

int Simulator::changeSettings(Settings* settings, bool shouldRegenFields) {

    buoyancy_direction = vec3(0.0f, 1.0f, 0.0f);

    dt = settings->getDeltaTime();
    buoyancyScale = settings->getBuoyancyScale();
    windScale = settings->getWindScale();
    windAngle = settings->getWindAngle();
    rotatingWindAngle = settings->getRotatingWindAngle();
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

    slab->boundaryMode(settings->getBoundaryType());

    orientationMode = settings->getOrientationMode();

    if(shouldRegenFields) {
        clearData();
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

void Simulator::updateDeviceRotationMatrix(float *rotationMatrix){
    // Update global variable deviceRotationMatrix with correct value in simulation file
    if(orientationMode)
        return;


    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            deviceRotationMatrix[i][j] = rotationMatrix[3*i + j];
        }
    }

    buoyancy_direction = deviceRotationMatrix * vec3(0.0f, 0.0f, 1.0f);
    buoyancy_direction[2] = 0.0f; // Project to xy-plane by setting z-coordinate to 0
    buoyancy_direction = normalize(buoyancy_direction); // Normalize to get a vector of length 1

    buoyancy_direction = vec3(rotate(mat4(1.0f), rotation, vec3(0.0f, 1.0f, 0.0f)) * vec4(buoyancy_direction, 0.0f));

    LOG_INFO("Device rotation matrix: %s", glm::to_string(deviceRotationMatrix).c_str());
}


void Simulator::initData(Settings* settings) {

    settings->printInfo("SIMULATOR");

    ivec3 lowResSize = settings->getSize(Resolution::velocity);
    ivec3 highResSize = settings->getSize(Resolution::substance);

    float lowScaleFactor = 1.0f/settings->getResToSimFactor(Resolution::velocity);
    float highScaleFactor = 1.0f/settings->getResToSimFactor(Resolution::substance);

    float* density_field = createScalarField(0.0f, highResSize);
    float* density_source = createScalarField(0.0f, highResSize);
    float* temperature_field = createScalarField(0.0f, highResSize);
    float* temperature_source = createScalarField(0.0f, highResSize);
    vec3* velocity_field = createVectorField(vec3(0.0f), lowResSize);
    vec3* velocity_source = createVectorField(vec3(0.0f), lowResSize);

    initSourceField(density_source, settings->getSourceDensity(), Resolution::substance, settings);
    initSourceField(temperature_source, settings->getSourceTemperature(), Resolution::substance, settings);
    initSourceField(velocity_source, settings->getSourceVelocity(), Resolution::velocity, settings);

    smokeDensity = createScalarDataPair(density_field, highResSize, highScaleFactor);
    createScalar3DTexture(densitySource, highResSize, density_source);

    temperature = createScalarDataPair(temperature_field, highResSize, highScaleFactor);
    createScalar3DTexture(temperatureSource, highResSize, temperature_source);

    lowerVelocity = createVectorDataPair(velocity_field, lowResSize, lowScaleFactor);
    higherVelocity = createVectorDataPair(nullptr, highResSize, highScaleFactor);
    createVector3DTexture(velocitySource, lowResSize, velocity_source);

    force_field = createVectorField(vec3(0.0f, 0.0f,0.0f), lowResSize);

    delete[] density_field;
    delete[] density_source;
    delete[] temperature_field;
    delete[] temperature_source;
    delete[] velocity_field;
    delete[] velocity_source;
}

void Simulator::clearData() {
    delete smokeDensity;
    delete temperature;
    delete lowerVelocity;
    delete higherVelocity;
    glDeleteTextures(1, &densitySource);
    glDeleteTextures(1, &temperatureSource);
    glDeleteTextures(1, &velocitySource);
}



void Simulator::velocityStep(float delta_time){
    // Source
    if(buoyancyScale != 0.0f)
        operations->buoyancy(lowerVelocity, temperature, buoyancy_direction, buoyancyScale, delta_time);

    if(windScale != 0.0f)
        updateAndApplyWind(windScale, delta_time);

    if(externalForceReady){
        operations->externalForce(lowerVelocity, force, delta_time);
        delete[] force_field;
        ivec3 lowResSize = lowerVelocity->getSize();
        force_field = createVectorField(vec3(0.0f, 0.0f,0.0f), lowResSize);
        externalForceReady = false;
    }

    // Advect
    operations->advect(lowerVelocity, lowerVelocity, true, delta_time);

    // Diffuse
    if(velKinematicViscosity != 0.0f)
        operations->diffuse(lowerVelocity, Resolution::velocity,
                velDiffusionIterations, velKinematicViscosity, delta_time);

    // Vorticity
    if(vorticityScale != 0.0f)
        operations->createVorticity(lowerVelocity, vorticityScale, delta_time);
  
    // Project
    if(projectionIterations != 0)
        operations->project(lowerVelocity, projectionIterations);

    // Go from low-res velocity to high-res velocity using Wavelet
    wavelet->waveletStep(lowerVelocity, higherVelocity, delta_time);
}

void Simulator::updateAndApplyWind(float scale, float delta_time) {
    if(rotatingWindAngle)
        windAngle += 90.0*delta_time;

    float windStrength = scale;
    operations->addWind(lowerVelocity, PI * windAngle / 180.0f, windStrength, delta_time);
}

void Simulator::temperatureStep(float delta_time) {
    // Force
    operations->addSource(temperature, temperatureSource, sourceMode, delta_time);

    // Advection
    operations->advect(higherVelocity, temperature, false, delta_time);

    // Diffusion
    if(tempKinematicViscosity != 0.0f)
        operations->diffuse(temperature, Resolution::substance,
                            tempDiffusionIterations, tempKinematicViscosity, delta_time);

    // Dissipation
    operations->heatDissipation(temperature, delta_time);

}

void Simulator::smokeDensityStep(float delta_time) {

    // addForce
    operations->addSource(smokeDensity, densitySource, sourceMode, delta_time);

    // Advect
    operations->advect(higherVelocity, smokeDensity, false, delta_time);

    // Diffuse
    if(smokeKinematicViscosity != 0.0f)
        operations->diffuse(smokeDensity, Resolution::substance,
                smokeDiffusionIterations, smokeKinematicViscosity, delta_time);

    // Dissipate
    if(smokeDissipation != 0.0f)
        operations->dissipate(smokeDensity, smokeDissipation, delta_time);

}

void Simulator::addExternalForce(vec3 position, vec3 vector, Settings* settings) {
    float radius = 4.0f;

    LOG_INFO("POSITION: %f, %f, %f", position.x, position.y, position.z);

    fillSphere(force_field, vector * 100.0f, settings->getSimulationSize() * position, radius, Resolution::velocity, settings);

    createVector3DTexture(force, settings->getSize(Resolution::velocity), force_field);

    externalForceReady = true;
}
