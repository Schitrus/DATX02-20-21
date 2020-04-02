#version 310 es
precision highp float;
precision highp sampler3D;

in vec3 hit;

layout(binding = 0) uniform sampler2D lastHit;
layout(binding = 2) uniform sampler3D pressure;
layout(binding = 3) uniform sampler3D temperature;

// black-body radiation
out vec4 outColor;
const int LambdaSamples = 5;
const float wmin = 400.0f;
const float wmax = 700.0f;
const float dw = (wmax - wmin)/(float(LambdaSamples));

const mat3 M = mat3(
0.4f, 0.708f, -0.081f,
-0.226f, 1.165f, 0.046f,
0.0f, 0.0f, 0.918f
);
const mat3 inversM = mat3(
1.861f, -1.13098f, 0.220878,
0.361018f, 0.638969f, -0.000163571,
0.0f, 0.0f, 1.08932f);

const mat3 RGB = mat3(
3.2406f, -1.5372f, -0.4986f,
-0.9689f, 1.8758f, 0.0415,
0.0557f, -0.2040f, 1.0570f);

//todo RGB * inversM

/*
float xFit_1964(float lambda){
    float tmp1 = log((lambda+570.1f)/1014.0f);
    float tmp2 = log((1338.0f-lambda)/743.5f);
    return 0.398f * exp(-1250.0f * tmp1 * tmp1 ) + 1.132f * exp(-234.0f * tmp2 * tmp2 );
}
float yFit_1964(float lambda){
    float tmp = (lambda -556.1f)/46.14f;
    return 1.011 * exp(-0.5f * tmp * tmp );
}
float zFit_1964(float lambda){
    float tmp = log((lambda - 265.8f)/180.4f);
    return 2.06  * exp(-32.0f * tmp * tmp);
}
*/
float xFit_1931(float lambda){
    float tmp1 = (lambda-595.8f)/33.33f;
    float tmp2 = (lambda-446.8f)/19.44f;
    return 1.065f * exp(-0.5f * tmp1 * tmp1) + 0.366f * exp(-0.5f * tmp2 * tmp2);
}
float yFit_1931(float lambda){
    float tmp = (log(lambda) - log(556.3f))/0.075f;
    return 1.014 * exp(-0.5f * tmp * tmp);
}
float zFit_1931(float lambda){
    float tmp = (log(lambda) - log(449.8f))/0.051f;
    return 1.839  * exp(-0.5f * tmp * tmp);
}

vec3 gamma_correction(vec3 rgb){ //todo

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

float planks_formula(float lambda, float T){
    float C = 0.00000000000000037418;
    float Ct = 0.014388f;
    return (2.0f * C)/(pow(lambda, 5.0f) * (exp(Ct/(lambda * T)) - 1.0f));
}

vec3 radiance_to_XYZ(float RadList[LambdaSamples]){

    float w;
    float rad;
    vec3 XYZ = vec3(0.0f);

    for (int i = 0; i < LambdaSamples; i++){

        w = wmin + float(i) * dw;
        rad = RadList[i];

        XYZ.x += rad * xFit_1931(w);
        XYZ.y += rad * yFit_1931(w);
        XYZ.z += rad * zFit_1931(w);

    }
    return XYZ;
}

vec3 radiance_to_RGB(float RadList[LambdaSamples], vec3 maxLMS){

    vec3 XYZraw = radiance_to_XYZ(RadList);

    mat3 LMS = mat3(0.0f);
    LMS[0].x = 1.0f / maxLMS.x;
    LMS[1].y = 1.0f / maxLMS.y;
    LMS[2].z = 1.0f / maxLMS.z;


    vec3 XYZadapted =((XYZraw * M) * LMS) * inversM;

    vec3 rgb = (XYZadapted) * RGB;

    return clamp(gamma_correction(rgb), vec3(0.0f), vec3(1.0f));
}

float radiance(float L, float T, float lambda, float dx, float density){
    dx = dx * 1.0f;
   // float absorbtion = 0.05f * (density);                           // todo what value
    float absorbtion = 0.05f;                           // todo what value
    float scattering  = 0.0f;                           // todo
    float tot = absorbtion + scattering;

    lambda *= pow(10.0f, -9.0f);

    return exp(-tot * dx) * L + absorbtion * planks_formula(lambda, T) * dx;
}

float[LambdaSamples] black_body_radiation(float RadList[LambdaSamples], float T, float dx, float density){
    //T += 273.15f;

    float lambda = wmin;
    for(int i = 0; i < LambdaSamples; i++){
        RadList[i] = radiance(RadList[i], T, lambda, dx, density);
        lambda += dw;
    }
    return RadList;
}

vec3 maxLMS(float maxTemp){ // todo move
    float L[LambdaSamples];
    float wnm;
    for(int i = 0; i < LambdaSamples; i++){
        wnm = (wmin + float(i) * dw) * pow(10.0f, -9.0f);
        L[i] = planks_formula(wnm, maxTemp);
    }

    vec3 XYZ = radiance_to_XYZ(L);

    XYZ /= (XYZ.x + XYZ.y + XYZ.z);
    return XYZ * M;
}

void main() {
    ivec2 tcoord = ivec2(gl_FragCoord.xy);
    vec3 last = texelFetch(lastHit, tcoord, 0).xyz;
    vec3 direction = last.xyz - hit.xyz;
    float D = length(direction);
    direction = normalize(direction);
    vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    color.a = 0.0f;
    float h = 1.0/32.0;// todo fix
    vec3 tr = last;
    vec3 rayStep = -direction * h;


    float RadList[LambdaSamples];
    vec3 maxLMS = maxLMS(1500.0f); // todo ta in maxtemp


    for (float t = 0.0; t<=D; t+=h){
        ivec3 iv = ivec3(tr);

        float samp = clamp(texture(pressure, tr).x, 0.0, 1.0);

        float temp = texture(temperature, tr).x;

        RadList = black_body_radiation(RadList, temp, h, samp);

        float alpha = pow(samp,1.0);
        float over = color.a + alpha * (1.0 - color.a);

        color.a = over;

        tr += rayStep;
    }

    color.rgb = radiance_to_RGB(RadList, maxLMS);

    //debug
    if (any(isinf(color))){
          color.rgb = vec3(0.0f,1.0f,0.0f);
          color.a = 1.0f;
    }

    outColor = color;
   // outColor = vec4((maxLMS*inversM)*RGB,color.a);
   // outColor = vec4(1.0f,1.0f,1.0f,color.a);
}