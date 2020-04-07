//
// Created by Kalle on 2020-03-04.
//
#include "simulator.h"
#include "helper.h"

#include <jni.h>
#include <gles3/gl31.h>

#include <chrono>

#define NOW std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::now())
#define DURATION(a, b) (std::chrono::duration_cast<std::chrono::milliseconds>(a - b)).count() / 1000.0f;

int Simulator::init(){

    if (!slab.init() || !wavelet.init(lowResSize, highResSize))
        return 0;
    initData();

    start_time = NOW;
    last_time = start_time;
    return 1;
}

void Simulator::update(){
    // todo maybe put a cap on the delta time to not get too big time steps during lag?
    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;

    slab.prepare();

    velocityStep(delta_time);

    waveletStep(delta_time);

    densityStep(delta_time);

    temperatureStep(delta_time);

    slab.finish();
}

void Simulator::getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth){
    temperatureData = temperature->getDataTexture();
    densityData = density->getDataTexture();
    width = highResSize.x;
    height = highResSize.y;
    depth = highResSize.z;
}

void Simulator::initData() {
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

    float radius = 1;
    float middleW = sizeRatio.x * simulationScale / 2;
    float middleD = sizeRatio.z * simulationScale / 2;
    vec3 start = vec3(middleW - radius, 3 - radius, middleD - radius);
    vec3 end = vec3(middleW + radius, 3 + radius, middleD + radius);

    fillIntensive(density_source, 1.0f, start, end, highResSize);
    fillIntensive(temperature_source, 800.0f, start, end, highResSize);
    //fillOutgoingVector(velocity_source, 1.0f, start, end);

    density = createScalarDataPair(highResSize, density_field);
    createScalar3DTexture(&densitySource, highResSize, density_source);

    temperature = createScalarDataPair(highResSize, temperature_field);
    createScalar3DTexture(&temperatureSource, highResSize, temperature_source);

    lowerVelocity = createVectorDataPair(lowResSize, velocity_field);
    higherVelocity = createVectorDataPair(highResSize, nullptr);
    createVector3DTexture(&velocitySource, lowResSize, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::velocityStep(float dt){
    // Source
    slab.buoyancy(lowerVelocity, temperature, dt, 1.0f);
    slab.addSource(lowerVelocity, velocitySource, dt);
    // Advect
    slab.advection(lowerVelocity, lowerVelocity, dt);
    slab.diffuse(lowerVelocity, 20, 18e-6f, dt);
    //slab.dissipate(velocity, 0.9f, dt);
    // Project
    slab.projection(lowerVelocity, 20);
}

void Simulator::waveletStep(float dt){
    // Advect texture coordinates
    wavelet.advection(lowerVelocity, dt);

    wavelet.calcEnergy(lowerVelocity);

    //wavelet.turbulence();

    wavelet.fluidSynthesis(lowerVelocity, higherVelocity);
}

void Simulator::temperatureStep(float dt) {

    slab.setSource(temperature, temperatureSource, dt);

    slab.temperatureOperation(temperature, higherVelocity, dt);
}

void Simulator::densityStep(float dt){
    // addForce
    slab.setSource(density, densitySource, dt);
    slab.dissipate(density, 0.9f, dt);

    // Advect
    slab.fulladvection(higherVelocity, density, dt);

    // Diffuse
    //slab.diffuse(density, 20, 1.0, dt);
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

void Simulator::fillExtensive(float *field, float density, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                //Lower corner of cell in meters
                vec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in meters
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedArea = getOverlapArea(pos, pos1, minPos, maxPos);

                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = density*overlappedArea;
                }
            }
        }
    }
}

void Simulator::fillIntensive(float *field, float value, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    float cellArea = (1 / simulationScale) * (1 / simulationScale);
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                //Lower corner of cell in meters
                vec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in meters
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedArea = getOverlapArea(pos, pos1, minPos, maxPos);

                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = value*(overlappedArea/cellArea);
                }
            }
        }
    }
}

void Simulator::fillOutgoingVector(vec3 *field, float scale, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    vec3 center = (minPos + maxPos)/2.0f;
    float cellArea = (1 / simulationScale) * (1 / simulationScale);
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                //Lower corner of cell in meters
                vec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in meters
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedArea = getOverlapArea(pos, pos1, minPos, maxPos);

                    vec3 vector = pos - center;
                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = vector*(scale*overlappedArea/cellArea);
                }
            }
        }
    }
}

bool Simulator::hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    return max1.x > min2.x && max1.y > min2.y && max1.z > min2.z
           && min1.x < max2.x && min1.y < max2.y && min1.z < max2.z;
}

float Simulator::getOverlapArea(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    vec3 overlapMin = max(min1, min2);
    vec3 overlapMax = min(max1, max2);
    return (overlapMax.x - overlapMin.x)*(overlapMax.y - overlapMin.y)*(overlapMax.z - overlapMin.z);
}
