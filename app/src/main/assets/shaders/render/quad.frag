#version 310 es

precision highp float;
precision highp sampler2D;

layout(binding = 0) uniform sampler2D LMS;
layout(binding = 2) uniform sampler2D max_LMS_tex;

in vec2 texCoord;

out vec4 outColor;

const mat3 inversM = mat3(
1.86007f, -1.12948f, 0.219898f,
0.361223f, 0.638804f, -0.0000071275f,
0.0f, 0.0f, 1.08909f
);

const mat3 M = mat3(
0.4002f, 0.7076f, -0.0808f,
-0.2263f, 1.1653f, 0.0457f,
0.0f, 0.0f, 0.9182f
);

const mat3 RGB = mat3(
3.2406f, -1.5372f, -0.4986f,
-0.9689f, 1.8758f, 0.0415,
0.0557f, -0.2040f, 1.0570f);

vec3 gamma_correction(vec3 rgb){

    for (int i = 0;  i < 3; i++){
        if (rgb[i] <= 0.00304f){
            rgb[i] = 12.92f * rgb[i];
        } else {
            rgb[i] = 1.055f * pow(rgb[i], 1.0f / 2.4f) -0.055f;
        }
    }
    //return pow( rgb.xyz, vec3(0.454545f));
    return rgb;
}

vec3 XYZ_to_RGB(vec3 XYZraw, vec3 maxLMS){

    mat3 LMS = mat3(0.0f);

    LMS[0].x = 1.0f / (maxLMS.x);
    LMS[1].y = 1.0f / (maxLMS.y);
    LMS[2].z = 1.0f / (maxLMS.z);

    vec3 XYZadapted =((XYZraw * M) * LMS) * inversM;

    vec3 rgb = ((XYZadapted)) * RGB;

    return clamp(gamma_correction(rgb), vec3(0.0f), vec3(1.0f));
}

void main() {

    vec4 max_LMS = texelFetch(max_LMS_tex, ivec2(0, 0), 0);

    vec4 color = texture(LMS, texCoord).rgba;

    color.rgb = XYZ_to_RGB(color.rgb, max_LMS.xyz);

    outColor = color;
}