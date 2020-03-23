//
// Created by Kalle on 2020-03-10.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H

#include <jni.h>
#include <GLES3/gl31.h>

class Shader {
    GLuint shader_program;
public:
    void load(const char* vertex_path, const char* fragment_path);

    void use();

    GLuint program();
private:
    GLuint createShader(GLenum type, const char* src);

    GLuint createProgram(const char* vertex_path, const char* fragment_path);
};


#endif //DATX02_20_21_SHADER_H