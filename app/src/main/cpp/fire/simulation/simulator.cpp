//
// Created by Kalle on 2020-03-04.
//
#include "simulator.h"
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

#define MAX_TEMPERATURE

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

    slab->prepare();

    delta_time = 1/30.0f;

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
    vec3 simulationSize = settings.getSimulationSize();

    int lowerSize = lowResSize.x * lowResSize.y * lowResSize.z;
    int higherSize = highResSize.x * highResSize.y * highResSize.z;

    float* density_field = new float[higherSize];
    float* density_source = new float[higherSize];
    float* temperature_field = new float[higherSize];
    float* temperature_source = new float[higherSize];
    vec3* velocity_field = new vec3[lowerSize];
    vec3* velocity_source = new vec3[lowerSize];

    clearField(density_field, 0.0f, highResSize);
    clearField(density_source, 0.0f, highResSize);
    clearField(temperature_field, 0.0f, highResSize);
    clearField(temperature_source, 0.0f, highResSize);
    clearField(velocity_field, vec3(0.0f, 0.0f, 0.0f), lowResSize);
    clearField(velocity_source, vec3(0.0f, 0.0f, 0.0f), lowResSize);

    float radius = 16;
    float middleW = simulationSize.x / 2;
    float middleD = simulationSize.z / 2;
    vec3 start = vec3(middleW - radius, 3 - radius, middleD - radius);
    vec3 end = vec3(middleW + radius, 3 + radius, middleD + radius);

    vec3 center = vec3(0.5f, 0.2f, 0.5f) * vec3(highResSize);

    //fillOutgoingVector(velocity_source, 10.0f, start, end, Resolution::velocity, settings);

    fillSphere(density_source, 0.4f, center, radius, Resolution::substance, settings);
    fillSphere(temperature_source, 3500.0f, center, radius, Resolution::substance, settings);
    //fillSphere(velocity_source, vec3(8.0f, 1.0f, 2.0f), center, 4.0f*radius, Resolution::velocity, settings);

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
    operations->buoyancy(lowerVelocity, temperature, dt, 0.15f);

    //updateAndApplyWind(dt);
    // Advect
    operations->advection(lowerVelocity, lowerVelocity, dt);

    // Diffuse
    if(settings.getVelKinematicViscosity() != 0.0f)
        operations->velocityDiffusion(lowerVelocity, settings.getVelDiffusionIterations(), settings.getVelKinematicViscosity(), dt);

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

void Simulator::updateAndApplyWind(float dt) {

    windAngle += dt*0.5f;

    float windStrength = 12.0 + 11*sin(windAngle*2.14+123);
    LOG_INFO("Angle: %f, Wind: %f", windAngle, windStrength);
    operations->addWind(lowerVelocity, windAngle, windStrength, dt);
}

void Simulator::temperatureStep(float dt) {

    operations->setSource(temperature, temperatureSource, dt);

    operations->fulladvection(higherVelocity, temperature, dt);

    operations->heatDissipation(temperature, dt);

}

void Simulator::densityStep(float dt){
    // addForce
    operations->setSource(density, densitySource, dt);

    // Advect
    operations->fulladvection(higherVelocity, density, dt);
}

void Simulator::substanceMovementStep(DataTexturePair *data, float dissipationRate, float dt) {
    //todo use this for temperature and density when advection has been seperated from the heat dissipation part in the temperature shader
}

void Simulator::clearField(float* field, float value, ivec3 gridSize) {
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                int index = gridSize.x * (gridSize.y * z + y) + x;
                field[index] = value;
            }
        }
    }
}

void Simulator::clearField(vec3* field, vec3 value, ivec3 gridSize) {
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                int index = gridSize.x * (gridSize.y * z + y) + x;
                field[index] = value;
            }
        }
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
void Simulator::fillExtensive(float *field, float density, vec3 minPos, vec3 maxPos, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {
                //Lower corner of cell in simulation space (no border)
                vec3 pos = vec3(x, y, z) * toSimulationScale;
                //Upper corner of cell in simulation space (no border)
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) * toSimulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = density*overlappedVolume;
                }
            }
        }
    }
}

void Simulator::fillIntensive(float *field, float value, vec3 minPos, vec3 maxPos, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    //Volume for an 1x1x1 voxel cell, in meters
    float cellVolume = (1 * toSimulationScale) * (1 * toSimulationScale) * (1 * toSimulationScale);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {
                //Lower corner of cell in simulation space (no border)
                vec3 pos = vec3(x, y, z) * toSimulationScale;
                //Upper corner of cell in simulation space (no border)
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) * toSimulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = value*(overlappedVolume/cellVolume);
                }
            }
        }
    }
}

void Simulator::fillOutgoingVector(vec3 *field, float scale, vec3 minPos, vec3 maxPos, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    //center position of filled area in simulation space
    vec3 center = (minPos + maxPos)/2.0f;
    //Volume for an 1x1x1 voxel cell, in meters
    float cellVolume = (1 * toSimulationScale) * (1 * toSimulationScale) * (1 * toSimulationScale);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {
                //Lower corner of cell in simulation space (no border)
                vec3 pos = vec3(x, y, z) * toSimulationScale;
                //Upper corner of cell in simulation space (no border)
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) * toSimulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    vec3 vector = pos - center;
                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = vector*(scale*overlappedVolume/cellVolume);
                }
            }
        }
    }
}


void Simulator::fillSphere(float* field, float value, vec3 center, float radius, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {

                //Center of cell in simulation space (no border)
                vec3 pos = vec3(x + 0.5f, y + 0.5f, z + 0.5f) * toSimulationScale;

                if(distance(pos, center) <= radius) {

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = value;
                }
            }
        }
    }
}

void Simulator::fillSphere(vec3* field, vec3 value, vec3 center, float radius, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {

                //Center of cell in simulation space (no border)
                vec3 pos = vec3(x + 0.5f, y + 0.5f, z + 0.5f) * toSimulationScale;

                if(distance(pos, center) <= radius) {

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = value;
                }
            }
        }
    }
}


#pragma clang diagnostic pop

bool Simulator::hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    return max1.x > min2.x && max1.y > min2.y && max1.z > min2.z
           && min1.x < max2.x && min1.y < max2.y && min1.z < max2.z;
}

float Simulator::getOverlapVolume(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    vec3 overlapMin = max(min1, min2);
    vec3 overlapMax = min(max1, max2);
    return (overlapMax.x - overlapMin.x)*(overlapMax.y - overlapMin.y)*(overlapMax.z - overlapMin.z);
}
