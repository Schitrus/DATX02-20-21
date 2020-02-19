//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H


static const char VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"
        "uniform mat4 mvp;\n"
        "out vec3 hit;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(pos, 1.0);\n"
        "    hit = pos;\n"
        "}\n";

static const char BACK_FACE_FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision highp float;\n"
        "in vec3 hit;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = vec4(hit, 1.0);\n"
        "}\n";

static const char FRONT_FACE_FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision highp float;\n"
        "in vec3 hit;\n"
        "layout(binding = 0) uniform sampler2D lastHit;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "vec3 last = texelFetch(lastHit , tcoord,0).xyz;\n"
        "vec3 direction = last.xyz - hit.xyz;\n"
    //    "direction = normalize(direction);\n"
        "    outColor = vec4(1.0-direction.x, 1.0-direction.y, 1.0-direction.z, 1.0);\n"
        "}\n";

#endif //DATX02_20_21_SHADER_H
