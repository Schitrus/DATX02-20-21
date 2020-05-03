//
// Created by kirderf on 2020-04-23.
//

#include <string>

#include <glm/glm.hpp>

using namespace glm;

#ifndef DATX02_20_21_SETTINGS_H
#define DATX02_20_21_SETTINGS_H

// The different types of resolutions that are being used in the simulation
enum class Resolution {velocity, substance};

// The different ways to add a source field to its respective field
// set forces the value as long as the source value != 0
// add adds on the source value multiplied by dt
enum class SourceMode {set, add};

// The type of source to place
// singleSphere places a single force in the center of the simulation
enum class SourceType {singleSphere, dualSpheres};

class Settings {
    std::string name;

    ivec3 velocityResSize, substanceResSize;
    float velocityToSimFactor, substanceToSimFactor;
    vec3 simulationSize;

    SourceMode sourceMode;
    SourceType sourceType;
    float temperatureSourceDensity;
    float smokeSourceDensity;

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

    SourceMode getSourceMode();
    Settings withSourceMode(SourceMode mode) const;

    SourceType getSourceType();
    Settings withSourceType(SourceType type) const;

    float getTempSourceDensity();
    Settings withTempSourceDensity(float density) const;

    float getSmokeSourceDensity();
    Settings withSmokeSourceDensity(float density) const;

    float getVelKinematicViscosity();
    int getVelDiffusionIterations();
    Settings withVelDiffusion(float viscosity, int iterations) const;

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
    Settings withSmokeDiffusion(float viscosity, int iterations) const;

    float getSmokeDissipation();
    Settings withSmokeDissipation(float smokeDissipation) const;

    float getTempKinematicViscosity();
    int getTempDiffusionIterations();
    Settings withTempDiffusion(float viscosity, int iterations) const;


private:
    Settings(const Settings* other);

};

Settings nextSettings();

#endif //DATX02_20_21_SETTINGS_H
