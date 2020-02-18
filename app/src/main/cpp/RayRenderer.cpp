//
// Created by Anton Forsberg on 18/02/2020.
//
#include <jni.h>

#include <time.h>
#include <math.h>
#include <chrono>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

#include "helper.h"
#include "Shader.h"
void init();

void initQuad();

void initProgram();

void resize(int width, int height);

void step();

extern "C" {

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_RayRenderer_init(JNIEnv *env, jobject) {
    init();
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_RayRenderer_resize(JNIEnv *env, jobject, jint width, jint height) {
    resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_example_datx02_120_121_RayRenderer_step(JNIEnv *env, jobject /* this */) {
    step();
}

}

GLuint shaderProgram;
GLuint vertexArrayObject;
GLuint positionBuffer, colorBuffer, indexBuffer;

void init() {
    glClearColor(1.0, 0.2, 0.2, 0.2);
    initQuad();
    initProgram();
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void initQuad() {

    glGenVertexArrays(1, &vertexArrayObject);
    // Bind the vertex array object
    // The following calls will affect this vertex array object.
    glBindVertexArray(vertexArrayObject);

    ///////////////////////////////////
    // Define the positions for each of the three vertices of the triangle
    const float positions[] = {
            // X      Y       Z
            -0.5f, -0.5f, 1.0f,  // v0
            -0.5f, 0.5f, 1.0f, // v1
            0.5f, 0.5f, 1.0f, // v2
            0.5f, -0.5f, 1.0f   // v3
    };

    // Create a handle for the position vertex buffer object
    glGenBuffers(1, &positionBuffer);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    // Send the vertex position data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // Attaches positionBuffer to vertexArrayObject, in the 0th attribute location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex position attribute
    glEnableVertexAttribArray(0);

    ///////////////////////////////////

    // Define the colors for each of the three vertices of the triangle
    const float colors[] = {
            //   R     G     B
            0.0f, 0.0f, 1.0f, //
            0.0f, 1.0f, 1.0f, //
            0.0f, 1.0f, 0.0f,  //
            0.0f, 1.0f, 1.0f  //
    };
    // Create a handle for the vertex color buffer
    glGenBuffers(1, &colorBuffer);
    // Set the newly created buffer as the current one
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    // Send the vertex color data to the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // Attaches colorBuffer to vertexArrayObject, in the 1st attribute location
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE /*normalized*/, 0 /*stride*/, 0 /*offset*/);
    // Enable the vertex color attribute
    glEnableVertexAttribArray(1);

    ///////////////////////////////////

    const int indices[] = {
            0, 1, 3, // Triangle 1
            1, 2, 3  // Triangle 2
    };
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void initProgram() {
    shaderProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
}

void step() {

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Set the shader program to use for this draw call
    glUseProgram(shaderProgram);

    glBindVertexArray(vertexArrayObject);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}