//
// Created by Daniel on 2020-03-05.
//

#include <android/asset_manager_jni.h>
#include <string>
#include "file_loader.h"
#include <memory>
#include <android/log.h>

#define LOG_TAG "file_loader"
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static FileLoader *fileLoader;

void initFileLoader(JNIEnv *env, jobject obj, jobject assetManager){
    fileLoader = new FileLoader(env, obj, assetManager);
}

std::string loadFileFromAssets(const char *path){
    return fileLoader->loadFile(path);
}

FileLoader::FileLoader(JNIEnv *env, jobject obj, jobject assetManager) {
    this->assetManager = AAssetManager_fromJava(env, assetManager);
}

// Return string instead of char pointer to avoid potential memory leaks
std::string FileLoader::loadFile(const char *path) {
    AAsset *asset = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);
    off_t size = AAsset_getLength(asset);

    std::unique_ptr<char[]> buffer(new char[size]);
    int bytesRead = AAsset_read(asset, buffer.get(), size);

    // If error occurred
    if(bytesRead < 0){
        return nullptr;
    }

    std::string fileContent(buffer.get(), size);

    return fileContent;
}
