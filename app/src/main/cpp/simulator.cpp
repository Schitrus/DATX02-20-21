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
    resize(vec3(12, 48, 12), vec3(60, 240, 60));

    if (!slab.init(lowerResolution))
        return 0;

    GLuint VAO = slab.getVAO();

    if(!wavelet.init(lowerResolution, higherResolution, VAO))
        return 0;

    initData();

    start_time = NOW;
    last_time = start_time;
    return 1;
}

void Simulator::resize(vec3 lowerResolution, vec3 higherResolution) {
    this->lowerResolution = lowerResolution;
    this->higherResolution = higherResolution;
    slab.resize(lowerResolution);
    //wavelet.resize(lowerResolution, higherResolution);
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
    width = higherResolution.x;
    height = higherResolution.y;
    depth = higherResolution.z;
}

void Simulator::initData() {
    int lowerSize = lowerResolution.x * lowerResolution.y * lowerResolution.z;
    int higherSize = higherResolution.x * higherResolution.y * higherResolution.z;

    float* density_field = new float[higherSize];
    float* density_source = new float[higherSize];
    float* temperature_field = new float[higherSize];
    float* temperature_source = new float[higherSize];
    vec3* velocity_field = new vec3[lowerSize];
    vec3* velocity_source = new vec3[lowerSize];

    vec3 center = vec3(higherResolution.x/2, higherResolution.y/6, higherResolution.z/2);

    for (int z = 0; z < higherResolution.z; z++) {
        for (int y = 0; y < higherResolution.y; y++) {
            for (int x = 0; x < higherResolution.x; x++) {
                int index = higherResolution.x * (higherResolution.y * z + y) + x;
                density_field[index]      = 0.0f;
                density_source[index]     = 0.0f;
                temperature_field[index]  = 0.0f;
                temperature_source[index] = 0.0f;

                if(distance(vec3(x,y,z), center) <= 12.0){
                    density_source[index] = 1.0f;
                    temperature_source[index] = 800.0f;
                }

            }
        }
    }

    for (int z = 0; z < lowerResolution.z; z++) {
        for (int y = 0; y < lowerResolution.y; y++) {
            for (int x = 0; x < lowerResolution.x; x++) {
                int index = lowerResolution.x * (lowerResolution.y * z + y) + x;
                velocity_field[index]     = vec3(0, 0, 0);
                velocity_source[index]    = vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }


    density = createScalarDataPair(higherResolution.x, higherResolution.y, higherResolution.z, density_field);
    createScalar3DTexture(&densitySource, higherResolution, density_source);

    temperature = createScalarDataPair(higherResolution.x, higherResolution.y, higherResolution.z, temperature_field);
    createScalar3DTexture(&temperatureSource, higherResolution, temperature_source);

    lowerVelocity = createVectorDataPair(lowerResolution.x, lowerResolution.y, lowerResolution.z, velocity_field);
    higherVelocity = createVectorDataPair(higherResolution.x, higherResolution.y, higherResolution.z, nullptr);
    createVector3DTexture(&velocitySource, lowerResolution, velocity_source);

    delete[] density_field, delete[] density_source, delete[] temperature_field, delete[] temperature_source, delete[] velocity_field, delete[] velocity_source;
}

void Simulator::velocityStep(float dt){
    // Source
    slab.buoyancy(lowerVelocity, temperature, lowerResolution, dt, 1.0f);
    slab.addSource(lowerVelocity, lowerResolution, velocitySource, dt);
    // Advect
    slab.advection(lowerVelocity, lowerVelocity, lowerResolution, lowerResolution, dt);
    //slab.diffuse(velocity, 20, 1.0, dt);
    slab.dissipate(lowerVelocity, lowerResolution, 0.9f, dt);
    // Project
    slab.projection(lowerVelocity, lowerResolution);
}

void Simulator::waveletStep(float dt){
    // Advect texture coordinates
    wavelet.advection(lowerVelocity, dt);

    wavelet.calcEnergy(lowerVelocity);

    wavelet.fluidSynthesis(lowerVelocity, higherVelocity);
}

void Simulator::temperatureStep(float dt) {

    slab.setSource(temperature, higherResolution, temperatureSource, dt);

    slab.temperatureOperation(temperature, higherVelocity, higherResolution, higherResolution, dt);
}

void Simulator::densityStep(float dt){
    // addForce
    slab.setSource(density, higherResolution, densitySource, dt);
    slab.dissipate(density, higherResolution, 0.9f, dt);

    // Advect
    slab.fulladvection(higherVelocity, higherResolution, higherResolution, density, dt);

    // Diffuse
    //slab.diffuse(density, 20, 1.0, dt);
}

void Simulator::substanceMovementStep(DataTexturePair *data, float dissipationRate, float dt) {
    //todo use this for temperature and density when advection has been seperated from the heat dissipation part in the temperature shader
}