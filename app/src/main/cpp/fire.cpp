//
// Created by Kalle on 2020-03-04.
//

#include "fire.h"
#include "file_loader.h"

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

Fire::Fire(int width, int height) : screen_width(width), screen_height(height){

}

void Fire::update(){
    simulator.update();
    renderer.update();
}

// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JCT, jint width, jint height){
    fire = new Fire(width, height);
}

JC(void) Java_com_pbf_FireActivity_initFileLoader(JNIEnv *env, jobject obj, jobject assetManager){
    initFileLoader(env, obj, assetManager);
    std::string fileText = loadFileFromAssets("shader.vert");
    __android_log_print(ANDROID_LOG_INFO, "FileLoader", "%s", fileText.c_str());
}

// FireRenderer
JC(void) Java_com_pbf_FireRenderer_init(JNIEnv *env, jobject, jobject mgr){
    fire->renderer.init(env, mgr);
    fire->simulator.init();
}

JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height){
    fire->renderer.resize(width, height);
}

JC(void) Java_com_pbf_FireRenderer_update(JCT){
    fire->update();
}

// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble dx, jdouble dy){
    fire->renderer.touch(dx, dy);
}

JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY){
    fire->renderer.scale(scaleFactor, scaleX, scaleY);
}

