//
// Created by kirderf on 2020-04-14.
//

#include <glm/gtx/string_cast.hpp>
#include "simulation_operations.h"
#include "fire/util/helper.h"

#include <android/log.h>

#define LOG_TAG "Simulation operations"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int SimulationOperations::init(SlabOperation* slab, Settings* settings) {
    this->slab = slab;
    
    initTextures(settings);

    if(!initShaders()) {
        LOG_ERROR("Failed to compile simulation_operations shaders");
        return 0;
    }

    LOG_INFO("Finished initializing simulation operations");

    return 1;
}

int SimulationOperations::initShaders() {
    bool success = true;
    // Advection Shaders
    success &= advectionShader.load("shaders/simulation/slab.vert", "shaders/simulation/advection/advection.frag");
    // Dissipate Shaders
    success &= dissipateShader.load("shaders/simulation/slab.vert", "shaders/simulation/dissipate/dissipate.frag");
    // Force Shaders
    success &= addSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_source.frag");
    success &= setSourceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/set_source.frag");
    success &= buoyancyShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/buoyancy.frag");
    success &= windShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/add_wind.frag");
    success &= externalForceShader.load("shaders/simulation/slab.vert", "shaders/simulation/force/external_force.frag");
    // Projection Shaders
    success &= divergenceShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/divergence.frag");
    success &= jacobiShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/jacobi.frag");
    success &= gradientShader.load("shaders/simulation/slab.vert", "shaders/simulation/projection/gradient_subtraction.frag");
    // Vorticity Shaders
    success &= vorticityShader.load("shaders/simulation/slab.vert", "shaders/simulation/vorticity/vorticity.frag");
    // Temperature Shaders
    success &= temperatureShader.load("shaders/simulation/slab.vert", "shaders/simulation/temperature/temperature.frag");
    return success;
}

void SimulationOperations::initTextures(Settings* settings) {

    ivec3 lowResSize = settings->getSize(Resolution::velocity);
    ivec3 highResSize = settings->getSize(Resolution::substance);

    float lowScaleFactor = 1.0f/settings->getResToSimFactor(Resolution::velocity);
    float highScaleFactor = 1.0f/settings->getResToSimFactor(Resolution::substance);

    createVector3DTexture(diffusionBHRTexture, highResSize, (vec3*)nullptr);
    createVector3DTexture(diffusionBLRTexture, lowResSize, (vec3*)nullptr);

    divergence = createScalarDataPair(nullptr, lowResSize, lowScaleFactor);

    jacobi = createScalarDataPair(nullptr, lowResSize, lowScaleFactor);

}

void SimulationOperations::clearTextures() {
    delete divergence;
    delete jacobi;
    glDeleteTextures(1, &diffusionBHRTexture);
    glDeleteTextures(1, &diffusionBLRTexture);
}

int SimulationOperations::changeSettings(Settings* settings, bool shouldRegenFields) {
    if(shouldRegenFields) {
        //clearTextures();
        initTextures(settings);
    }
    return 1;
}

void SimulationOperations::heatDissipation(DataTexturePair* temperature, float dt){
    temperatureShader.use();
    temperatureShader.uniform1f("dt", dt);
    temperature->bindData(GL_TEXTURE0);

    slab->fullOperation(temperatureShader, temperature);
}

void SimulationOperations::addSource(DataTexturePair* data, GLuint source, SourceMode mode, float dt) {
    Shader shader = mode == SourceMode::add ? addSourceShader : setSourceShader;

    shader.use();
    shader.uniform1f("dt", dt);
    data->bindData(GL_TEXTURE0);
    bindData(source, GL_TEXTURE1);

    slab->fullOperation(shader, data);
}

void SimulationOperations::buoyancy(DataTexturePair* velocity, DataTexturePair* temperature, vec3 direction, float scale, float dt){
    buoyancyShader.use();
    buoyancyShader.uniform1f("dt", dt);
    buoyancyShader.uniform1f("scale", scale);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
    // Apply rotation to direction vector, pointing upwards
    buoyancyShader.uniform3f("direction", direction);
    buoyancyShader.uniform3f("temp_border_width", vec3(1)/vec3(temperature->getSize()));
#pragma clang diagnostic pop
    buoyancyShader.uniform3f("gridSize", velocity->getSize());

    temperature->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    slab->interiorOperation(buoyancyShader, velocity, -1);
}

void SimulationOperations::diffuse(DataTexturePair* data, Resolution res, int iterationCount, float kinematicViscosity, float dt) {

    GLuint diffusionTexture = res == Resolution::velocity ? diffusionBLRTexture : diffusionBHRTexture;
    slab->copy(data, diffusionTexture);

    float dx = 1.0f / data->toVoxelScaleFactor();
    float alpha = (dx*dx) / (kinematicViscosity * dt);
    float beta = 6.0f + alpha; // For 3D grids

    jacobiIteration(data, diffusionTexture, iterationCount, alpha, beta, -1);
}

void SimulationOperations::dissipate(DataTexturePair* data, float dissipationRate, float dt){

    dissipateShader.use();
    dissipateShader.uniform1f("dt", dt);
    dissipateShader.uniform1f("dissipation_rate", dissipationRate);
    data->bindData(GL_TEXTURE0);

    slab->fullOperation(dissipateShader, data);
}

void SimulationOperations::advect(DataTexturePair* velocity, DataTexturePair* data, bool applyVelocityBorder, float dt) {
    advectionShader.use();
    advectionShader.uniform1f("dt", dt);
    advectionShader.uniform1f("meterToVoxels", velocity->toVoxelScaleFactor());
    advectionShader.uniform3f("gridSize", velocity->getSize());
    velocity->bindData(GL_TEXTURE0);
    data->bindData(GL_TEXTURE1);

    if(applyVelocityBorder)
        slab->interiorOperation(advectionShader, data, -1);
    else slab->fullOperation(advectionShader, data);
}

void SimulationOperations::project(DataTexturePair* velocity, int iterationCount){
    float dx = 1.0f/velocity->toVoxelScaleFactor();
    float alpha = -(dx*dx);
    float beta = 6.0f;

    int zSize = velocity->getSize().z;
    // Clear gradient texture, unsure if needed?
    for(int depth = 0; depth < zSize; depth++){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, jacobi->getDataTexture(), 0, depth);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    createDivergence(velocity, dx);
    jacobiIteration(jacobi, divergence->getDataTexture(), iterationCount, alpha, beta, 1);
    subtractGradient(velocity, dx);
}

void SimulationOperations::createVorticity(DataTexturePair *velocity, float vorticityScale, float dt) {
    vorticityShader.use();
    vorticityShader.uniform1f("dt", dt);
    vorticityShader.uniform1f("vorticityScale", vorticityScale);
    velocity->bindData(GL_TEXTURE0);

    slab->interiorOperation(vorticityShader, velocity, -1);
}

void SimulationOperations::createDivergence(DataTexturePair* vectorData, float dx) {
    divergenceShader.use();
    divergenceShader.uniform1f("dh", dx);
    vectorData->bindData(GL_TEXTURE0);

    slab->interiorOperation(divergenceShader, divergence, 1);
}

void SimulationOperations::jacobiIteration(DataTexturePair *xTexturePair, GLuint bTexture,
                                   int iterationCount, float alpha, float beta, int scale){

    bindData(bTexture, GL_TEXTURE1);
    for(int i = 0; i < iterationCount; i++){
        jacobiShader.use();
        jacobiShader.uniform1f("alpha", alpha);
        jacobiShader.uniform1f("beta", beta);
        xTexturePair->bindData(GL_TEXTURE0);

        slab->interiorOperation(jacobiShader, xTexturePair, scale);
    }
}

void SimulationOperations::subtractGradient(DataTexturePair* velocity, float dx){
    gradientShader.use();
    gradientShader.uniform1f("dh", dx);
    jacobi->bindData(GL_TEXTURE0);
    velocity->bindData(GL_TEXTURE1);

    slab->interiorOperation(gradientShader, velocity, -1);
}

void SimulationOperations::addWind(DataTexturePair* velocity, float wind_angle, float wind_strength, float dt) {
    windShader.use();
    windShader.uniform1f("dt", dt);
    windShader.uniform1f("wind_angle", wind_angle);
    windShader.uniform1f("wind_strength", wind_strength);
    velocity->bindData(GL_TEXTURE0);

    slab->interiorOperation(windShader, velocity, -1);
}


void SimulationOperations::externalForce(DataTexturePair *velocity, GLuint &force, float dt) {
    externalForceShader.use();
    externalForceShader.uniform1f("dt", dt);
    velocity->bindData(GL_TEXTURE0);
    bindData(force, GL_TEXTURE1);

    slab->fullOperation(externalForceShader, velocity);
}