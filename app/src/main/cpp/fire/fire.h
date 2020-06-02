//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_FIRE_H
#define DATX02_20_21_FIRE_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <mutex>
#include <condition_variable>

#include "rendering/renderer.h"
#include "simulation/simulator.h"
#include "settings.h"



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

    bool shouldUpdateSettings;
    bool shouldRegenFields;

public:

    Settings* settings;

    Renderer* renderer;
    Simulator* simulator;

    JNIEnv* javaEnvironment;
    AAssetManager* assetManager;

    Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height);

    int init();
    void resize(int width, int height);
    void update();

    void touch(double x, double y, double dx, double dy);
    void scale(float scaleFactor, double scaleX, double scaleY);

    void onClick();

    void setTouchMode(bool touchMode);
    void setOrientation(bool orientationMode);
    void updateResolution(int lowerRes);
    void updateResolutionScale(float scale);
    void updateSimulationScale(float scale);
    void updateTimeStep(float timeStep);
    void updateBackgroundColor(float red, float green, float blue);
    void updateFilterColor(float red, float green, float blue);
    void updateColorSpace(float X, float Y, float Z);
    void updateObjectType(std::string type);
    void updateObjectRadius(float radius);
    void updateObjectTemperature(float temperature);
    void updateObjectDensity(float density);
    void updateObjectVelocity(float velocity);
    void updateWindStrength(float strength);
    void setWindAngle(bool custom);
    void updateWindAngle(float angle);
    void updateVorticity(float vorticityScale);
    void updateBuoyancy(float buoyancyScale);
    void updateSmokeDissipation(float dissipation);
    void updateTemperatureViscosity(float viscosity);
    void updateSmokeViscosity(float viscosity);
    void updateVelocityViscosity(float viscosity);
    void setMinNoiseBand(bool custom);
    void updateMinNoiseBand(float band);
    void setMaxNoiseBand(bool custom);
    void updateMaxNoiseBand(float band);
    void updateDensityDiffusionIterations(int iterations);
    void updateVelocityDiffusionIterations(int iterations);
    void updateProjectionIterations(int iterations);
    void updateBoundaries(std::string mode);

    bool changedSettings();
};

Fire* fire;

AAssetManager* loadAssetManager(JNIEnv *env, jobject assetManager);

// Interface between java and c++
#define JC(T) extern "C" JNIEXPORT T JNICALL
#define JCT JNIEnv*, jobject

// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JNIEnv* env, jobject, jobject mgr, jint width, jint height);
// FireRenderer
JC(jint) Java_com_pbf_FireRenderer_init(JCT);
JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height);
JC(void) Java_com_pbf_FireRenderer_update(JCT);
// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble x, jdouble y, jdouble dx, jdouble dy);
JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY);
JC(void) Java_com_pbf_FireListener_onClick(JCT);

JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateResolutionScale(JCT, jfloat scale);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSimulationScale(JCT, jfloat scale);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateTimeStep(JCT, jfloat timeStep);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectRadius(JCT, jfloat radius);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectTemperature(JCT, jfloat temperature);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectDensity(JCT, jfloat density);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectVelocity(JCT, jfloat velocity);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateWindStrength(JCT, jfloat strength);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateWindAngle(JCT, jfloat angle);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVorticity(JCT, jfloat vorticityScale);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateBuoyancy(JCT, jfloat buoyancyScale);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSmokeDissipation(JCT, jfloat dissipation);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateTemperatureViscosity(JCT, jfloat viscosity);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSmokeViscosity(JCT, jfloat viscosity);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVelocityViscosity(JCT, jfloat viscosity);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateMinNoiseBand(JCT, jfloat band);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateMaxNoiseBand(JCT, jfloat band);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateDensityDiffusionIterations(JCT, jint iterations);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVelocityDiffusionIterations(JCT, jint iterations);
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateProjectionIterations(JCT, jint iterations);

JC(void) Java_com_pbf_SettingsFragment_setTouchMode(JCT, jboolean touchMode);
JC(void) Java_com_pbf_SettingsFragment_setOrientation(JCT, jboolean orientationMode);
JC(void) Java_com_pbf_SettingsFragment_updateResolution(JCT, jint lowerRes);
JC(void) Java_com_pbf_SettingsFragment_updateBackgroundColor(JCT, jfloat red, jfloat green, jfloat blue);
JC(void) Java_com_pbf_SettingsFragment_updateFilterColor(JCT, jfloat red, jfloat green, jfloat blue);
JC(void) Java_com_pbf_SettingsFragment_updateColorSpace(JCT, jfloat X, jfloat Y, jfloat Z);
JC(void) Java_com_pbf_SettingsFragment_updateObjectType(JCT, jstring type);
JC(void) Java_com_pbf_SettingsFragment_setWindAngle(JCT, jboolean custom);
JC(void) Java_com_pbf_SettingsFragment_setMinNoiseBand(JCT, jboolean custom);
JC(void) Java_com_pbf_SettingsFragment_setMaxNoiseBand(JCT, jboolean custom);
JC(void) Java_com_pbf_SettingsFragment_updateBoundaries(JCT, jstring mode);

JC(jboolean) Java_com_pbf_FireRenderer_changedSettings(JCT);

#endif //DATX02_20_21_FIRE_H