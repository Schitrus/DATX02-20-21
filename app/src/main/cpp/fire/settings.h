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
public:
    Settings();

    std::string getName();
    Settings withName(std::string name) const;

    int getProjectionIterations();
    Settings withProjectIterations(int projectionIterations) const;

    float getVorticityScale();
    Settings withVorticityScale(float vorticityScale) const;

    ivec3 getSize(Resolution res);

    // Returns a scale factor that converts a unit from voxels to meters.
    // Note that this will not be a perfect scaling factor since resolutions normally use 1-voxel borders, unlike the simulation unit.
    float getResToSimFactor(Resolution res);

    vec3 getSimulationSize();

    Settings withSize(ivec3 sizeRatio, int velocityScale, int substanceScale, float simulationScale) const;

private:
    Settings(const Settings* other);

};

Settings nextSettings();

#endif //DATX02_20_21_SETTINGS_H
