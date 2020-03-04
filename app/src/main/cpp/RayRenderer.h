//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_RAYRENDERER_H
#define DATX02_20_21_RAYRENDERER_H

#include <jni.h>
#include <GLES3/gl31.h>

void init(JNIEnv *env, jobject mgr);

void loadAssetManager(JNIEnv *env, jobject assetManager);

void load3DTexture(const char *fileName);

void initCube();
git
void initProgram();

void loadAssetManager(JNIEnv *env, jobject mgr);

void load3DTexture(const char *fileName);

void resize(int width, int height);

void step();

void loadMVP(GLuint shaderProgram);

void display();

//THIS IS NOW CHANGED!!!!!!!

#endif //DATX02_20_21_RAYRENDERER_H
