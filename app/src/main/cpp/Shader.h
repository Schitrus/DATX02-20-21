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
        "layout(binding = 0) uniform sampler2D data;\n"
        "out float outColor;\n"
        "void main() {\n"
        "   ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "   float value = texelFetch(data ,ivec2( tcoord.x - 1, tcoord.y),0).x;\n"
        "   value += texelFetch(data ,ivec2( tcoord.x + 1, tcoord.y),0).x;\n"
        "   value += texelFetch(data ,ivec2( tcoord.x, tcoord.y - 1),0).x;\n"
        "   value += texelFetch(data ,ivec2( tcoord.x, tcoord.y + 1),0).x;\n"
        "   outColor = value;\n"
        "}\n";

static const char BOUNDARY_FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision highp float;\n"
        "layout(binding = 0) uniform sampler2D data;\n"
        "uniform int width;\n"
        "uniform int height; \n"
        "out float outColor;\n"
        "void main() {\n"
        "   ivec2 tcoord = ivec2(gl_FragCoord.xy); \n"
        "   float value = 0.0f;\n"
        "    if(tcoord.x == 0){ value += texelFetch(data , ivec2( tcoord.x + 1, tcoord.y),0).x;}\n"
        "    if(tcoord.x == width -1){ value += texelFetch(data , ivec2( tcoord.x - 1, tcoord.y),0).x;}\n"
        "    if(tcoord.y == 0){ value += texelFetch(data , ivec2( tcoord.x, tcoord.y + 1 ),0).x;}\n"
        "    if(tcoord.y == height -1){ value += texelFetch(data , ivec2( tcoord.x, tcoord.y - 1),0).x;}\n"
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
        "layout(binding = 0) uniform sampler2D result;\n"
        "in vec2 texcoord;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    float q = texture(result, texcoord).x;\n"
        "    float c = q / 4.0f; "
        "    outColor = vec4(c, c, c, 1.0);\n"
        "}\n";


#endif //DATX02_20_21_SHADER_H
