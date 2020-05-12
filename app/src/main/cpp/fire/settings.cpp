//
// Created by kirderf on 2020-04-23.
//

#include <string>

#include "settings.h"
#include "fire/simulation/simulator.h"

const Settings DEFAULT = Settings().withSize(ivec3(1, 4, 1), 12, 60, 24.0f).withDeltaTime(1/30.0f)
        .withSourceMode(SourceMode::set).withTempSourceDensity(3500.0f).withSmokeSourceDensity(0.4f)
        .withVorticityScale(8.0f).withProjectIterations(20).withBuoyancyScale(0.15f).withName("Default");
const Settings FEW_ITERATIONS = DEFAULT.withProjectIterations(5).withName("Few Iterations");
const Settings SMALL_RES = DEFAULT.withSize(ivec3(1, 4, 1), 6, 24, 24.0f).withName("Small Resolution");
const Settings LIGHTWEIGHT = DEFAULT.withProjectIterations(5).withSize(ivec3(1, 3, 1), 8, 30, 24.0f).withName("Lightweight");
const Settings DIFFUSION = DEFAULT.withSmokeDiffusion(1.0f, 10).withTempDiffusion(1.0f, 10).withName("Little bit of Diffusion");
const Settings MORE_DIFFUSION = DIFFUSION.withVelDiffusion(5.0f, 20).withName("More diffusion");
const Settings WIND = DEFAULT.withWindScale(2.0f).withName("Wind");
const Settings STORM = DEFAULT.withWindScale(10.0f).withSize(ivec3(1, 1, 1), 18, 90, 48.0f).withSmokeSourceDensity(0.8f).withSmokeDissipation(0.1f).withBuoyancyScale(0.3f).withName("Storm");
const Settings SOURCE_MODE_ADD = LIGHTWEIGHT.withSourceMode(SourceMode::add).withTempSourceDensity(5000.0f).withSmokeSourceDensity(1.0f).withName("Source mode: add");
const Settings FLOOR_IS_FIRE = DEFAULT.withSourceType(SourceType::floor).withName("Floor is Fire");

int index = -1;

Settings nextSettings() {
    index++;
    if(index >= 10)
        index = 0;
    switch(index) {
        case 1: return FEW_ITERATIONS;
        case 2: return SMALL_RES;
        case 3: return LIGHTWEIGHT;
        case 4: return DIFFUSION;
        case 5: return MORE_DIFFUSION;
        case 6: return WIND;
        case 7: return STORM;
        case 8: return SOURCE_MODE_ADD;
        case 9: return FLOOR_IS_FIRE;
        default: return DEFAULT;
    }
}

Settings::Settings() {
    dt = 0.0f;

    sourceMode = SourceMode::add;
    sourceType = SourceType::singleSphere;
    temperatureSourceDensity = 0.0f;
    smokeSourceDensity = 0.0f;

    projectionIterations = 0;
    vorticityScale = 0.0f;
    velocityKinematicViscosity = 0.0f;
    velocityDiffusionIterations = 0;
    buoyancyScale = 0.0f;
    windScale = 0.0f;

    smokeKinematicViscosity = 0.0f;
    smokeDiffusionIterations = 0;
    smokeDissipation = 0.0f;
    tempKinematicViscosity = 0.0f;
    tempDiffusionIterations = 0;
}

Settings::Settings(const Settings* other) {
    this->name = other->name;

    velocityResSize = other->velocityResSize;
    substanceResSize = other->substanceResSize;
    velocityToSimFactor = other->velocityToSimFactor;
    substanceToSimFactor = other->substanceToSimFactor;
    simulationSize = other->simulationSize;
    dt = other->dt;

    sourceMode = other->sourceMode;
    sourceType = other->sourceType;
    temperatureSourceDensity = other->temperatureSourceDensity;
    smokeSourceDensity = other->smokeSourceDensity;

    projectionIterations = other->projectionIterations;
    vorticityScale = other->vorticityScale;
    velocityKinematicViscosity = other->velocityKinematicViscosity;
    velocityDiffusionIterations = other->velocityDiffusionIterations;
    buoyancyScale = other->buoyancyScale;
    windScale = other->windScale;

    smokeKinematicViscosity = other->smokeKinematicViscosity;
    smokeDiffusionIterations = other->smokeDiffusionIterations;
    smokeDissipation = other->smokeDissipation;
    tempKinematicViscosity = other->tempKinematicViscosity;
    tempDiffusionIterations = other->tempDiffusionIterations;
}

std::string Settings::getName() {
    return name;
}

Settings Settings::withName(std::string name) const {
    Settings newSetting = Settings(this);
    newSetting.name = name;
    return newSetting;
}

ivec3 Settings::getSize(Resolution res) {
    switch(res) {
        case Resolution::velocity: return velocityResSize;
        case Resolution::substance: return substanceResSize;
    }
}

float Settings::getResToSimFactor(Resolution res) {
    switch(res) {
        case Resolution::velocity: return velocityToSimFactor;
        case Resolution::substance: return substanceToSimFactor;
    }
}

vec3 Settings::getSimulationSize() {
    return simulationSize;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
Settings Settings::withSize(ivec3 sizeRatio, int velocityScale, int substanceScale, float simulationScale) const {
    Settings newSetting = Settings(this);
    newSetting.simulationSize = simulationScale * vec3(sizeRatio);
    newSetting.velocityResSize = velocityScale * sizeRatio + ivec3(2, 2, 2);
    newSetting.velocityToSimFactor = simulationScale / velocityScale;
    newSetting.substanceResSize = substanceScale * sizeRatio + ivec3(2, 2, 2);
    newSetting.substanceToSimFactor = simulationScale / substanceScale;
    return newSetting;
}
#pragma clang diagnostic pop

float Settings::getDeltaTime() {
    return dt;
}

Settings Settings::withDeltaTime(float dt) const {
    Settings newSetting = Settings(this);
    newSetting.dt = dt;
    return newSetting;
}

SourceMode Settings::getSourceMode() {
    return sourceMode;
}

Settings Settings::withSourceMode(SourceMode mode) const {
    Settings newSetting = Settings(this);
    newSetting.sourceMode = mode;
    return newSetting;
}

SourceType Settings::getSourceType() {
    return sourceType;
}

Settings Settings::withSourceType(SourceType type) const {
    Settings newSetting = Settings(this);
    newSetting.sourceType = type;
    return newSetting;
}

float Settings::getTempSourceDensity() {
    return temperatureSourceDensity;
}

Settings Settings::withTempSourceDensity(float density) const {
    Settings newSetting = Settings(this);
    newSetting.temperatureSourceDensity = density;
    return newSetting;
}

float Settings::getSmokeSourceDensity() {
    return smokeSourceDensity;
}

Settings Settings::withSmokeSourceDensity(float density) const {
    Settings newSetting = Settings(this);
    newSetting.smokeSourceDensity = density;
    return newSetting;
}

float Settings::getVelKinematicViscosity() {
    return velocityKinematicViscosity;
}

int Settings::getVelDiffusionIterations() {
    return velocityDiffusionIterations;
}

Settings Settings::withVelDiffusion(float viscosity, int iterations) const {
    Settings newSetting = Settings(this);
    newSetting.velocityKinematicViscosity = viscosity;
    newSetting.velocityDiffusionIterations = iterations;
    return newSetting;
}

float Settings::getVorticityScale() {
    return vorticityScale;
}

Settings Settings::withVorticityScale(float vorticityScale) const {
    Settings newSetting = Settings(this);
    newSetting.vorticityScale = vorticityScale;
    return newSetting;
}

int Settings::getProjectionIterations() {
    return projectionIterations;
}

Settings Settings::withProjectIterations(int projectionIterations) const {
    Settings newSetting = Settings(this);
    newSetting.projectionIterations = projectionIterations;
    return newSetting;
}

float Settings::getBuoyancyScale() {
    return buoyancyScale;
}

Settings Settings::withBuoyancyScale(float buoyancyScale) const {
    Settings newSetting = Settings(this);
    newSetting.buoyancyScale = buoyancyScale;
    return newSetting;
}

float Settings::getWindScale() {
    return windScale;
}

Settings Settings::withWindScale(float windScale) const {
    Settings newSetting = Settings(this);
    newSetting.windScale = windScale;
    return newSetting;
}

float Settings::getSmokeKinematicViscosity() {
    return smokeKinematicViscosity;
}

int Settings::getSmokeDiffusionIterations() {
    return smokeDiffusionIterations;
}

Settings Settings::withSmokeDiffusion(float viscosity, int iterations) const {
    Settings newSetting = Settings(this);
    newSetting.smokeKinematicViscosity = viscosity;
    newSetting.smokeDiffusionIterations = iterations;
    return newSetting;
}

float Settings::getSmokeDissipation() {
    return smokeDissipation;
}

Settings Settings::withSmokeDissipation(float smokeDissipation) const {
    Settings newSetting = Settings(this);
    newSetting.smokeDissipation = smokeDissipation;
    return newSetting;
}

float Settings::getTempKinematicViscosity() {
    return tempKinematicViscosity;
}

int Settings::getTempDiffusionIterations() {
    return tempDiffusionIterations;
}

Settings Settings::withTempDiffusion(float viscosity, int iterations) const {
    Settings newSetting = Settings(this);
    newSetting.tempKinematicViscosity = viscosity;
    newSetting.tempDiffusionIterations = iterations;
    return newSetting;
}
