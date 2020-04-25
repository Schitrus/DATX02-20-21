//
// Created by kirderf on 2020-04-23.
//

#include <string>

#include "settings.h"
#include "fire/simulation/simulator.h"

const Settings DEFAULT = Settings().withSize(ivec3(1, 4, 1), 12, 60, 24.0f).withName("Default");
const Settings FEW_ITERATIONS = DEFAULT.withProjectIterations(5).withName("Few Iterations");
const Settings SMALL_RES = DEFAULT.withSize(ivec3(1, 4, 1), 6, 24, 24.0f).withName("Small Resolution");
const Settings LARGE_RES = DEFAULT.withSize(ivec3(1, 4, 1), 24, 120, 24.0f).withName("Large Resolution");

int index = -1;

Settings nextSettings() {
    index++;
    if(index >= 4)
        index = 0;
    switch(index) {
        case 1: return FEW_ITERATIONS;
        case 2: return SMALL_RES;
        case 3: return LARGE_RES;
        default: return DEFAULT;
    }
}

Settings::Settings() {
    this->projectionIterations = 20;
    this->vorticityScale = 8.0f;
}

Settings::Settings(const Settings* other) {
    this->name = other->name;
    this->projectionIterations = other->projectionIterations;
    this->vorticityScale = other->vorticityScale;
}

std::string Settings::getName() {
    return name;
}

Settings Settings::withName(std::string name) const {
    Settings newSetting = Settings(this);
    newSetting.name = name;
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

float Settings::getVorticityScale() {
    return vorticityScale;
}

Settings Settings::withVorticityScale(float vorticityScale) const {
    Settings newSetting = Settings(this);
    newSetting.vorticityScale = vorticityScale;
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
