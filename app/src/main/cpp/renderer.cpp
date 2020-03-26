#include "renderer.h"

#include <jni.h>
#include <string>

#include <time.h>
#include <math.h>
#include <chrono>

#include <stdlib.h>
#include <utility>

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

/*
 Water. Earth. Fire. Air.
 Long ago, the four nations lived together in harmony.
 Then everything changed when the Fire Nation attacked.
 Only the Avatar, master of all four elements,
 could stop them. But when the world needed him most,
 he vanished. A hundred years passed and my brother and
 I discovered the new Avatar, an airbender named Aang,
 and although his airbending skills are great, he still
 has a lot to learn before he's ready to save anyone.
 But I believe Aang can save the world.
*/

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

void Renderer::setData(GLuint data, int width, int height, int depth){
    rayRenderer.setData(data, width, height, depth);
}

void Renderer::getData(GLuint& data, int& width, int& height, int& depth){
    rayRenderer.getData(data, width, height, depth);
}