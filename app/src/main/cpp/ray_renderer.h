//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_RAY_RENDERER_H
#define DATX02_20_21_RAY_RENDERER_H

#include <jni.h>
#include <GLES3/gl31.h>

#include <android/asset_manager.h>

class RayRenderer{
public:
    void init(AAssetManager* assetManager);

    void resize(int width, int height);

    void step();

private:
    void loadAssetManager(JNIEnv *env, jobject assetManager);

    void load3DTexture(const char *fileName);

    void initCube();

    void initProgram();

    void loadMVP(GLuint shaderProgram);

    void display();
};

#endif //DATX02_20_21_RAY_RENDERER_H
