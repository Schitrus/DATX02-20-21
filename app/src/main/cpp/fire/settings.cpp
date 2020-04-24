//
// Created by kirderf on 2020-04-23.
//

#include <string>
#include "settings.h"

const Settings DEFAULT;
const Settings FEW_ITERATIONS = DEFAULT.withProjectIterations(10).withName("Few Iterations");
const Settings OTHER_EXAMPLE = DEFAULT.withProjectIterations(34).withVorticityScale(6.0f).withName("Example");

int index = -1;

Settings::Settings() {
    this->name = "Default";
    this->projectionIterations = 20;
    this->vorticityScale = 8.0f;
}

Settings::Settings(const Settings* other) {
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

Settings getNext() {
    index++;
    if(index >= 3)
        index = 0;
    switch(index) {
        case 1: return FEW_ITERATIONS;
        case 2: return OTHER_EXAMPLE;
        default: return DEFAULT;
    }
}
