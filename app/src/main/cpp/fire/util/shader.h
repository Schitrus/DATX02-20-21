//
// Created by Kalle on 2020-03-10.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H

#include <jni.h>
#include <GLES3/gl31.h>

#include <glm/glm.hpp>

using namespace glm;

class Shader {
    GLuint shader_program;
public:
    int load(const char* vertex_path, const char* fragment_path);

    int load(const char* compute_path);

    void use();

    GLuint program();

    void uniform1i(const GLchar *name, GLint value);

    void uniform1f(const GLchar *name, GLfloat value);

    void uniform3f(const GLchar *name, ivec3 vector);
private:
    GLuint createShader(GLenum type, const char* src);

    GLuint createProgram(const char *compute_path);

    GLuint createProgram(const char* vertex_path, const char* fragment_path);

};


#endif //DATX02_20_21_SHADER_H
