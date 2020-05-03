//
// Created by kirderf on 2020-05-03.
//

#include "field_initialization.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
void initSourceField(float* field, float value, Resolution res, Settings settings) {
    if(settings.getSourceType() == SourceType::singleSphere) {
        float radius = 6.4f;
        vec3 center = vec3(0.5f, 0.2f, 0.5f) * settings.getSimulationSize();

        fillSphere(field, value, center, radius, res, settings);
    }
}

float* createScalarField(float value, ivec3 gridSize) {
    float* field = new float[gridSize.x * gridSize.y * gridSize.z];
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                int index = gridSize.x * (gridSize.y * z + y) + x;
                field[index] = value;
            }
        }
    }
    return field;
}

vec3* createVectorField(vec3 value, ivec3 gridSize) {
    vec3* field = new vec3[gridSize.x * gridSize.y * gridSize.z];
    for (int z = 0; z < gridSize.z; z++) {
        for (int y = 0; y < gridSize.y; y++) {
            for (int x = 0; x < gridSize.x; x++) {
                int index = gridSize.x * (gridSize.y * z + y) + x;
                field[index] = value;
            }
        }
    }
    return field;
}

void fillField(float *field, float value, vec3 minPos, vec3 maxPos, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    //Volume for an 1x1x1 voxel cell, in meters
    float cellVolume = (1 * toSimulationScale) * (1 * toSimulationScale) * (1 * toSimulationScale);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {
                //Lower corner of cell in simulation space (no border)
                vec3 pos = vec3(x, y, z) * toSimulationScale;
                //Upper corner of cell in simulation space (no border)
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) * toSimulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    if(settings.getSourceMode() == SourceMode::add)
                        field[index] = value*(overlappedVolume/cellVolume);
                    else field[index] = value;
                }
            }
        }
    }
}

void fillOutgoingVector(vec3 *field, float scale, vec3 minPos, vec3 maxPos, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    //center position of filled area in simulation space
    vec3 center = (minPos + maxPos)/2.0f;
    //Volume for an 1x1x1 voxel cell, in meters
    float cellVolume = (1 * toSimulationScale) * (1 * toSimulationScale) * (1 * toSimulationScale);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {
                //Lower corner of cell in simulation space (no border)
                vec3 pos = vec3(x, y, z) * toSimulationScale;
                //Upper corner of cell in simulation space (no border)
                vec3 pos1 = vec3(x + 1, y + 1, z + 1) * toSimulationScale;
                //Does this cell overlap with the fill area?
                if(hasOverlap(pos, pos1, minPos, maxPos)) {
                    float overlappedVolume = getOverlapVolume(pos, pos1, minPos, maxPos);

                    vec3 vector = pos - center;
                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    if(settings.getSourceMode() == SourceMode::add)
                        field[index] = vector*(scale*overlappedVolume/cellVolume);
                    else field[index] = vector;
                }
            }
        }
    }
}


void fillSphere(float* field, float value, vec3 center, float radius, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {

                //Center of cell in simulation space (no border)
                vec3 pos = vec3(x + 0.5f, y + 0.5f, z + 0.5f) * toSimulationScale;

                if(distance(pos, center) <= radius) {

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = value;
                }
            }
        }
    }
}

void fillSphere(vec3* field, vec3 value, vec3 center, float radius, Resolution res, Settings settings) {
    int border = 1;
    ivec3 gridSize = settings.getSize(res);
    float toSimulationScale = settings.getResToSimFactor(res);
    for (int z = 0; z < gridSize.z - 2 * border; z++) {
        for (int y = 0; y < gridSize.y - 2 * border; y++) {
            for (int x = 0; x < gridSize.x - 2 * border; x++) {

                //Center of cell in simulation space (no border)
                vec3 pos = vec3(x + 0.5f, y + 0.5f, z + 0.5f) * toSimulationScale;

                if(distance(pos, center) <= radius) {

                    //Index of position in texture space (with border)
                    int index = gridSize.x * (gridSize.y * (z + border) + y + border) + x + border;
                    field[index] = value;
                }
            }
        }
    }
}


#pragma clang diagnostic pop

bool hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    return max1.x > min2.x && max1.y > min2.y && max1.z > min2.z
           && min1.x < max2.x && min1.y < max2.y && min1.z < max2.z;
}

float getOverlapVolume(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    vec3 overlapMin = max(min1, min2);
    vec3 overlapMax = min(max1, max2);
    return (overlapMax.x - overlapMin.x)*(overlapMax.y - overlapMin.y)*(overlapMax.z - overlapMin.z);
}