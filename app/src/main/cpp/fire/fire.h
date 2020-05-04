//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_FIRE_H
#define DATX02_20_21_FIRE_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "rendering/renderer.h"
#include "simulation/simulator.h"

/*
 Did you ever hear the tragedy of Darth Plagueis The Wise?
 I thought not. It’s not a story the Jedi would tell you.
 It’s a Sith legend. Darth Plagueis was a Dark Lord of the Sith,
 so powerful and so wise he could use the Force to influence the
 midichlorians to create life… He had such a knowledge of the
 dark side that he could even keep the ones he cared about from dying.
 The dark side of the Force is a pathway to many abilities some
 consider to be unnatural. He became so powerful… the only thing
 he was afraid of was losing his power, which eventually, of course,
 he did. Unfortunately, he taught his apprentice everything he knew,
 then his apprentice killed him in his sleep. Ironic. He could save
 others from death, but not himself.
*/
class Fire{
    int screen_width, screen_height;
public:

    Renderer renderer;
    Simulator simulator;

    JNIEnv* javaEnvironment;
    AAssetManager* assetManager;

    Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height);

    int init();
    void resize(int width, int height);
    void update();

    void touch(double dx, double dy);
    void scale(float scaleFactor, double scaleX, double scaleY);
};

Fire* fire;

AAssetManager* loadAssetManager(JNIEnv *env, jobject assetManager);

// Interface between java and c++
#define JC(T) extern "C" JNIEXPORT T JNICALL
#define JCT JNIEnv*, jobject

// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JNIEnv* env, jobject, jobject mgr, jint width, jint height);
JC(void) Java_com_pbf_FireActivity_initFileLoader(JNIEnv *env, jobject obj, jobject assetManager);
// FireRenderer
JC(jint) Java_com_pbf_FireRenderer_init(JCT);
JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height);
JC(void) Java_com_pbf_FireRenderer_update(JCT);
// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble dx, jdouble dy);
JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY);

#endif //DATX02_20_21_FIRE_H
