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
// dualSpheres places two spheres located besides each other
enum class SourceType {singleSphere, dualSpheres, floor};

class Settings {
    std::string name;

    ivec3 velocityResSize, substanceResSize;

    float velocityScale, substanceScale;

    float resScale, simulationScale;
    float velocityToSimFactor, substanceToSimFactor;
    vec3 simulationSize;
    float dt;

    ivec3 sizeRatio;

    vec3 backgroundColor, filterColor;
    vec3 colorSpace;

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
    // Returns the name of this settings template
    std::string getName();
    // Sets the name of this settings template
    Settings* withName(std::string name);

    // Returns the texture size of a specific resolution (border included)
    ivec3 getSize(Resolution res);
    // Returns a scale factor that converts a unit from voxels to meters.
    // Note that this will not be a perfect scaling factor since resolutions normally use 1-voxel borders, unlike the simulation unit.
    float getResToSimFactor(Resolution res);
    // Returns the size of the simulation in resolution-independent units (without border)
    vec3 getSimulationSize();
    // Returns the simulation scale
    float getSimulationScale();
    // Returns the size Ratio
    ivec3 getSizeRatio();
    // Get ResolutionScale
    float getResScale();

    float getVelocityScale();

    float getSubstanceScale();

    // Sets the size of the simulation and resolutions
    Settings* withSize(ivec3 sizeRatio, float velocityScale, float substanceScale, float simulationScale);

    // Returns the fixed delta time, or 0 if the delta time shouldn't be fixed
    float getDeltaTime();

    // Sets the fixed delta time of the simulation
    // If the delta time is 0, real time will be used as delta time
    Settings* withDeltaTime(float dt);

    // Returns the mode that determines how sources are applied to fields
    SourceMode getSourceMode();
    // Sets the source mode for how sources are applied to fields
    // See comment on SourceMode for details on what the modes do
    Settings* withSourceMode(SourceMode mode);

    // Returns the type of source setup
    SourceType getSourceType();
    // Sets the type of source setup
    // See comment on SourceType for details on what the different setups are
    Settings* withSourceType(SourceType type);

    // Returns the value used for the temperature source
    float getTempSourceDensity();
    // Sets the value used for the temperature source
    Settings* withTempSourceDensity(float density);

    // Returns the value used for the smoke/density source
    float getSmokeSourceDensity();
    // Sets the value used for the smoke/density source
    Settings* withSmokeSourceDensity(float density);

    // Returns the kinematic viscosity used by diffusion of the velocity
    float getVelKinematicViscosity();
    // Returns the number of iterations used by diffusion of the velocity
    int getVelDiffusionIterations();
    // Sets the parameters related to diffusion of the velocity
    // If the viscosity is set to 0, the diffusion step step will be skipped for velocity
    Settings* withVelDiffusion(float viscosity, int iterations);

    // Returns the scale factor used by vorticity
    float getVorticityScale();
    // Sets the scale factor used by vorticity
    // If the scale factor is 0, the vorticity step will be skipped
    Settings* withVorticityScale(float vorticityScale);

    // Returns the number of iterations used in jacobi iteration during projection
    int getProjectionIterations();
    // Sets the number of iterations used in jacobi iteration during projection
    // If the number of iterations is 0, the projection step will be skipped
    Settings* withProjectIterations(int projectionIterations);

    // Returns the scale factor for buoyancy
    float getBuoyancyScale();
    // Sets the scale factor for buoyancy
    // If the buoyancy scale is set to 0, the buoyancy step will be skipped
    Settings* withBuoyancyScale(float buoyancyScale);

    // Returns a scale factor for wind strength
    float getWindScale();

    // Sets the scale factor for wind strength
    // If the scale factor is set to 0, the wind step will be skipped
    Settings* withWindScale(float windScale);

    // Returns the kinematic viscosity used by diffusion of the smoke/density
    float getSmokeKinematicViscosity();
    // Returns the number of iterations used by diffusion of the smoke/density
    int getSmokeDiffusionIterations();
    // Sets the parameters related to diffusion of the smoke/density
    // If the viscosity is set to 0, the diffusion step step will be skipped for smoke/density
    Settings* withSmokeDiffusion(float viscosity, int iterations);

    // Returns the dissipation rate for smoke/density
    float getSmokeDissipation();
    // Sets the dissipation rate for smoke/density
    // If the dissipation rate is set to 0, the dissipation step will be skipped for smoke/density
    Settings* withSmokeDissipation(float smokeDissipation);

    // Returns the kinematic viscosity used by diffusion of the temperature
    float getTempKinematicViscosity();
    // Returns the number of iterations used by diffusion of the temperature
    int getTempDiffusionIterations();
    // Sets the parameters related to diffusion of the temperature
    // If the viscosity is set to 0, the diffusion step step will be skipped for temperature
    Settings* withTempDiffusion(float viscosity, int iterations);

    vec3 getBackgroundColor();
    Settings* withBackgroundColor(vec3 backgroundColor);

    vec3 getFilterColor();
    Settings* withFilterColor(vec3 filterColor);

    vec3 getColorSpace();
    Settings* withColorSpace(vec3 colorSpace);

};

#endif //DATX02_20_21_SETTINGS_H
