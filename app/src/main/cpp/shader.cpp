//
// Created by Kalle on 2020-03-10.
//

#include "shader.h"

#include <jni.h>
#include <gles3/gl31.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include <string>

#include "helper.h"
#include "file_loader.h"

#define LOG_TAG "shader"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int Shader::load(const char* vertex_path, const char* fragment_path){
    shader_program = createProgram(vertex_path, fragment_path);
    return shader_program != 0;
}

void Shader::use(){
    if(program() != 0)
        glUseProgram(program());
    else LOG_ERROR("Tried to use a shader that isn't initiated!");
}

GLuint Shader::program(){
    return shader_program;
}

GLuint Shader::createShader(GLenum type, const char *src) {
    clearGLErrors("shader creation");
    GLuint shader = glCreateShader(type);
    if (!shader) {
        checkGLError("shader creation");
        return 0;
    }

    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                LOG_ERROR("Could not compile %s shader:\n%s\n",
                      type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                          infoLog);
                free(infoLog);
            }
        }
        return 0;
    }

    return shader;
}

GLuint Shader::createProgram(const char *vertex_path, const char *fragment_path) {
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    GLuint shaderProgram = 0;
    GLint linked = GL_FALSE;

    const char* vertexSrc;
    const char* fragmentSrc;

    std::string vertex;
    std::string fragment;

    vertex = loadFileFromAssets(vertex_path);
    vertexSrc = vertex.c_str();

    LOG_INFO("Creating Vertex shader: %s", vertex_path);
    vertex_shader = createShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vertex_shader){
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    fragment = loadFileFromAssets(fragment_path);
    fragmentSrc = fragment.c_str();

    LOG_INFO("Creating Fragment shader: %s", fragment_path);
    fragment_shader = createShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fragment_shader) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    LOG_INFO("Creating Program: %s, %s", vertex_path, fragment_path);
    clearGLErrors("shader program creation");
    shader_program = glCreateProgram();
    if (!shader_program) {
        checkGLError("shader program creation");
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOG_ERROR("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar *infoLog = (GLchar *) malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(shader_program, infoLogLen, NULL, infoLog);
                LOG_ERROR("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(shader_program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

void Shader::uniform1i(const GLchar *name, GLint value) {
    if(program() != 0)
        glUniform1i(glGetUniformLocation(program(), name), value);
    else LOG_ERROR("Tried to set uniform %s for a shader that isn't initiated!", name);
}

void Shader::uniform1f(const GLchar *name, GLfloat value) {
    if(program() != 0)
        glUniform1f(glGetUniformLocation(program(), name), value);
    else LOG_ERROR("Tried to set uniform %s for a shader that isn't initiated!", name);
}

void Shader::uniform3f(const GLchar *name, ivec3 vector) {
    if(program() != 0)
        glUniform3f(glGetUniformLocation(program(), name), vector.x, vector.y, vector.z);
    else LOG_ERROR("Tried to set uniform %s for a shader that isn't initiated!", name);
}