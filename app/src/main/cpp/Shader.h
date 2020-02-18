//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H


static const char VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"
        "layout(location = 1) in vec3 color;\n"
        "out vec3 vColor;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 1.0);\n"
        "    vColor = color;\n"
        "}\n";


static const char FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision mediump float;\n"
        "in vec3 vColor;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = vec4(vColor, 1.0);\n"
        "}\n";


#endif //DATX02_20_21_SHADER_H
