//
// Created by Kalle on 2020-04-23.
//

#include "turbulence.h"

#include <pthread.h>

struct thread_turbulence{
    double* t;
    float band, min_band, max_band;
    ivec3 position, size;
};

typedef struct thread_turbulence turb_struct;

void* threadTurbulence(void* args){
    turb_struct* turb = (turb_struct*)args;
    double* t = turb->t;
    float band = turb->band;
    float min_band = turb->min_band;
    float max_band = turb->max_band;
    ivec3 position = turb->position;
    ivec3 size = turb->size;

    float len = max(max(size.x, size.y), size.z);

    for(int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            int index = position.z * size.y * size.x + y * size.x + x;
            t[index] += fabs(pow(2.0, -(band-min_band)) * perlin(float(pow(2.0, band)) * vec3((float)x/len, (float)y/len, (float)position.z/len)));
        }
    }

    pthread_exit(NULL);
}

#define NUM_THREADS 16

double* turbulence(ivec3 size, float min_band, float max_band) {

    double* t = new double[size.x * size.y * size.z];

    pthread_t th[NUM_THREADS];

    LOG_INFO("GENERATING");

    for(int i = 0; i < size.x*size.y*size.z; i++)
        t[i] = 0.0f;

    for(float band = min_band; band <= max_band; band += 1.0) {
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

    LOG_INFO("GENERATING DONE");

    return t;
}