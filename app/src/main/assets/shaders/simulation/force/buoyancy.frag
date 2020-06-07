#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D temperature_field;
layout(binding = 1) uniform sampler3D velocity_field;

uniform float dt;
uniform int depth;
uniform float scale;
uniform vec3 temp_border_width;
uniform vec3 gridSize;
uniform vec3 direction;

out vec3 outVelocity;

// Performs the buoyancy step, simulating density changes caused by temperature
void main() {

    float ambient_temperature = 0.0f;

    ivec3 position = ivec3(gl_FragCoord.xy, depth);
    // Because the borders in the temperature and velocity textures are 1 voxel in different resolutions,
    // coordinate conversion between the two are slightly messy, and is best done while the border is not accounted for
    // The coordinate of the center of the voxel
    vec3 voxel_center = vec3(position) + vec3(0.5);
    // Texture coordinate for a texture without a border. Is the one type of coordinate that is the same for both types of texture resolutions
    vec3 tex_without_border_coords = (voxel_center - vec3(1))/(gridSize - vec3(2));
    // Texture coordinate for the temperature texture (with border)
    vec3 temp_tex_coord = temp_border_width + tex_without_border_coords*(vec3(1) - 2.0f*temp_border_width);

    float temperature = texture(temperature_field, temp_tex_coord).x;
    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;

    //vec3 vertical_direction = vec3(0.0f, 1.0f, 0.0f);

    // Buoyancy force
    //vec3 buoyancy = (temperature - ambient_temperature) * vertical_direction;
    vec3 buoyancy = (temperature - ambient_temperature) * direction;

    outVelocity = velocity + scale * buoyancy * dt;
}
