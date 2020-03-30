//
// Created by kirde on 2020-03-29.
//

#include "data_texture_pair.h"

#include <GLES3/gl32.h>

#include <glm/glm.hpp>

using namespace glm;



void swapData(GLuint& d1, GLuint& d2){
    GLuint tmp = d1;
    d1 = d2;
    d2 = tmp;
}