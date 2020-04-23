#version 310 es
precision highp float;
precision highp sampler3D;

in vec3 hit;

layout(binding = 0) uniform sampler2D lastHit;
layout(binding = 2) uniform sampler3D pressure;
layout(binding = 3)  uniform sampler3D temperature;


// black-body radiation
out vec4 outColor;
const int LambdaSamples = 5;
const float wmin = 400.0f;
const float wmax = 700.0f;
const float dw = (wmax - wmin)/(float(LambdaSamples));

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


float xDFit_1931(float wave){
    float t1 = (wave-442.0f)*((wave<442.0f)?0.0624f:0.0374f);
    float t2 = (wave-599.8f)*((wave<599.8f)?0.0264f:0.0323f);
    float t3 = (wave-501.1f)*((wave<501.1f)?0.0490f:0.0382f);
    return 0.362f*exp(-0.5f*t1*t1) + 1.056f*exp(-0.5f*t2*t2) - 0.065f*exp(-0.5f*t3*t3);
}
float yDFit_1931(float wave){
    float t1 = (wave-568.8f)*((wave<568.8f)?0.0213f:0.0247f);
    float t2 = (wave-530.9f)*((wave<530.9f)?0.0613f:0.0322f);
    return 0.821f*exp(-0.5f*t1*t1) + 0.286f*exp(-0.5f*t2*t2);
}
float zDFit_1931(float wave){
    float t1 = (wave-437.0f)*((wave<437.0f)?0.0845f:0.0278f);
    float t2 = (wave-459.0f)*((wave<459.0f)?0.0385f:0.0725f);
    return 1.217f*exp(-0.5f*t1*t1) + 0.681f*exp(-0.5f*t2*t2);
}

float planks_formula(float lambda, float T){
    //float C = 0.00000000000000037418;
    float C = 3.7418 * pow(10.0f, -16.0f);
    // float Ct = 0.014388f;
    float Ct = 1.4388f * pow(10.0f, -2.0f);
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
    // XYZ *= dw;
    XYZ *= dw * pow(10.0f, -9.0f);
    //XYZ /= XYZ.y;
    //  XYZ = clamp(XYZ, vec3(0.0f), vec3(1.0f));
    /*float falloff = exp(-distance);
    for (int i = 0; i < LambdaSamples; i++){
        RadList[i] *= falloff;
    }
*/
    //XYZ = clamp(XYZ, vec3(0.0f), vec3(255.0f));
    return XYZ;
}


float radiance(float L, float T, float lambda, float dx, float density){
    dx = dx * 1.0f;
    // float absorbtion = 0.05f * (density);                           // todo what value
    //float absorbtion = 1.0f;                           // todo what value
    float absorbtion = 1.0f * density;// todo what value
    float scattering  = 0.0f;// todo
    float tot = absorbtion + scattering;

    lambda *= pow(10.0f, -9.0f);

    return exp(-tot * dx) * L +1.0f* absorbtion * planks_formula(lambda, T) * dx;
}

float[LambdaSamples] black_body_radiation(float RadList[LambdaSamples], float T, float dx, float density){

    float lambda = wmin;
    for (int i = 0; i < LambdaSamples; i++){
        RadList[i] = radiance(RadList[i], T, lambda, dx, density);
        lambda += dw;
    }
    return RadList;
}

void main() {

    ivec2 tcoord = ivec2(gl_FragCoord.xy);
    vec3 last = texelFetch(lastHit, tcoord, 0).xyz;
    vec3 direction = last.xyz - hit.xyz;
    float D = length(direction);
    direction = normalize(direction);
    vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    color.a = 0.0f;
    float h = 1.0/42.0;// todo fix
    vec3 tr = last;
    vec3 rayStep = -direction * h;

    float RadList[LambdaSamples];

    for (float t = 0.0; t<=D; t+=h){
        ivec3 iv = ivec3(tr);

        float alpha = clamp(texture(pressure, tr).x, 0.0, 1.0);
        //float alpha = 0.5;

        float temp = texture(temperature, tr).x;
        //float temp = 2000.0f;

        RadList = black_body_radiation(RadList, temp, h, alpha);

        alpha = pow(alpha, 2.0);

        float over = alpha + color.a * (1.0 - alpha);

        color.a = over;

        tr += rayStep;
    }

    color.xyz = radiance_to_XYZ(RadList);

    outColor = color;
}
