//
// Created by kirderf on 2020-04-23.
//

#include <string>
#include <android/log.h>

#include "settings.h"

#define LOG_TAG "SETTINGS"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

Settings::Settings() {

    name = "untitled";

    velocityResSize = ivec3(1, 1, 1) + ivec3(2, 2, 2);
    substanceResSize = ivec3(1, 1, 1) + ivec3(2, 2, 2);

    velocityScale = 1;
    substanceScale = 1;

    resScale = 1.0f;
    simulationScale = 1.0f;
    velocityToSimFactor = 1.0f;
    substanceToSimFactor = 1.0f;
    simulationSize = ivec3(1.0f, 1.0f, 1.0f);
    dt = 0.0f;

    sizeRatio = ivec3(1, 1, 1);

    touchMode = true;
    orientationMode = true;
    backgroundColor = vec3(0.0f, 0.0f, 0.0f);
    filterColor = vec3(1.0f, 1.0f, 1.0f);
    colorSpace = vec3(1.0f, 1.0f, 1.0f);

    boundaryType = BoundaryType::none;
    sourceMode = SourceMode::add;
    sourceType = SourceType::singleSphere;
    sourceRadius = 0.0f;
    sourceTemperature = 0.0f;
    sourceDensity = 0.0f;
    sourceVelocity = vec3(0.0f, 0.0f, 0.0f);

    orientationVector = vec3(0.0f, 1.0f, 0.0f);

    projectionIterations = 0;
    vorticityScale = 0.0f;
    velocityKinematicViscosity = 0.0f;
    velocityDiffusionIterations = 0;
    buoyancyScale = 0.0f;

    windStrength = 0.0f;
    rotatingWindAngle = false;
    windAngle = 0.0f;

    smokeKinematicViscosity = 0.0f;
    smokeDiffusionIterations = 0;
    smokeDissipation = 0.0f;
    tempKinematicViscosity = 0.0f;
    tempDiffusionIterations = 0;

    customMinBand = false;
    customMaxBand = false;
    minBand = 1.0f;
    maxBand = 1.0f;
}

void Settings::printInfo(std::string header) {

    LOG_INFO("SETTING INFO: %s", header.c_str());

    LOG_INFO("name: %s", name.c_str());
    LOG_INFO("VelocityResSize: %d, %d, %d", velocityResSize.x, velocityResSize.y, velocityResSize.z);
    LOG_INFO("SubstanceResSize: %d, %d, %d", substanceResSize.x, substanceResSize.y, substanceResSize.z);
    LOG_INFO("velocityScale: %d", velocityScale);
    LOG_INFO("substanceScale: %d", substanceScale);
    LOG_INFO("resScale: %f", resScale);
    LOG_INFO("simulationScale: %f", simulationScale);
    LOG_INFO("velocityToSimFactor: %f", velocityToSimFactor);
    LOG_INFO("substanceToSimFactor: %f", substanceToSimFactor);
    LOG_INFO("simulationSize: %f, %f, %f", simulationSize.x, simulationSize.y, simulationSize.z);
    LOG_INFO("dt: %f", dt);
    LOG_INFO("sizeRatio: %d, %d, %d", sizeRatio.x, sizeRatio.y, sizeRatio.z);
    LOG_INFO("touchMode: %s", touchMode ? "true" : "false");
    LOG_INFO("orientationMode: %s", orientationMode ? "true" : "false");
    LOG_INFO("backgroundColor: %f, %f, %f", backgroundColor.x, backgroundColor.y, backgroundColor.z);
    LOG_INFO("filterColor: %f, %f, %f", filterColor.x, filterColor.y, filterColor.z);
    LOG_INFO("colorSpace: %f, %f, %f", colorSpace.x, colorSpace.y, colorSpace.z);
    LOG_INFO("boundaryType: %d", (int)boundaryType);
    LOG_INFO("sourceMode: %d", (int)sourceMode);
    LOG_INFO("sourceType: %d", (int)sourceType);
    LOG_INFO("sourceRadius: %f", sourceRadius);
    LOG_INFO("sourceTemperature: %f", sourceTemperature);
    LOG_INFO("sourceDensity: %f", sourceDensity);
    LOG_INFO("sourceVelocity: %f, %f, %f", sourceVelocity.x, sourceVelocity.y, sourceVelocity.z);
    LOG_INFO("orientationVector: %f, %f, %f", orientationVector.x, orientationVector.y, orientationVector.z);
    LOG_INFO("projectionIterations: %d", projectionIterations);
    LOG_INFO("vorticityScale: %f", vorticityScale);
    LOG_INFO("velocityKinematicViscosity: %f", velocityKinematicViscosity);
    LOG_INFO("velocityDiffusionIterations: %d", velocityDiffusionIterations);
    LOG_INFO("buoyancyScale: %f", buoyancyScale);
    LOG_INFO("windStrength: %f", windStrength);
    LOG_INFO("rotatingWindAngle: %s", rotatingWindAngle ? "true" : "false");
    LOG_INFO("windAngle: %f", windAngle);
    LOG_INFO("smokeKinematicViscosity: %f", smokeKinematicViscosity);
    LOG_INFO("smokeDiffusionIterations: %d", smokeDiffusionIterations);
    LOG_INFO("smokeDissipation: %f", smokeDissipation);
    LOG_INFO("tempKinematicViscosity: %f", tempKinematicViscosity);
    LOG_INFO("tempDiffusionIterations: %d", tempDiffusionIterations);
    LOG_INFO("customMinBand: %s", customMinBand ? "true" : "false");
    LOG_INFO("customMaxBand: %s", customMaxBand ? "true" : "false");
    LOG_INFO("minBand: %f", minBand);
    LOG_INFO("maxBand: %f", maxBand);
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

Settings* Settings::withSize(ivec3 sizeRatio, int velocityScale, int substanceScale, float simulationScale) {
    this->simulationScale = simulationScale;
    this->resScale = substanceScale/velocityScale;
    this->sizeRatio = sizeRatio;
    this->velocityScale = velocityScale;
    this->substanceScale = substanceScale;
    this->simulationSize = simulationScale * vec3(sizeRatio);
    this->velocityResSize = ivec3((float)velocityScale * vec3(sizeRatio)) + ivec3(2, 2, 2);
    this->velocityToSimFactor = simulationScale / velocityScale;
    this->substanceResSize = ivec3((float)substanceScale * vec3(sizeRatio)) + ivec3(2, 2, 2);
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

float Settings::getSourceTemperature() {
    return sourceTemperature;
}

Settings* Settings::withSourceTemperature(float density) {
    this->sourceTemperature = density;
    return this;
}

float Settings::getSourceDensity() {
    return sourceDensity;
}

Settings* Settings::withSourceDensity(float density) {
    this->sourceDensity = density;
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
    return windStrength;
}

Settings* Settings::withWindStrength(float windScale) {
    this->windStrength = windScale;
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

bool Settings::getTouchMode(){
    return touchMode;
}
Settings* Settings::withTouchMode(bool touchMode){
    this->touchMode = touchMode;
    return this;
}

bool Settings::getOrientationMode(){
    return orientationMode;
}
Settings* Settings::withOrientationMode(bool orientationMode){
    this->orientationMode = orientationMode;
    return this;
}

float Settings::getSourceRadius(){
    return sourceRadius;
}
Settings* Settings::withSourceRadius(float radius){
    this->sourceRadius = radius;
    return this;
}

vec3 Settings::getSourceVelocity(){
    return sourceVelocity;
}
Settings* Settings::withSourceVelocity(float length){
    this->sourceVelocity = orientationVector * length;
    return this;
}

vec3 Settings::getOrientationVector(){
    return orientationVector;
}
Settings* Settings::withOrientationVector(vec3 orientationVector){
    this->orientationVector = orientationVector;
    return this;
}

bool Settings::getRotatingWindAngle(){
    return rotatingWindAngle;
}

Settings* Settings::withRotatingWindAngle(bool rotatingWindAngle){
    this->rotatingWindAngle = rotatingWindAngle;
    return this;
}

float Settings::getWindAngle(){
    return windAngle;
}

Settings* Settings::withWindAngle(float windAngle){
    this->windAngle = windAngle;
    return this;
}

bool Settings::getCustomMinBand(){
    return customMinBand;
}

Settings* Settings::withCustomMinBand(bool customMinBand){
    this->customMinBand = customMinBand;
    return this;
}

bool Settings::getCustomMaxBand(){
    return customMaxBand;
}

Settings* Settings::withCustomMaxBand(bool customMaxBand){
    this->customMaxBand = customMaxBand;
    return this;
}

float Settings::getMinBand(){
    return minBand;
}

Settings* Settings::withMinBand(float minBand){
    this->minBand = minBand;
    return this;
}

float Settings::getMaxBand(){
    return maxBand;
}

Settings* Settings::withMaxBand(float maxBand){
    this->maxBand = maxBand;
    return this;
}

BoundaryType Settings::getBoundaryType(){
    return boundaryType;
}

Settings* Settings::withBoundaryType(BoundaryType boundaryType){
    this->boundaryType = boundaryType;
    return this;
}
