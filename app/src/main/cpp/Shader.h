//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_SHADER_H
#define DATX02_20_21_SHADER_H


static const char VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = 0) in vec3 pos;\n"

        "void main() {\n"
        "    gl_Position = vec4(pos, 1.0);\n"
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

static const char BOUNDARY_FRAGMENT_SHADER[] =
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
        "vec3 coords = vec3(texcoord, 0.0f);"
        "    float q = texture(result, coords).x;\n"
        "    float c = q / 6.0f; "
        "    outColor = vec4(c, c, c, 1.0);\n"
        "}\n";


#endif //DATX02_20_21_SHADER_H
