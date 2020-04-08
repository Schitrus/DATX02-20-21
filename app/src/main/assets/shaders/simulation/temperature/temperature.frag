#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D temperature_field;

uniform float dt;
uniform int depth;

out float outTemperature;

// Performs heat dissipation of the temperature field
void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    float temperature = texelFetch(temperature_field, position, 0).x;

    float ambient_temperature = 0.0f;
    float max_temperature = 3000.0f - 273.15f;
   // float temperature_loss = pow((temperature - ambient_temperature) / (max_temperature - ambient_temperature), 4.0f);
    float temperature_loss = ((temperature - ambient_temperature) / (max_temperature - ambient_temperature)) *3.5f; //todo fel men ger bättre resultat?

    outTemperature = temperature - dt * max_temperature * temperature_loss;
}