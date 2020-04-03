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
    initSize(12, 48, 12, 12.0f);
    if (!slab.init())
        return 0;
    initData();

    start_time = NOW;
    last_time = start_time;
    return 1;
}

void Simulator::initSize(int width, int height, int depth, float simulationWidth) {
    grid_width = width + 2;
    grid_height = height + 2;
    grid_depth = depth + 2;
    meter_to_pixels = width/simulationWidth;
    slab.initSize(width, height, depth, meter_to_pixels);
}

void Simulator::update(){
    // todo maybe put a cap on the delta time to not get too big time steps during lag?
    float current_time = DURATION(NOW, start_time);
    float delta_time = DURATION(NOW, last_time);
    last_time = NOW;

    slab.prepare();

    velocityStep(delta_time);

    densityStep(delta_time);

    temperatureStep(delta_time);

    slab.finish();
}

void Simulator::getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth){
    temperatureData = temperature->getDataTexture();
    densityData = density->getDataTexture();
    width = grid_width;
    height = grid_height;
    depth = grid_depth;
}

void Simulator::initData() {
    int size = grid_width * grid_height * grid_depth;

    float* density_field = new float[size];
    float* density_source = new float[size];
    float* temperature_field = new float[size];
    float* temperature_source = new float[size];
    vec3* velocity_field = new vec3[size];
    vec3* velocity_source = new vec3[size];

    clearField(density_field, 0.0f);
    clearField(density_source, 0.0f);
    clearField(temperature_field, 0.0f);
    clearField(temperature_source, 0.0f);
    clearField(velocity_field, vec3(0.0f, 0.0f, 0.0f));
    clearField(velocity_source, vec3(0.0f, 0.0f, 0.0f));

    float radius = 1;
    float middleW = grid_width/meter_to_pixels/2;
    float middleD = grid_depth/meter_to_pixels/2;
    vec3 start = vec3(middleW - radius, 3 - radius, middleD - radius);
    vec3 end = vec3(middleW + radius, 3 + radius, middleD + radius);

    fillIntensive(density_source, 1.0f, start, end);
    fillIntensive(temperature_source, 800.0f, start, end);

    density = createScalarDataPair(grid_width, grid_height, grid_depth, density_field);
    createScalar3DTexture(&densitySource, grid_width, grid_height, grid_depth, density_source);

    temperature = createScalarDataPair(grid_width, grid_height, grid_depth, temperature_field);
    createScalar3DTexture(&temperatureSource, grid_width, grid_height, grid_depth, temperature_source);

    velocity = createVectorDataPair(grid_width, grid_height, grid_depth, velocity_field);
    createVector3DTexture(&velocitySource, grid_width, grid_height, grid_depth, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::velocityStep(float dt){
    // Source
    slab.buoyancy(velocity, temperature, dt, 1.0f);
    slab.addSource(velocity, velocitySource, dt);
    // Advect
    slab.advection(velocity, velocity, dt);
    //slab.diffuse(velocity, 20, 1.0, dt);
    slab.dissipate(velocity, 0.9f, dt);
    // Project
    slab.projection(velocity);
}

void Simulator::temperatureStep(float dt) {

    slab.setSource(temperature, temperatureSource, dt);

    slab.temperatureOperation(temperature, velocity, dt);
}

void Simulator::densityStep(float dt){
    // addForce
    slab.setSource(density, densitySource, dt);
    slab.dissipate(density, 0.9f, dt);

    // Advect
    slab.fulladvection(velocity, density, dt);

    // Diffuse
    //slab.diffuse(density, 20, 1.0, dt);
}

void Simulator::substanceMovementStep(DataTexturePair *data, float dissipationRate, float dt) {
    //todo use this for temperature and density when advection has been seperated from the heat dissipation part in the temperature shader
}

void Simulator::clearField(float* field, float value) {
    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                field[index] = value;
            }
        }
    }
}

void Simulator::clearField(vec3* field, vec3 value) {
    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                field[index] = value;
            }
        }
    }
}

void Simulator::fillExtensive(float *field, float density, vec3 minPos, vec3 maxPos) {
    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                //Lower corner of cell in meters
                vec3 pos = vec3(x, y, z)/meter_to_pixels;
                //Upper corner of cell in meters
                vec3 pos1 = vec3(x + 1, y + 1, z + 1)/meter_to_pixels;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedArea = getOverlapArea(pos, pos1, minPos, maxPos);

                    int index = grid_width * (grid_height * z + y) + x;
                    field[index] = density*overlappedArea;
                }
            }
        }
    }
}

void Simulator::fillIntensive(float *field, float value, vec3 minPos, vec3 maxPos) {
    float cellArea = (1 / meter_to_pixels) * (1 / meter_to_pixels);
    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                //Lower corner of cell in meters
                vec3 pos = vec3(x, y, z)/meter_to_pixels;
                //Upper corner of cell in meters
                vec3 pos1 = vec3(x + 1, y + 1, z + 1)/meter_to_pixels;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedArea = getOverlapArea(pos, pos1, minPos, maxPos);

                    int index = grid_width * (grid_height * z + y) + x;
                    field[index] = value*(overlappedArea/cellArea);
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