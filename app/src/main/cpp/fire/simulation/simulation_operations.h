//
// Created by kirderf on 2020-04-14.
//

#ifndef DATX02_20_21_SIMULATION_OPERATIONS_H
#define DATX02_20_21_SIMULATION_OPERATIONS_H

#include <GLES3/gl31.h>
#include <fire/settings.h>

#include "slab_operation.h"
#include "fire/util/data_texture_pair.h"
#include "fire/util/shader.h"

class SimulationOperations {
    SlabOperation slab;

    GLuint diffusionBLRTexture, diffusionBHRTexture;
    DataTexturePair* divergence;
    DataTexturePair* jacobi;

    Shader temperatureShader;
    Shader divergenceShader, jacobiShader, gradientShader;
    Shader addSourceShader, buoyancyShader, advectionShader;
    Shader dissipateShader, setSourceShader, windShader;
    Shader vorticityShader;

public:
    int init(SlabOperation slab, Settings* settings);

    int changeSettings(Settings* settings, bool shouldRegenFields);

    // Applies buoyancy forces to velocity, based on the temperature
    void buoyancy(DataTexturePair* velocity, DataTexturePair* temperature, float scale, float dt);

    // Performs advection on the given data
    // The data and the velocity should use the same resolution for the shader to work correctly
    void advect(DataTexturePair* velocity, DataTexturePair* data, bool applyVelocityBorder, float dt);

    // Performs heat dissipation on the given temperature field
    void heatDissipation(DataTexturePair* temperature, float dt);

    // Adds the given source field multiplied by dt to the target field
    void addSource(DataTexturePair* data, GLuint source, SourceMode mode, float dt);

    void dissipate(DataTexturePair* data, float dissipationRate, float dt);

    // Performs diffusion on a texture with given resolution
    void diffuse(DataTexturePair* data, Resolution res, int iterationCount, float kinematicViscosity, float dt);

    // Projects the given *vector* field
    void project(DataTexturePair* velocity, int iterationCount);

    // Apply rotational flows
    void createVorticity(DataTexturePair* velocity, float vorticityScale, float dt);

    void addWind(DataTexturePair* velocity, float wind_angle, float wind_strength, float dt);

private:

    int initShaders();

    void initTextures(Settings* settings);

    void clearTextures();

    // Performs a number of jacobi iterations with two field inputs
    void jacobiIteration(DataTexturePair *xTexturePair, GLuint bTexture,
                         int iterationCount, float alpha, float beta, int scale );

    // Calculates the divergence of the vector field
    void createDivergence(DataTexturePair* vectorData, float dx);

    // Subtracts the gradient of the given scalar field from the target vector field
    void subtractGradient(DataTexturePair* velocity, float dx);

};


#endif //DATX02_20_21_SIMULATION_OPERATIONS_H
