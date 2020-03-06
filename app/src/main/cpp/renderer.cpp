#include "renderer.h"

#include <jni.h>
#include <string>

#include <time.h>
#include <math.h>
#include <chrono>

#include <stdlib.h>
#include <utility>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

void Renderer::init(AAssetManager* assetManager){
    rayRenderer.init(assetManager);
}

void Renderer::resize(int width, int height){
    rayRenderer.resize(width, height);
}

void Renderer::update(){
    rayRenderer.step();
}

void Renderer::scale(float scaleFactor, double scaleX, double scaleY){
    // TODO implement
}

void Renderer::touch(double dx, double dy){
    // TODO implement
}