//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_FIRE_H
#define DATX02_20_21_FIRE_H

#include <jni.h>
#include <GLES3/gl31.h>

#include "renderer.h"
#include "simulator.h"

class Fire{
    int screen_width, screen_height;
public:

    Renderer renderer;
    Simulator simulator;

    Fire(int width, int height);
    void update();
};

Fire* fire;

// Interface between java and c++
#define JC(T) extern "C" JNIEXPORT T JNICALL
#define JCT JNIEnv*, jobject

// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JCT, jint width, jint height);
JC(void) Java_com_pbf_FireActivity_initFileLoader(JNIEnv *env, jobject obj, jobject assetManager);
// FireRenderer
JC(void) Java_com_pbf_FireRenderer_init(JCT, jobject mgr);
JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height);
JC(void) Java_com_pbf_FireRenderer_update(JCT);
// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble dx, jdouble dy);
JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY);

#endif //DATX02_20_21_FIRE_H
