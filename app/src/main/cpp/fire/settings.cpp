//
// Created by kirderf on 2020-04-23.
//

#include <string>
#include <android/log.h>

#include "settings.h"
#include "fire/simulation/simulator.h"

#define LOG_TAG "SETTINGS"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

/*
const Settings DEFAULT = Settings().withSize(ivec3(1, 4, 1), 12, 60, 24.0f).withDeltaTime(1/30.0f)
        .withSourceMode(SourceMode::set).withSourceType(SourceType::singleSphere).withTempSourceDensity(3500.0f)
        .withSmokeSourceDensity(0.4f).withVelDiffusion(0.0f, 0).withVorticityScale(8.0f).withProjectIterations(20)
        .withBuoyancyScale(0.15f).withWindScale(0.0f).withSmokeDiffusion(0.0f, 0).withSmokeDissipation(0.0f)
        .withTempDiffusion(0.0f, 0).withBackgroundColor(vec3(0.0f, 0.0f, 0.0f)).withFilterColor(vec3(1.0f, 1.0f, 1.0f))
        .withColorSpace(vec3(1.8f, 2.2f, 2.2f)).withName("Default");
*/
Settings::Settings() {
    dt = 0.0f;

    resScale = 1;
    simulationScale = 1;
    sizeRatio = ivec3(1, 1, 1);

    velocityScale = 1;
    substanceScale = 1;

    velocityResSize = ivec3(1.0f, 1.0f, 1.0f) + ivec3(2, 2, 2);
    substanceResSize = ivec3(1.0f, 1.0f, 1.0f) + ivec3(2, 2, 2);
    velocityToSimFactor = 1.0f;
    substanceToSimFactor = 1.0f;
    simulationSize = ivec3(1.0f, 1.0f, 1.0f);

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

    backgroundColor = vec3(0.0f, 0.0f, 0.0f);
    filterColor = vec3(1.0f, 1.0f, 1.0f);
    colorSpace = vec3(1.0f, 1.0f, 1.0f);

}

std::string Settings::getName() {
    return name;
}

Settings* Settings::withName(std::string name) {
    this->name = name;
    return this;
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

ivec3 Settings::getSizeRatio(){
    return sizeRatio;
}

float Settings::getResScale(){
    return resScale;
}

float Settings::getSimulationScale(){
    return simulationScale;
}

float Settings::getVelocityScale() {
    return velocityScale;
}

float Settings::getSubstanceScale() {
    return substanceScale;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

Settings* Settings::withSize(ivec3 sizeRatio, float velocityScale, float substanceScale, float simulationScale) {
    this->simulationScale = simulationScale;
    this->resScale = ((float)substanceScale/velocityScale);
    this->sizeRatio = sizeRatio;
    this->velocityScale = velocityScale;
    this->substanceScale = substanceScale;
    this->simulationSize = simulationScale * vec3(sizeRatio);
    this->velocityResSize = ivec3(velocityScale * vec3(sizeRatio)) + ivec3(2, 2, 2);
    this->velocityToSimFactor = simulationScale / velocityScale;
    this->substanceResSize = ivec3(substanceScale * vec3(sizeRatio)) + ivec3(2, 2, 2);
    this->substanceToSimFactor = simulationScale / substanceScale;
    return this;
}
#pragma clang diagnostic pop

float Settings::getDeltaTime() {
    return dt;
}

Settings* Settings::withDeltaTime(float dt) {
   this->dt = dt;
    return this;
}

SourceMode Settings::getSourceMode() {
    return sourceMode;
}

Settings* Settings::withSourceMode(SourceMode mode) {
    this->sourceMode = mode;
    return this;
}

SourceType Settings::getSourceType() {
    return sourceType;
}

Settings* Settings::withSourceType(SourceType type) {
    this->sourceType = type;
    return this;
}

float Settings::getTempSourceDensity() {
    return temperatureSourceDensity;
}

Settings* Settings::withTempSourceDensity(float density) {
    this->temperatureSourceDensity = density;
    return this;
}

float Settings::getSmokeSourceDensity() {
    return smokeSourceDensity;
}

Settings* Settings::withSmokeSourceDensity(float density) {
    this->smokeSourceDensity = density;
    return this;
}

float Settings::getVelKinematicViscosity() {
    return velocityKinematicViscosity;
}

int Settings::getVelDiffusionIterations() {
    return velocityDiffusionIterations;
}

Settings* Settings::withVelDiffusion(float viscosity, int iterations) {
    this->velocityKinematicViscosity = viscosity;
    this->velocityDiffusionIterations = iterations;
    return this;
}

float Settings::getVorticityScale() {
    return vorticityScale;
}

Settings* Settings::withVorticityScale(float vorticityScale) {
    this->vorticityScale = vorticityScale;
    return this;
}

int Settings::getProjectionIterations() {
    return projectionIterations;
}

Settings* Settings::withProjectIterations(int projectionIterations) {
    this->projectionIterations = projectionIterations;
    return this;
}

float Settings::getBuoyancyScale() {
    return buoyancyScale;
}

Settings* Settings::withBuoyancyScale(float buoyancyScale) {
    this->buoyancyScale = buoyancyScale;
    return this;
}

float Settings::getWindScale() {
    return windScale;
}

Settings* Settings::withWindScale(float windScale) {
    this->windScale = windScale;
    return this;
}

float Settings::getSmokeKinematicViscosity() {
    return smokeKinematicViscosity;
}

int Settings::getSmokeDiffusionIterations() {
    return smokeDiffusionIterations;
}

Settings* Settings::withSmokeDiffusion(float viscosity, int iterations) {
    this->smokeKinematicViscosity = viscosity;
    this->smokeDiffusionIterations = iterations;
    return this;
}

float Settings::getSmokeDissipation() {
    return smokeDissipation;
}

Settings* Settings::withSmokeDissipation(float smokeDissipation) {
    this->smokeDissipation = smokeDissipation;
    return this;
}

float Settings::getTempKinematicViscosity() {
    return tempKinematicViscosity;
}

int Settings::getTempDiffusionIterations() {
    return tempDiffusionIterations;
}

Settings* Settings::withTempDiffusion(float viscosity, int iterations) {
    this->tempKinematicViscosity = viscosity;
    this->tempDiffusionIterations = iterations;
    return this;
}

vec3 Settings::getBackgroundColor(){
    return backgroundColor;
}

Settings* Settings::withBackgroundColor(vec3 backgroundColor) {
    this->backgroundColor = backgroundColor;
    return this;
}

vec3 Settings::getFilterColor(){
    return filterColor;
}

Settings* Settings::withFilterColor(vec3 filterColor) {
    this->filterColor = filterColor;
    return this;
}

vec3 Settings::getColorSpace(){
    return colorSpace;
}

Settings* Settings::withColorSpace(vec3 colorSpace) {
    this->colorSpace = colorSpace;
    return this;
}
