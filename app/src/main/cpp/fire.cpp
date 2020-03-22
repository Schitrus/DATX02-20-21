//
// Created by Kalle on 2020-03-04.
//

#include "fire.h"
#include "file_loader.h"
#include <android/asset_manager_jni.h>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

Fire::Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height)
    : javaEnvironment(javaEnvironment), assetManager(assetManager),
      screen_width(width), screen_height(height) {
    initFileLoader(assetManager);
}

void Fire::init(){
    renderer.init(assetManager);
    GLuint data;
    int width, height, depth;
    simulator.init();
    simulator.getData(data, width, height, depth);
    renderer.setData(data, width, height, depth);
}

void Fire::resize(int width, int height){
    fire->renderer.resize(width, height);
}

void Fire::update(){
    simulator.update();
    GLuint data;
    int width, height, depth;
    simulator.getData(data, width, height, depth);
    renderer.setData(data, width, height, depth);
    renderer.update();
}

void Fire::touch(double dx, double dy){
    renderer.touch(dx, dy);
}

void Fire::scale(float scaleFactor, double scaleX, double scaleY){
    renderer.scale(scaleFactor, scaleX, scaleY);
}


AAssetManager* loadAssetManager(JNIEnv *env, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        ALOGE("error loading asset manger");
    } else {
        LOGE("loaded asset manager");
    }
    return mgr;
}



// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JNIEnv* env, jobject , jobject mgr, jint width, jint height){

    fire = new Fire(env, loadAssetManager(env, mgr), width, height);
}

// FireRenderer
JC(void) Java_com_pbf_FireRenderer_init(JCT){
    fire->init();
}

JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height){
    fire->resize(width, height);
}

JC(void) Java_com_pbf_FireRenderer_update(JCT){
    fire->update();
}

// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble dx, jdouble dy){
    fire->touch(dx, dy);
}

JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY){
    fire->scale(scaleFactor, scaleX, scaleY);
}

