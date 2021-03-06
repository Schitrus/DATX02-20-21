#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;

uniform float dt;
uniform float meterToVoxels;  //conversion factor from meter to voxels
uniform int depth;
uniform vec3 gridSize;

// Result data from the advection
out vec3 outTextureCoord;

//Performs the advection step on the given data under the given velocity
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    // Get velocity at a specific position in the velocity field
    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    // Location of the previous position, back in time
    vec3 previous_position = vec3(position) - dt * velocity * meterToVoxels;

    // Set result to velocity at previous position
    // Note: Linear interpolation due to linear texture
    outTextureCoord = (previous_position + vec3(0.5)) / (gridSize);
}

