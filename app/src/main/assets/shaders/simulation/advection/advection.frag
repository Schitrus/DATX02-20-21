#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;
layout(binding = 1) uniform sampler3D data_field;

uniform float dt;   //in seconds
uniform float meterToPixels;  //conversion factor from meter to pixels
uniform int depth;  //pixel layer that is updated
uniform vec3 gridSize;  //grid size in pixels

// Result data from the advection
out vec3 outData;

//Performs the advection step on the given data under the given velocity
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth); //position in pixels

    // Get velocity at a specific position in the velocity field
    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;    //velocity in meters/second

    // Location of the previous position, back in time
    vec3 previous_position = vec3(position) + vec3(0.5) - dt * velocity * meterToPixels;  //position in pixels

    // Set result to velocity at previous position
    // Note: Linear interpolation due to linear texture
    outData = texture(data_field, previous_position / gridSize).xyz;
}

