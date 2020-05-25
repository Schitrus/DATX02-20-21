//
// Created by kirderf on 2020-05-03.
//

#include <glm/glm.hpp>
#include <fire/settings.h>

#ifndef DATX02_20_21_FIELD_INITIALIZATION_H
#define DATX02_20_21_FIELD_INITIALIZATION_H

void initSourceField(float* field, float value, Resolution res, Settings* settings);

// creates a field array to use for texture creation, that need to be deleted after use
float* createScalarField(float value, ivec3 gridSize);

// creates a field array to use for texture creation, that need to be deleted after use
vec3* createVectorField(vec3 value, ivec3 gridSize);

// value is in unit
void fillField(float* field, float value, vec3 minPos, vec3 maxPos, Resolution res, Settings* settings);

void fillSphere(float* field, float value, vec3 center, float radius, Resolution res, Settings* settings);
void fillSphere(vec3* field, vec3 value, vec3 center, float radius, Resolution res, Settings* settingse);

// fills the field with vectors pointing outward from the center,
// and that scale with the distance from the center
// scale is unit/meter from center
void fillOutgoingVector(vec3* field, float scale, vec3 minPos, vec3 maxPos, Resolution res, Settings* settings);

bool hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

// might return negative values if there is no overlap
float getOverlapVolume(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

#endif //DATX02_20_21_FIELD_INITIALIZATION_H
