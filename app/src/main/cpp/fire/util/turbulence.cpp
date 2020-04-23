//
// Created by Kalle on 2020-04-23.
//

#include "turbulence.h"

#include <pthread.h>

struct thread_turbulence{
    double* t;
    int band, min_band, max_band;
    ivec3 position, size;
};

typedef struct thread_turbulence turb_struct;

void* threadTurbulence(void* args){
    turb_struct* turb = (turb_struct*)args;
    double* t = turb->t;
    int band = turb->band;
    int min_band = turb->min_band;
    int max_band = turb->max_band;
    vec3 position = turb->position;
    vec3 size = turb->size;

    for(int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            int index = position.z * size.y * size.x + y * size.x + x;
            t[index] += fabs(pow(2.0, -(band-min_band)) * perlin(float(pow(2.0, band)) * vec3((float)x/size.x, (float)y/size.y, (float)position.z/size.z)));
        }
    }

    pthread_exit(NULL);
}

#define NUM_THREADS 16

double* turbulence(ivec3 size, int min_band, int max_band) {

    double* t = new double[size.x * size.y * size.z];

    pthread_t th[NUM_THREADS];

    LOG_INFO("GENERATING");

    for(int i = 0; i < size.x*size.y*size.z; i++)
        t[i] = 0.0f;

    for(int band = min_band; band <= max_band; band++) {
        initPerlin();
        for (int z = 0; z < size.z; z++) {
            LOG_INFO("GEN: %d%%", (int) round(100 * (float) ((band-min_band)*size.z + z) / ((max_band-min_band+1)*size.z)));

            turb_struct *turb = new turb_struct;
            turb->t = t;
            turb->position = vec3(0, 0, z);
            turb->band = band;
            turb->min_band = min_band;
            turb->max_band = max_band;
            turb->size = size;


            if (z >= NUM_THREADS)
                pthread_join(th[z % NUM_THREADS], NULL);
            pthread_create(&th[z % NUM_THREADS], NULL, threadTurbulence, (void *) turb);

        }
        for(int i = 0; i < NUM_THREADS; i++)
            pthread_join(th[i], NULL);
    }

    for(int i = 0; i < size.x*size.y*size.z; i++)
        t[i] = 0.0f;

    LOG_INFO("GENERATING DONE");

    return t;
}