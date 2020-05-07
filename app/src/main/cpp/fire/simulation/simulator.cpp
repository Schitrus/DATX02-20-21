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

const ivec3 sizeRatio = ivec3(1, 4, 1);
const int lowResScale = 8;
const int highResScale = lowResScale*8;
const float simulationScale = 24.0f;
// size of low resolution textures. This also includes the border of the texture
const ivec3 lowResSize = lowResScale * sizeRatio + ivec3(2, 2, 2);
// size of high resolution textures. This also includes the border of the texture
const ivec3 highResSize = highResScale * sizeRatio + ivec3(2, 2, 2);
// size of fire.simulation space in meters. This does not include the border that is included in the resolution sizes
const vec3 simulationSize = simulationScale * vec3(sizeRatio);


int Simulator::init(){

    slab = new SlabOperator();
    operations = new SimulationOperations();
    wavelet = new WaveletTurbulence();

    if (!slab->init())
        return 0;

    if(!operations->init(slab))
        return 0;

    if(!wavelet->init(slab))
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

    slab->prepare();

    delta_time = 1/30.0f;
    stopTime += delta_time;

    //if(stopTime<10.0f){

    velocityStep(delta_time);

    waveletStep(delta_time);

    densityStep(delta_time);

    temperatureStep(delta_time);

    slab->finish();
    //}
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

    float radius = 20;
    float middleW = simulationSize.x / 2;
    float middleD = simulationSize.z / 2;
    vec3 start = vec3(middleW - radius, 3 - radius, middleD - radius);
    vec3 end = vec3(middleW + radius, 3 + radius, middleD + radius);



    vec3 center = vec3(0.5f, 0.2f, 0.5f) * vec3(highResSize);

    //fillOutgoingVector(velocity_source, 10.0f, start, end, lowResSize);

    fillSphere(density_source, 0.5f, center, radius, highResSize);
    fillSphere(temperature_source, 3500.0f, center, radius, highResSize);
    //fillSphere(velocity_source, vec3(8.0f, 1.0f, 2.0f), center, 4.0f*radius, lowResSize);

    density = createScalarDataPair(true, density_field);
    createScalar3DTexture(&densitySource, highResSize, density_source);

    temperature = createScalarDataPair(true, temperature_field);
    createScalar3DTexture(&temperatureSource, highResSize, temperature_source);

    lowerVelocity = createVectorDataPair(false, velocity_field);
    higherVelocity = createVectorDataPair(true, nullptr);
    createVector3DTexture(&velocitySource, lowResSize, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::velocityStep(float dt){
    // Source
    operations->buoyancy(lowerVelocity, temperature, dt, 0.15f);

    //diffuse
    //operations->diffuse(lowerVelocity, 20, 0.0000181, dt);

    //updateAndApplyWind(dt);
    // Advect
    operations->advection(lowerVelocity, lowerVelocity, dt);

    operations->vorticity(lowerVelocity, 5.0f, dt);
  
    // Project
    operations->projection(lowerVelocity, 20);
}

void Simulator::waveletStep(float dt){
    // Advect texture coordinates
    wavelet->advection(lowerVelocity, dt);

    wavelet->calcEnergy(lowerVelocity);

    wavelet->calcScattering();

    wavelet->regenerate(lowerVelocity);

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
void Simulator::fillExtensive(float *field, float density, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    for (int z = 1; z < gridSize.z - 1; z++) {
        for (int y = 1; y < gridSize.y - 1; y++) {
            for (int x = 1; x < gridSize.x - 1; x++) {
                //Lower corner of cell in fire.simulation space
                ivec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in fire.simulation space
                ivec3 pos1 = vec3(x + 1, y + 1, z + 1) / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = density*overlappedVolume;
                }
            }
        }
    }
}

void Simulator::fillIntensive(float *field, float value, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    float cellVolume = (1 / simulationScale) * (1 / simulationScale) * (1 / simulationScale);
    for (int z = 1; z < gridSize.z - 1; z++) {
        for (int y = 1; y < gridSize.y - 1; y++) {
            for (int x = 1; x < gridSize.x - 1; x++) {
                //Lower corner of cell in fire.simulation space
                ivec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in fire.simulation space
                ivec3 pos1 = vec3(x + 1, y + 1, z + 1)  / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = value*(overlappedVolume/cellVolume);
                }
            }
        }
    }
}

void Simulator::fillOutgoingVector(vec3 *field, float scale, vec3 minPos, vec3 maxPos, ivec3 gridSize) {
    vec3 center = (minPos + maxPos)/2.0f;
    float cellVolume = (1 / simulationScale) * (1 / simulationScale) * (1 / simulationScale);
    for (int z = 1; z < gridSize.z - 1; z++) {
        for (int y = 1; y < gridSize.y - 1; y++) {
            for (int x = 1; x < gridSize.x - 1; x++) {
                //Lower corner of cell in fire.simulation space
                vec3 pos = vec3(x, y, z) / simulationScale;
                //Upper corner of cell in fire.simulation space
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) / simulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    vec3 vector = pos - center;
                    int index = gridSize.x * (gridSize.y * z + y) + x;
                    field[index] = vector*(scale*overlappedVolume/cellVolume);
                }
            }
        }
    }
}


void Simulator::fillSphere(float* field, float value, vec3 center, float radius, vec3 size){
    for(int z = 0; z < size.z; z++){
        for(int y = 0; y < size.y; y++){
            for(int x = 0; x < size.x; x++){
                int index = size.x * (size.y * z + y) + x;
                vec3 pos = vec3(x,y,z);
                if(distance(pos, center) <= radius)
                    field[index] = value;
            }
        }
    }
}

void Simulator::fillSphere(vec3* field, vec3 value, vec3 center, float radius, vec3 size){
    for(int z = 0; z < size.z; z++){
        for(int y = 0; y < size.y; y++){
            for(int x = 0; x < size.x; x++){
                int index = size.x * (size.y * z + y) + x;
                vec3 pos = vec3(x,y,z);
                if(distance(pos, center) <= radius)
                    field[index] = value;
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