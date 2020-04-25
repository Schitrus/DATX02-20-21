//
// Created by kirderf on 2020-04-23.
//

#include <string>

#include <glm/glm.hpp>

using namespace glm;

#ifndef DATX02_20_21_SETTINGS_H
#define DATX02_20_21_SETTINGS_H

enum class Resolution {velocity, substance};

class Settings {
    std::string name;

    ivec3 velocityResSize, substanceResSize;
    float velocityToSimFactor, substanceToSimFactor;
    vec3 simulationSize;

    int projectionIterations;
    float vorticityScale;
    float velocityKinematicViscosity;
    int velocityDiffusionIterations;
public:
    Settings();

    // todo it'd be nice to display the name of the current settings
    std::string getName();
    Settings withName(std::string name) const;

    ivec3 getSize(Resolution res);

    // Returns a scale factor that converts a unit from voxels to meters.
    // Note that this will not be a perfect scaling factor since resolutions normally use 1-voxel borders, unlike the simulation unit.
    float getResToSimFactor(Resolution res);

    vec3 getSimulationSize();

    Settings withSize(ivec3 sizeRatio, int velocityScale, int substanceScale, float simulationScale) const;

    float getVelKinematicViscosity();
    int getVelDiffusionIterations();
    Settings withVelDiffusion(float viscosity, int iterations);

    float getVorticityScale();
    Settings withVorticityScale(float vorticityScale) const;

    int getProjectionIterations();
    Settings withProjectIterations(int projectionIterations) const;

private:
    Settings(const Settings* other);

};

Settings nextSettings();

#endif //DATX02_20_21_SETTINGS_H
