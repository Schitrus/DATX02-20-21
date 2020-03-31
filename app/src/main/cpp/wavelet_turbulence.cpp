//
// Created by Kalle on 2020-04-01.
//

#include "wavelet_turbulence.h"

#define LOG_TAG "Renderer"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

vec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
                   {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

vec3 seed = vec3(rand()%10000, rand()%10000, rand()%10000);

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

double lerp(double a, double b, double t){
    return (a + t * (b - a));
}

double WaveletTurbulence::perlin(vec3 position){
    vec3 internal_position = position - floor(position);
    vec3 external_position = floor(position);
    double dots[8] = {0};
    for (int c = 0; c < 8; c++){
        vec3 external_corner = external_position + corners[c] + seed;
        double yaw   = (angle[int(external_corner.x*external_corner.x+external_corner.y)%256]
                        +  angle[int(external_corner.y*external_corner.y+external_corner.x)%256]) / 180.0 * PI;
        double pitch = (angle[int(external_corner.x*external_corner.x+external_corner.z)%256]
                        +  angle[int(external_corner.z*external_corner.z+external_corner.x)%256]) / 180.0 * PI;
        dots[c] = dot(vec3(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), cos(pitch)), internal_position - corners[c]);
    }
    return  lerp(   lerp(   lerp(dots[0], dots[1], fade(internal_position.x)),
                            lerp(dots[2], dots[3], fade(internal_position.x)),
                            fade(internal_position.y)),
                    lerp(   lerp(dots[4], dots[5], fade(internal_position.x)),
                            lerp(dots[6], dots[7], fade(internal_position.x)),
                            fade(internal_position.y)),
                    fade(internal_position.z));
}