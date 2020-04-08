//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"
#include "simulator.h"

#include <stdio.h>

#define LOG_TAG "wavelet"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


int WaveletTurbulence::init(SlabOperator* slab) {
    this->slab = slab;
    if(!initShaders())
        return 0;

    texture_coord = createScalarDataPair(false, nullptr);
    energy = createScalarDataPair(true, nullptr);

    band_min = glm::log2(min(min((float)lowResSize.x, (float)lowResSize.y), (float)lowResSize.z));
    band_max = glm::log2(max(max((float)highResSize.x, (float)highResSize.y), (float)highResSize.z)/2);

    //generateWavelet();

    wavelet_turbulence = createVectorDataPair(true, nullptr);

    return 1;
}

void WaveletTurbulence::generateAngles() {
    num_angles = 256;
    angles = new double[num_angles];
    for(int i = 0; i < num_angles; i++)
        angles[i] = (rand()%36000)/100.0;
}

int WaveletTurbulence::initShaders() {
    bool success = true;
    //success &= turbulenceShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/turbulence.frag");
    success &= synthesisShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/fluid_synthesis.frag");
    success &= textureCoordShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/advection.frag");
    success &= energyShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/energy_spectrum.frag");
    return success;
}

void WaveletTurbulence::generateWavelet(){

    generateAngles();

    double* w1 = generateTurbulence(highResSize + ivec3(2.0));
    double* w2 = generateTurbulence(highResSize + ivec3(2.0));
    double* w3 = generateTurbulence(highResSize + ivec3(2.0));


    vec3* wavelet = new vec3[int(highResSize.x*highResSize.y*highResSize.z)];
    int dx = 1;
    int dy = highResSize.x;
    int dz = highResSize.y*highResSize.x;
    for(int z = 0; z < highResSize.z; z++) {
        for (int y = 0; y < highResSize.y; y++) {
            for (int x = 0; x < highResSize.x; x++) {
                int wi  = z*highResSize.y*highResSize.x + y*highResSize.x + x;
                int ti  = wi+dz+dy+dx;
                wavelet[wi].x = 0.5*((w1[ti-dy] - w1[ti+dy]) - (w2[ti-dz] - w2[ti+dz]));
                wavelet[wi].y = 0.5*((w3[ti-dz] - w3[ti+dz]) - (w1[ti-dx] - w1[ti+dx]));
                wavelet[wi].z = 0.5*((w2[ti-dx] - w2[ti+dx]) - (w3[ti-dy] - w3[ti+dy]));
            }
        }
        LOGE("Generating noise: %f %", glm::round(100 * z/(float)highResSize.z));
    }

    wavelet_turbulence = createVectorDataPair(true, wavelet);
    delete[] wavelet;
    delete[] w1;
    delete[] w2;
    delete[] w3;
}

double* WaveletTurbulence::generateTurbulence(vec3 size) {
    vec3 offset = vec3(rand()%10000, rand()%10000, rand()%10000);
    double* turbulence = new double[int(size.x*size.y*size.z)];
    for(int z = 0; z < size.z; z++){
        for(int y = 0; y < size.y; y++){
            for(int x = 0; x < size.x; x++){
                int index = z*size.y*size.x + y*size.x + x;
                turbulence[index] = 0;
                for(int i = band_min; i <= band_max; i++){
                    turbulence[index] += fabs(pow(2.0, -(5.0/6.0)*(i-band_min)) * perlin(float(pow(2.0, i)) * vec3(x/size.x, y/size.y, z/size.z) + offset));
                }
            }
        }
    }
    return turbulence;
}



vec3 seed = vec3(rand()%10000, rand()%10000, rand()%10000);



double fade(double x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}
//linear interpolation with smoothstep
double lerp(double a, double b, double t){
    return (a + t * (b - a));
}

//Get perlin noise from 2d value
double WaveletTurbulence::perlin(vec3 position){
    vec3 internal_position = fract(position);
    vec3 external_position = floor(position);  																	//Extern position (-infinity, -infinity to +infinity, +infinity)
    double dots[8] = {0};																					//Declare 4 dot products, one for each corner
    //Loop through all corners
    for (int c = 0; c < 8; c++){
        vec3 external_corner = external_position + corners[c] + seed;																			//Extern corner coordinate
        //Get gradient angle from sum of extern corner values (x, y)
        double yaw   = (angles[int(external_corner.x*external_corner.x+external_corner.y)%num_angles]
                        +  angles[int(external_corner.y*external_corner.y+external_corner.x)%num_angles]) / 180.0 * PI;
        double pitch = (angles[int(external_corner.x*external_corner.x+external_corner.z)%num_angles]
                        +  angles[int(external_corner.z*external_corner.z+external_corner.x)%num_angles]) / 180.0 * PI;
        dots[c] = dot(vec3(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), cos(pitch)), internal_position - corners[c]);												//Get dot product from distance and gradient vector
    }
    //Interpolate dot product values with common y value
    //then interpolate the interpolated values and return
    return  lerp(   lerp(   lerp(dots[0], dots[1], fade(internal_position.x)),
                            lerp(dots[2], dots[3], fade(internal_position.x)),
                            fade(internal_position.y)),
                    lerp(   lerp(dots[4], dots[5], fade(internal_position.x)),
                            lerp(dots[6], dots[7], fade(internal_position.x)),
                            fade(internal_position.y)),
                    fade(internal_position.z));
}

void WaveletTurbulence::advection(DataTexturePair* lowerVelocity, float dt){
    textureCoordShader.use();
    glViewport(0, 0, lowResSize.x, lowResSize.y);

    textureCoordShader.uniform3f("gridSize", lowResSize);
    textureCoordShader.uniform1f("dt", dt);

    lowerVelocity->bindData(GL_TEXTURE0);

    slab->fullOperation(textureCoordShader, texture_coord);
}

void WaveletTurbulence::calcEnergy(DataTexturePair* lowerVelocity){
    energyShader.use();
    lowerVelocity->bindData(GL_TEXTURE0);
    slab->fullOperation(energyShader, energy);
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    synthesisShader.uniform3f("gridSize", highResSize);
    synthesisShader.uniform1f("scale", length((vec3)highResSize)/length((vec3)lowResSize));

    lowerVelocity->bindData(GL_TEXTURE0);
    wavelet_turbulence->bindData(GL_TEXTURE1);
    texture_coord->bindData(GL_TEXTURE2);
    energy->bindData(GL_TEXTURE3);

    slab->fullOperation(synthesisShader, higherVelocity);

}

