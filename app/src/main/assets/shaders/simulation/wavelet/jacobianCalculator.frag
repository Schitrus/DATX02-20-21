#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D advected_field;

uniform int depth;
uniform vec3 gridSize;
uniform int axis;

// Result data from the advection
out vec3 outTextureCoord;

// inspired by Theodore Kim & Nils Thürey, 2 of the authors of the paper "Wavelet Turbulence for Fluid Simulation"
// https://www.cs.cornell.edu/~tedkim/WTURB/source.html
void main() {
    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    vec3 center, next, prev, dprev, dnext, dcenter;

    vec3 dx = vec3(1.0f, 0.0f, 0.0f);
    vec3 dy = vec3(0.0f, 1.0f, 0.0f);
    vec3 dz = vec3(0.0f, 0.0f, 1.0f);

    center = texture(advected_field, (vec3(position)  + vec3(0.5))/gridSize).xyz;

    if(axis == 0){
        // fetch the previous value as well as the next value on the x-axis
        next = texture(advected_field, ((vec3(position) + dx) + vec3(0.5))/gridSize).xyz;
        prev = texture(advected_field, ((vec3(position) - dx) + vec3(0.5))/gridSize).xyz;
        //fetch texture coords for estimatation of the partial derivative
        dprev = (center - prev) * gridSize.x;
        dnext = (next - center) * gridSize.x;
        dcenter = (dnext - dprev) * gridSize.x;
    } else if (axis == 1){
        // fetch the previous value as well as the next value on the y-axis
        next = texture(advected_field, ((vec3(position) + dy) + vec3(0.5))/gridSize).xyz;
        prev = texture(advected_field, ((vec3(position) - dy) + vec3(0.5))/gridSize).xyz;
        //fetch texture coords for estimatation of the partial derivative
        dprev = (center - prev) * gridSize.y;
        dnext = (next - center) * gridSize.y;
        dcenter = (dnext - dprev) * gridSize.y;
    } else {
        // fetch the previous value as well as the next value on the z-axis
        next = texture(advected_field, ((vec3(position) + dz) + vec3(0.5))/gridSize).xyz;
        prev = texture(advected_field, ((vec3(position) - dz) + vec3(0.5))/gridSize).xyz;
        //fetch texture coords for estimatation of the partial derivative
        dprev = (center - prev) * gridSize.z;
        dnext = (next - center) * gridSize.z;
        dcenter = (dnext - dprev) * gridSize.z;
    }

    // calculate the smallest values of each vector, and return a new vector with these values
    float d0 = (abs(dcenter.x) < abs(dnext.x)) ? dcenter.x : dnext.x;
    d0 = (abs(d0) < abs(dprev.x)) ? d0 : dprev.x;

    float d1 = (abs(dcenter.y) < abs(dnext.y)) ? dcenter.y : dnext.y;
    d1 = (abs(d1) < abs(dprev.y)) ? d1 : dprev.y;

    float d2 = (abs(dcenter.z) < abs(dnext.z)) ? dcenter.z : dnext.z;
    d2 = (abs(d2) < abs(dprev.z)) ? d2 : dprev.z;

    outTextureCoord = vec3(d0,d1,d2);
}