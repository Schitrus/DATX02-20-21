//
// Created by Kalle on 2020-03-04.
//

#include "fire.h"
#include "util/file_loader.h"
#include "settings.h"
#include <android/asset_manager_jni.h>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#define LOG_TAG "FIRE"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

Fire::Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height)
    : javaEnvironment(javaEnvironment), assetManager(assetManager),
      screen_width(width), screen_height(height), shouldUpdateSettings(false) {
    initFileLoader(assetManager);
}

int Fire::init() {
    settings = new Settings;
    *settings = nextSettings();
    return renderer.init() && simulator.init(*settings);
}

void Fire::resize(int width, int height){
    fire->renderer.resize(width, height);
}

void Fire::update(){
    GLuint density, temperature;
    ivec3 size;

    if(shouldUpdateSettings){
        simulator.changeSettings(*settings);
        shouldUpdateSettings =  false;
    }

    simulator.update(density, temperature, size);

    renderer.update(density, temperature, size);
}

void Fire::touch(double dx, double dy){
    renderer.touch(dx, dy);
}

void Fire::scale(float scaleFactor, double scaleX, double scaleY){
    renderer.scale(scaleFactor, scaleX, scaleY);
}

void Fire::onClick() {
    //Settings newSettings = nextSettings();
    //std::string name = newSettings.getName();
    //LOG_INFO("Changing settings to %s", name.data());
    *settings = nextSettings();
    shouldUpdateSettings = true;
    //simulator.changeSettings(newSettings);
}

void Fire::updateWind(int strength) {
    *settings = settings->withWindScale(strength);
    LOG_INFO("WindUpdate, %d", strength);
    shouldUpdateSettings = true;
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
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble dx, jdouble dy){
    fire->touch(dx, dy);
}

JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY){
    fire->scale(scaleFactor, scaleX, scaleY);
}

JC(void) Java_com_pbf_FireListener_onClick(JCT){
    fire->onClick();
}

JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateWind(JCT, jint strength){
    fire->updateWind(strength);
}
