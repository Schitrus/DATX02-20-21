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
    initSize(20, 90, 20);
    if (!slab.init())
        return 0;
    initData();

    start_time = NOW;
    last_time = start_time;
    return 1;
}

void Simulator::initSize(int width, int height, int depth) {
    grid_width = width + 2;
    grid_height = height + 2;
    grid_depth = depth + 2;
    slab.initSize(width, height, depth);
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

    for (int z = 0; z < grid_depth; z++) {
        for (int y = 0; y < grid_height; y++) {
            for (int x = 0; x < grid_width; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                density_field[index]      = 0.0f;
                density_source[index]     = 0.0f;
                temperature_field[index]  = 0.0f;
                temperature_source[index] = 0.0f;
                velocity_field[index]     = vec3(0, 0, 0);
                velocity_source[index]    = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    for (int z = 1; z < grid_depth - 1; z++) {
        for (int y = 1; y < grid_height - 1; y++) {
            for (int x = 1; x < grid_width - 1; x++) {
                int index = grid_width * (grid_height * z + y) + x;
                velocity_source[index] = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    int z = grid_depth/2-2, y = 5, x = grid_width/2-2;

    //int z = 0, y = 2, x = 0;

    for(int zz = z; zz < z + 7; zz++) {
        for (int yy = y; yy < y + 40; yy++) {
            for (int xx = x; xx < x + 7; xx++) {
                int index = grid_width * (grid_height * (zz) + (yy)) + (xx);
                density_source[index] = 1.0f;
                temperature_source[index] = 800.0f;
                velocity_source[index] = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

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
    slab.addSource(velocity, velocitySource, dt);
    // External force
    slab.buoyancy(velocity, temperature, dt, 1.0f);
    slab.vorticity(velocity, 10.0f, dt);

    //slab.diffuse(velocity, 20, 18e-6f, dt);

    // Advect
    slab.advection(velocity, velocity, dt);


    //slab.dissipate(velocity, 0.9f, dt);
    // Project
    slab.projection(velocity, 20);
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