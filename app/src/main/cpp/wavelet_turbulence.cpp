//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#include "slab_operation.h"

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


int WaveletTurbulence::init(vec3 lowerResolution, vec3 higherResolution, GLuint VAO) {
    if(!initShaders())
        return 0;

    this->lowerResolution = lowerResolution;
    this->higherResolution = higherResolution;

    texture_coord = createScalarDataPair(lowerResolution, nullptr);

    band_min = log2(min(min(lowerResolution.x, lowerResolution.y), lowerResolution.z));
    band_max = log2(max(max(higherResolution.x, higherResolution.y), higherResolution.z)/2);

    generateWavelet();

    return 1;
}

int WaveletTurbulence::initShaders() {
    bool success = true;
    success &= turbulenceShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/turbulence.frag");
    success &= synthesisShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/fluid_synthesis.frag");
    success &= textureCoordShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/advection.frag");
    success &= energyShader.load("shaders/simulation/wavelet/turbulence.vert", "shaders/simulation/wavelet/energy_spectrum.frag");
    return success;
}

void WaveletTurbulence::generateWavelet(){
    double* w1 = generateTurbulence(higherResolution + vec3(2.0));
    double* w2 = generateTurbulence(higherResolution + vec3(2.0));
    double* w3 = generateTurbulence(higherResolution + vec3(2.0));


    vec3* wavelet = new vec3[higherResolution.x*higherResolution.y*higherResolution.z];
    int dx = 1;
    int dy = higherResolution.x;
    int dz = higherResolution.y*higherResolution.x;
    for(int z = 0; z < higherResolution.z; z++) {
        for (int y = 0; y < higherResolution.y; y++) {
            for (int x = 0; x < higherResolution.x; x++) {
                int wi  = z*higherResolution.y*higherResolution.x + y*higherResolution.x + x;
                int ti  = wi+dz+dy+dx;
                wavelet[wi].x = 0.5*((w1[ti-dy] - w1[ti+dy]) - (w2[ti-dz] - w2[ti+dz]));
                wavelet[wi].y = 0.5*((w3[ti-dz] - w3[ti+dz]) - (w1[ti-dx] - w1[ti+dx]));
                wavelet[wi].z = 0.5*((w2[ti-dx] - w2[ti+dx]) - (w3[ti-dy] - w3[ti+dy]));
            }
        }
    }

    wavelet_turbulence = createVectorDataPair(higherResolution, wavelet);
    delete[] wavelet, w1, w2, w3;
}

double* WaveletTurbulence::generateTurbulence(vec3 size) {
    vec3 offset = vec3(rand()%10000, rand()%10000, rand()%10000);
    double* turbulence = new double[size.x*size.y*size.z];
    for(int z = 0; z < size.z; z++){
        for(int y = 0; y < size.y; y++){
            for(int x = 0; x < size.x; x++){
                int index = z*size.y*size.x + y*size.x + x;
                turbulence[index] = 0;
                for(int i = band_min; i <= band_max; i++){
                    turbulence[index] += fabs(pow(2.0, -(5.0/6.0)*(i-band_min)) * perlin(pow(2.0, i) * vec3(x/size.x, y/size.y, z/size.z) + offset));
                }
            }
        }
    }
    return turbulence;
}

vec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
                   {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

vec3 seed = vec3(rand()%10000, rand()%10000, rand()%10000);

#define PI 3.1415

int angle[256] = {
        163,	 81,	 50,	 50,	 92,	164,	154,	 19,	274,	221,	 16,	174,	266,	 59,	  4,	302,
        65,	171,	124,	 96,	306,	230,	  7,	314,	241,	247,	142,	 81,	193,	 92,	267,	227,
        1,	257,	315,	118,	108,	  3,	120,	211,	 36,	 44,	 29,	256,	179,	331,	78 ,	 31,
        317,	 29,	345,	288,	253,	187,	227,	 60,	 48,	 78,	208,	211,	 79,	239,	123,	127,
        3,	277,	238,	271,	  8,	194,	252,	263,	 32,	301,	249,	 98,	136,	 98,	208,	206,
        344,	170,	249,	250,	197,	342,	325,	228,	 55,	196,	339,	358,	205,	 59,	  4,	125,
        122,	247,	 50,	161,	  7,	 80,	349,	247,	 87,	278,	314,	302,	326,	204,	271,	139,
        77,	330,	143,	236,	275,	245,	131,	100,	 94,	 44,	122,	202,	353,	295,	121,	 22,
        0,	157,	 64,	255,	329,	 55,	332,	 52,	 61,	192,	125,	 43,	 56,	270,	356,	263,
        30,	 50,	236,	156,	135,	 17,	 39,	148,	 27,	 28,	348,	233,	313,	 52,	245,	126,
        282,	299,	305,	252,	258,	 99,	207,	 33,	201,	198,	141,	222,	115,	298,	114,	 38,
        327,	274,	 63,	 76,	 68,	308,	138,	326,	278,	338,	331,	308,	347,	281,	 31,	320,
        322,	168,	301,	228,	185,	 57,	297,	 35,	 94,	346,	177,	 75,	344,	255,	150,	 77,
        242,	  7,	 88,	 19,	 79,	206,	211,	188,	255,	279,	320,	133,	226,	344,	209,	273,
        75,	 67,	221,	209,	 25,	324,	 35,	 11,	  1,	184,	273,	102,	302,	268,	169,	290,
        241,	283,	 42,	305,	 18,	255,	317,	316,	241,	154,	 40,	258,	128,	187,	 45,	355
};

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
        double yaw   = (angle[int(external_corner.x*external_corner.x+external_corner.y)%256]
                        +  angle[int(external_corner.y*external_corner.y+external_corner.x)%256]) / 180.0 * PI;
        double pitch = (angle[int(external_corner.x*external_corner.x+external_corner.z)%256]
                        +  angle[int(external_corner.z*external_corner.z+external_corner.x)%256]) / 180.0 * PI;
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
    glViewport(0, 0, lowerResolution.x, lowerResolution.y);

    for(int depth = 0; depth < lowerResolution.z; depth++){
        textureCoordShader.uniform3f("gridSize", lowerResolution.x, lowerResolution.y, lowerResolution.z);
        textureCoordShader.uniform1f("dt", dt);
        textureCoordShader.uniform1i("depth", depth);

        lowerVelocity->bindData(GL_TEXTURE0);

        texture_coord->bindToFramebuffer(depth);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    texture_coord->operationFinished();
}

void WaveletTurbulence::calcEnergy(DataTexturePair* lowerVelocity){
    energyShader.use();
    glViewport(0, 0, lowerResolution.x, lowerResolution.y);

    for(int depth = 0; depth < lowerResolution.z; depth++){

        textureCoordShader.uniform1i("depth", depth);

        lowerVelocity->bindData(GL_TEXTURE0);

        energy->bindToFramebuffer(depth);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)

    }
    energy->operationFinished();
}

void WaveletTurbulence::fluidSynthesis(DataTexturePair* lowerVelocity, DataTexturePair* higherVelocity){
    synthesisShader.use();
    glViewport(0, 0, higherResolution.x, higherResolution.y);

    for(int depth = 0; depth < higherResolution.z; depth++){
        synthesisShader.uniform3f("gridSize", higherResolution.x, higherResolution.y, higherResolution.z);
        synthesisShader.uniform1i("depth", depth);

        higherVelocity->bindToFramebuffer(depth);

        lowerVelocity->bindData(GL_TEXTURE0);
        noise->bindData(GL_TEXTURE1);
        texture_coord->bindData(GL_TEXTURE2);
        energy->bindData(GL_TEXTURE3);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    higherVelocity->operationFinished();
}

void WaveletTurbulence::turbulence() {
    turbulenceShader.use();
    glViewport(0, 0, higherResolution.x, higherResolution.y);
    for(int depth = 0; depth < higherResolution.z; depth++){
        noise->bindToFramebuffer(depth);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
    noise->operationFinished();
    for(int band = band_min; band <= band_max; band++){

        turbulenceShader.uniform3f("gridSize", higherResolution.x, higherResolution.y, higherResolution.z);
        turbulenceShader.uniform1f("band", band);
        turbulenceShader.uniform1f("min_band", band_min);

        for(int depth = 0; depth < higherResolution.z; depth++){
            noise->bindToFramebuffer(depth);
            glBindVertexArray(VAO);

            turbulenceShader.uniform1i("depth", depth);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        noise->operationFinished();
    }
}

