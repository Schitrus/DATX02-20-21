#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D velocity_field;

uniform int depth;
uniform float dt;
uniform float vorticityScale;
uniform float meterToVoxels; // Voxels to meters distance

out vec3 outData;

vec3 curl(ivec3 position){
    // Vectors used for adding/subtracting pressure vectors
    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    vec3 R = texelFetch(velocity_field, position + dx, 0).xyz;
    vec3 L = texelFetch(velocity_field, position - dx, 0).xyz;
    vec3 T = texelFetch(velocity_field, position + dy, 0).xyz;
    vec3 B = texelFetch(velocity_field, position - dy, 0).xyz;
    vec3 U = texelFetch(velocity_field, position + dz, 0).xyz;
    vec3 D = texelFetch(velocity_field, position - dz, 0).xyz;

    float curlX = (T.z - B.z) - (U.y - D.y);
    float curlY = (R.z - L.z) - (U.x - D.x);
    float curlZ = (R.y - L.y) - (T.x - B.x);
    vec3 curl = vec3(curlX, curlY, curlZ);

    return curl;
}

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    // Vectors used for adding/subtracting pressure vectors
    ivec3 dx = ivec3(1,0,0);
    ivec3 dy = ivec3(0,1,0);
    ivec3 dz = ivec3(0,0,1);

    vec3 R = abs(curl(position + dx));
    vec3 L = abs(curl(position - dx));
    vec3 T = abs(curl(position + dy));
    vec3 B = abs(curl(position - dy));
    vec3 U = abs(curl(position + dz));
    vec3 D = abs(curl(position - dz));
    vec3 currentCurl = curl(position);

    vec3 gradientVorticity = vec3(R.x - L.x, T.y - B.y, U.z - D.z);
    vec3 vorticityForce = vec3(0.0);
    if(dot(gradientVorticity, gradientVorticity) != 0.0){
        vec3 normGradVort = normalize(gradientVorticity);
        vorticityForce = vorticityScale * cross(normGradVort, currentCurl) * meterToVoxels;
    }

    vec3 velocity = texelFetch(velocity_field, position, 0).xyz;
    outData = velocity + vorticityForce * dt;
}
