//
// Created by Daniel on 2020-03-05.
//

#ifndef DATX02_20_21_FILE_LOADER_H
#define DATX02_20_21_FILE_LOADER_H

#include <GLES3/gl31.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <string>

class FileLoader;

extern FileLoader *fileLoader;

class FileLoader {
    AAssetManager *assetManager;
public:
    FileLoader(JNIEnv *env, jobject obj, jobject assetManager);
    std::string loadFile(const char *path);
};


#endif //DATX02_20_21_FILE_LOADER_H
