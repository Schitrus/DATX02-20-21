//
// Created by Kalle on 2020-04-23.
//

#include "wavelet.h"

vec3* wavelet(ivec3 size, int min_band, int max_band){

    ivec3 tsize = size + ivec3(2.0);

    double* w1 = turbulence(tsize, min_band, max_band);
    double* w2 = turbulence(tsize, min_band, max_band);
    double* w3 = turbulence(tsize, min_band, max_band);

    vec3* w = new vec3[size.x*size.y*size.z];

    int dx = 1;
    int dy = tsize.y;
    int dz = tsize.y*tsize.x;
    for(int z = 0; z < size.z; z++){
        for(int y = 0; y < size.y; y++){
            for(int x = 0; x < size.x; x++){
                int wi  = (z+1)*tsize.y*tsize.x + (y+1)*tsize.x + (x+1);
                w[z*size.y*size.x + y*size.x + x].x = ((w1[wi+dy] - w1[wi-dy]) - (w2[wi+dz] - w2[wi-dz]));
                w[z*size.y*size.x + y*size.x + x].y = ((w3[wi+dz] - w3[wi-dz]) - (w1[wi+dx] - w1[wi-dx]));
                w[z*size.y*size.x + y*size.x + x].z = ((w2[wi+dx] - w2[wi-dx]) - (w3[wi+dy] - w3[wi-dy]));
            }
        }
    }

    delete[] w1;
    delete[] w2;
    delete[] w3;

    return w;
}