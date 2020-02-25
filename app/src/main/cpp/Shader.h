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
        // hej
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = vec4(hit, 1.0);\n"
        "}\n";

static const char FRONT_FACE_FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision highp float;\n"
        "precision highp sampler3D;\n"

        "in vec3 hit;\n"

        "layout(binding = 0) uniform sampler2D lastHit;\n"
        "layout(binding = 3) uniform sampler3D volume;\n"

        //     "uniform float h"             todo fix

        "out vec4 outColor;\n"

        "void main() {\n"
        "   ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "   vec3 last = texelFetch(lastHit , tcoord,0).xyz;\n"

        "   vec3 direction = last.xyz - hit.xyz;\n"
        "   float D = length(direction);\n"
        "   direction = normalize(direction);\n"

        "   vec4 color = vec4(0.0f);\n"
        "   color.a = 1.0f;\n"

        "   float h = 0.015f; \n"         // todo fix
        "   vec3 tr = hit;\n"
        "   vec3 rayStep = direction * h; \n"
        "   float alpha; \n"
        "   vec3 sampColor; \n"
        "   vec3 baseColor = vec3(1.0,1.0,1.0); \n"
       // " float opacityThreshold = 0.95f;"
        "   for(float t = 0.0f; t<=D; t += h){\n"
        "           float samp = texture(volume, tr).x;\n"

                    //calculate Alpha
        "           alpha = 1.0f - exp(-0.5f * samp);\n"

                    //accumulating collor and alpha using under operator
        "           sampColor = baseColor * alpha;\n"

        "           color.rgb += sampColor * color.a;\n"
        "           color.a *= 1.0f - samp ;\n"

                    // checking early ray termination
 //       "               if(1.0f - color.w > opacityThreshold) break;\n"

                    //increment ray step
        "           tr += rayStep;\n"
        "   }\n"
        "\n"
        "color.w = 1.0f - color.w;\n"
        "outColor = color;\n"
      //  "outColor = vec4(hit,1.0f);" // todo
        "}\n";

#endif //DATX02_20_21_SHADER_H
