//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_SLAB_OPERATION_H
#define DATX02_20_21_SLAB_OPERATION_H

#include <jni.h>
#include <GLES3/gl31.h>

void init(JNIEnv *env, jobject mgr);

void initData();

void initLine();

void initQuad();

void initProgram();

void step();

void slabOperation();

#endif //DATX02_20_21_SLAB_OPERATION_H
