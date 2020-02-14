#include <jni.h>
#include <string>

#include <time.h>
#include <math.h>
#include <chrono>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

void initGraphics(int width, int height);
void updateGraphics();

extern "C" {
JNIEXPORT jstring JNICALL Java_com_example_datx02_120_121_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_init(
        JNIEnv *env,
        jobject,
        jint width,
        jint height);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_update(
        JNIEnv *env,
        jobject);
};

JNIEXPORT jstring JNICALL Java_com_example_datx02_120_121_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */){
    std::string hello;
    for (int i = 0; i < 10; i++)
        hello += "Hello from C++ and Kalle again! ";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_init(
        JNIEnv *env,
        jobject,
        jint width,
        jint height){
    initGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_update(
        JNIEnv *env,
        jobject){
    updateGraphics();
}

void initGraphics(int width, int height){
    glViewport(0, 0, width, height);
}

#define PI 3.14159265359

void updateGraphics(){
    // Time
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    double sec = ms.count()/1000.0;

    glClearColor(0.5f*sin(sec)+0.5f, 0.5f*sin(sec+(2*PI/3))+0.5f, 0.5f*sin(sec+(4*PI/3))+0.5f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}