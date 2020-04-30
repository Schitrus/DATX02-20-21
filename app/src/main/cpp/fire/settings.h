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
    float buoyancyScale;
    float windScale;

    float smokeKinematicViscosity;
    int smokeDiffusionIterations;
    float smokeDissipation;
    float tempKinematicViscosity;
    int tempDiffusionIterations;
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

    float getBuoyancyScale();
    Settings withBuoyancyScale(float buoyancyScale) const;

    float getWindScale();
    Settings withWindScale(float windScale) const;

    float getSmokeKinematicViscosity();
    int getSmokeDiffusionIterations();
    Settings withSmokeDiffusion(float viscosity, int iterations);

    float getSmokeDissipation();
    Settings withSmokeDissipation(float smokeDissipation) const;

    float getTempKinematicViscosity();
    int getTempDiffusionIterations();
    Settings withTempDiffusion(float viscosity, int iterations);


private:
    Settings(const Settings* other);

};

Settings nextSettings();

#endif //DATX02_20_21_SETTINGS_H
