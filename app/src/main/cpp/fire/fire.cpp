//
// Created by Kalle on 2020-03-04.
//

#include "fire.h"
#include "util/file_loader.h"
#include <android/asset_manager_jni.h>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#define LOG_TAG "FIRE"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Enable touch interaction with the fire, if not defined touch will rotate camera.
// Enable/disable by commenting/uncommenting the line below
#define ENABLE_TOUCH_INTERACTION

Fire::Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height)
    : javaEnvironment(javaEnvironment), assetManager(assetManager),
      screen_width(width), screen_height(height) {
    initFileLoader(assetManager);
}

int Fire::init(){
    return renderer.init(assetManager) && simulator.init();
}

void Fire::resize(int width, int height){
    fire->renderer.resize(width, height);
}

void Fire::update(){
    simulator.update();
    GLuint density, temperature;
    int width, height, depth;
    simulator.getData(density, temperature, width, height, depth);
    renderer.setData(density, temperature, width, height, depth);
    renderer.update();
}

void Fire::touch(double x, double y, double dx, double dy){
#ifndef ENABLE_TOUCH_INTERACTION
    renderer.touch(dx, dy);
    return;
#endif
//Only works when camera is not rotated

    float aspectRatio = (float)screen_width / screen_height;
    int width, height, depth;
    simulator.getSize(width, height, depth);

    // cube coordinates in ray_renderer
    float m = max(max(width, height), depth);
    vec3 tex = vec3(width, height, depth)/m;

    //check if x & y coordinate is inside the grid area
    double lowerBoundX = screen_width/2 - (screen_width * tex.x);
    double higherBoundX = screen_width/2 + (screen_width * tex.x);
    if(x < lowerBoundX || x > higherBoundX){
        return;
    }

    double higherBoundY = screen_height/2 + (screen_height * tex.y * aspectRatio);
    double lowerBoundY = screen_height/2 - (screen_height * tex.y * aspectRatio);
    if(y < lowerBoundY || y > higherBoundY){
        return;
    }

    //translate coordinates to texCoords
    double w = higherBoundX-lowerBoundX;
    double pixels_per_cellX = w/width;
    x = floor((x-lowerBoundX)/pixels_per_cellX);

    double h = higherBoundY-lowerBoundY;
    double pixels_per_cellY = h/height;
    y = height - ceil((y-lowerBoundY)/pixels_per_cellY);

    simulator.touch(x, y, dx, -dy);

    return;
}

void Fire::scale(float scaleFactor, double scaleX, double scaleY){
    renderer.scale(scaleFactor, scaleX, scaleY);
}


AAssetManager* loadAssetManager(JNIEnv *env, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        LOG_ERROR("error loading asset manger");
    } else {
        LOG_INFO("loaded asset manager");
    }
    return mgr;
}


// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JNIEnv* env, jobject , jobject mgr, jint width, jint height){

    fire = new Fire(env, loadAssetManager(env, mgr), width, height);
}

// FireRenderer
JC(jint) Java_com_pbf_FireRenderer_init(JCT){
    return fire->init();
}

JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height){
    fire->resize(width, height);
}

JC(void) Java_com_pbf_FireRenderer_update(JCT){
    fire->update();
}

// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble x, jdouble y, jdouble dx, jdouble dy){
    fire->touch(x, y, dx, dy);
}

JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY){
    fire->scale(scaleFactor, scaleX, scaleY);
}

