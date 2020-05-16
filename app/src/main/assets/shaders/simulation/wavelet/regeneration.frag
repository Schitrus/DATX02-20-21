#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D texture_field;
layout(binding = 1) uniform sampler3D eigen_field;

uniform int depth;
uniform vec3 gridSize;

// Result data from the advection
out vec3 outTextureCoord;

//Performs the advection step on the given data under the given velocity
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    float firstEigen = texture(eigen_field, (vec3(position) + vec3(0.5))/gridSize).x;
    float secondEigen = texture(eigen_field, (vec3(position) + vec3(0.5))/gridSize).y;
    float thirdEigen = texture(eigen_field, (vec3(position) + vec3(0.5))/gridSize).z;

    float highEigen = max(max(firstEigen, secondEigen), thirdEigen);
    float lowEigen = min(min(firstEigen, secondEigen), thirdEigen);

    vec3 coord = texture(texture_field, (vec3(position) + vec3(0.5))/gridSize).xyz;

    if (highEigen > 2.0f || lowEigen < 0.5f){
        coord = (vec3(position) + vec3(0.5)) / (gridSize);
    }
    // Set result to velocity at previous position
    // Note: Linear interpolation due to linear texture
    outTextureCoord = coord;
}
