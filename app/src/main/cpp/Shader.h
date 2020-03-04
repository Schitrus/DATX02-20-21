//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H

static const char SLAB_VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"

        "void main() {\n"
        "    gl_Position = vec4(pos, 1.0);\n"
        "}\n";

static const char RAY_VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"
        "uniform mat4 mvp;\n"
        "out vec3 hit;\n"

        "void main() {\n"
        "    gl_Position = mvp * vec4(pos, 1.0);\n"
        "    hit = pos;\n"
        "}\n";

static const char INTERIOR_FRAGMENT_SHADER[] =
        "#version 310 es\n"

        "precision highp float;\n"
        "precision highp sampler3D;\n"

        "layout(binding = 0) uniform sampler3D data;\n"
        "uniform int depth; \n"
        "out float outColor;\n"

        "void main() {\n"
        "   ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"

        "   float value = texelFetch(data ,ivec3( tcoord.x - 1, tcoord.y, depth),0).x;\n"
        "   value += texelFetch(data ,ivec3( tcoord.x + 1, tcoord.y, depth),0).x;\n"
        "   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y - 1, depth),0).x;\n"
        "   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y + 1, depth),0).x;\n"
        "   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth +1 ),0).x;\n"
        "   value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth -1 ),0).x;\n"

        "   outColor = value;\n"
        "}\n";

static const char FRONT_AND_BACK_INTERIOR_FRAGMENT_SHADER[] =
        "#version 310 es\n"

        "precision highp float;\n"
        "precision highp sampler3D;\n"

        "layout(binding = 0) uniform sampler3D data;\n"
        "uniform int depth; \n"
        "out float outColor;\n"

        "void main() {\n"
        "   int dir = depth == 0 ? 1 : -1; \n"   // todo remove if statement
        "   ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "   float value = texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth + dir),0).x;\n"
        "   outColor = value;\n"
        "}\n";

static const char FRONT_AND_BACK_BOUNDARY_FRAGMENT_SHADER[] = // todo remove all if statement
        "#version 310 es\n"
        "precision highp float;\n"
        "precision highp sampler3D;\n"

        "layout(binding = 0) uniform sampler3D data;\n"

        "uniform int width;\n"
        "uniform int height; \n"
        "uniform int depth; \n"
        "uniform float scale; \n"

        "out float outColor;\n"

        "void main() {\n"
        "    int dir = depth == 0 ? 1 : -1; \n"   // todo remove if statement
        "    ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "    float value = 0.0f;\n"

        "    if(tcoord.x == 0){ \n"
        "    value += 0.5 * scale * ( texelFetch(data , ivec3( tcoord.x + 1, tcoord.y, depth),0) + texelFetch(data , ivec3( tcoord.x , tcoord.y, depth + dir),0) );\n"
        "    }\n"
        "    if(tcoord.x == width -1){ \n"
        "    value += 0.5 * scale * ( texelFetch(data , ivec3( tcoord.x - 1, tcoord.y, depth),0) + texelFetch(data , ivec3( tcoord.x , tcoord.y, depth + dir),0) );\n"
        "    }\n"
        "    if(tcoord.y == 0){\n"
        "    value += 0.5 * scale * ( texelFetch(data , ivec3( tcoord.x, tcoord.y + 1, depth),0) + texelFetch(data , ivec3( tcoord.x , tcoord.y, depth + dir),0) );\n"
        "    }\n"
        "    if(tcoord.y == height -1){ \n"
        "    value += 0.5 * scale * ( texelFetch(data , ivec3( tcoord.x, tcoord.y -1, depth),0) + texelFetch(data , ivec3( tcoord.x , tcoord.y, depth + dir),0) );\n"
        "    }\n"

        "    bool corner = ((tcoord.x == 0 || tcoord.x == width -1) && (tcoord.y == 0 || tcoord.y == height -1 )); \n"
        "    if(corner){ \n"
        "        value *= 2.0f; \n"
        "        value -= scale * texelFetch(data , ivec3( tcoord.x , tcoord.y, depth + dir),0);\n"
        "        value *= 0.3333333333333333f;\n"
        "     } \n"

        "    outColor = value;\n"
        "}\n";

static const char BOUNDARY_FRAGMENT_SHADER[] = // todo remove all if statement
        "#version 310 es\n"
        "precision highp float;\n"
        "precision highp sampler3D;\n"

        "layout(binding = 0) uniform sampler3D data;\n"

        "uniform int width;\n"
        "uniform int height; \n"
        "uniform int depth; \n"
        "uniform float scale; \n"

        "out float outColor;\n"

        "void main() {\n"
        "    ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"

        "    float value = 0.0f;\n"
        "    if(tcoord.x == 0){ value += scale * texelFetch(data , ivec3( tcoord.x + 1, tcoord.y, depth),0).x;}\n"
        "    if(tcoord.x == width -1){ value += scale * texelFetch(data , ivec3( tcoord.x - 1, tcoord.y, depth),0).x;}\n"
        "    if(tcoord.y == 0){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y + 1, depth ),0).x;}\n"
        "    if(tcoord.y == height -1){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y - 1, depth),0).x;}\n"
        "    bool corner = ((tcoord.x == 0 || tcoord.x == width -1) && (tcoord.y == 0 || tcoord.y == height -1 )); \n"

        "    if(corner){ value *= 0.5f; }"

        "    outColor = value;\n"
        "}\n";

static const char RESULTS_VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"
        "layout(location = 2) in vec2 tex;\n"
        "out vec2 texcoord; \n"
        "void main() {\n"
        "    texcoord =  tex;  "
        "    gl_Position = vec4(pos, 1.0);\n"
        "}\n";

static const char RESULTS_FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision highp float;\n"
        "precision highp sampler3D;\n"
        "layout(binding = 0) uniform sampler3D result;\n"
        "in vec2 texcoord;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "vec3 coords = vec3(texcoord, 0.5f);"
        "    float q = texture(result, coords).x;\n"
        "    float c = q / 1.0f; "
        "    outColor = vec4(c, c, c, 1.0);\n"
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
        "}\n";


#endif //DATX02_20_21_SHADER_H
