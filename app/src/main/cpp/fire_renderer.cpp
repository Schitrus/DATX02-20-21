#include <jni.h>
#include <string>

#include <time.h>
#include <math.h>
#include <chrono>

#include <stdlib.h>
#include <utility>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

void initGraphics(int width, int height);
void updateGraphics();
void touch(double dx, double dy);
void scale(float scaleFactor, double scaleX, double scaleY);

extern "C" {
JNIEXPORT jstring JNICALL Java_com_example_datx02_120_121_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_init(
        JNIEnv *env,
        jobject,
        jint width,
        jint height);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_update(
        JNIEnv *env,
        jobject);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireListener_touch(
        JNIEnv *env,
        jobject,
        jdouble dx,
        jdouble dy);
JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireListener_scale(
        JNIEnv *env,
        jobject,
        jfloat scaleFactor,
        jdouble scaleX,
        jdouble scaleY);
};

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireListener_scale(
        JNIEnv *env,
        jobject,
        jfloat scaleFactor,
        jdouble scaleX,
        jdouble scaleY){
    scale(scaleFactor, scaleX, scaleY);
}

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireListener_touch(
        JNIEnv *env,
        jobject,
        jdouble dx,
        jdouble dy){
    touch(dx, dy);

}

JNIEXPORT jstring JNICALL Java_com_example_datx02_120_121_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */){
    std::string hello;
    for (int i = 0; i < 10; i++)
        hello += "Hello from C++ and Kalle again! ";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_init(
        JNIEnv *env,
        jobject,
        jint width,
        jint height){
    initGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_example_datx02_120_121_FireRenderer_update(
        JNIEnv *env,
        jobject){
    updateGraphics();
}

float vertices[] = {
         1.0f,  1.0f, 0.0f,   // top right
         1.0f, -1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f    // top left
};

float texCoords[] = {
        1.0f,  1.0f,
        1.0f,  0.0f,
        0.0f,  0.0f,
        0.0f,  1.0f
};

unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

unsigned int positionVBO, textureVBO;
unsigned int VAO, EBO;

unsigned int vertexShader;
const char* vertex = "#version 310 es\n"
                     "layout(location = 0) in vec3 pos;"
                     "layout(location = 1) in vec2 uv;"
                     "out vec2 tex;"
                     "void main() {"
                     "  tex = uv;"
                     "  gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
                     "}";

unsigned int fragmentShader;
const char* fragment = "#version 310 es\n"
                       "precision highp float;"
                       "out vec4 outColor;"
                       "in vec2 tex;"
                       "uniform sampler2D image;"
                       "uniform vec2 pos;"
                       "uniform float scale;"
                       "vec2 mandel(vec2 z, vec2 c){"
                       "    float Re = z.x*z.x - z.y*z.y + c.x;"
                       "    float Im = 2.0*z.x*z.y + c.y;"
                       "    return vec2(Re, Im);"
                       "}"
                       "vec3 mandelbrot(vec2 c, int level){"
                       "    vec2 z = vec2(0.0, 0.0);"
                       "    for(int i = 0; i < level; i++){"
                       "        z = mandel(z, c);"
                       "        if(length(z) > 2.0)"
                       "            return vec3(0.5*sin(float(i)*0.1)+0.5,0.5*sin(float(i)*0.07)+0.5,0.5*sin(float(i)*0.08)+0.5);"
                       "    }"
                       "    return vec3(0.0);"
                       "}"
                       "void main() {"
                       "    ivec2 dim = textureSize(image, 0);"
                       "    float ratio = float(dim.x)/float(dim.y);"
                       "    float dx = 1.0/float(dim.x);"
                       "    float dy = 1.0/float(dim.y);"
                       "    vec2 c = vec2((1.0 - tex.y + pos.x)*3.0 - 2.0, (tex.x + pos.y) * 2.0 - 1.0)/scale;"
                       //"    vec2 c0 = vec2((1.0 - (tex.y + 0.125*dy) + pos.x)*3.0 - 2.0, (tex.x + 0.375*dx + pos.y) * 2.0 - 1.0)/scale;"
                       //"    vec2 c1 = vec2((1.0 - (tex.y - 0.375*dy) + pos.x)*3.0 - 2.0, (tex.x + 0.125*dx + pos.y) * 2.0 - 1.0)/scale;"
                       //"    vec2 c2 = vec2((1.0 - (tex.y + 0.375*dy) + pos.x)*3.0 - 2.0, (tex.x - 0.125*dy + pos.y) * 2.0 - 1.0)/scale;"
                       //"    vec2 c3 = vec2((1.0 - (tex.y - 0.125*dy) + pos.x)*3.0 - 2.0, (tex.x - 0.375*dy + pos.y) * 2.0 - 1.0)/scale;"
                       "    int level = int(16.0 + 16.0*log2(scale));"
                       //"    vec3 val = mandelbrot(c0, level) + mandelbrot(c1, level) + mandelbrot(c2, level) + mandelbrot(c3, level);"
                       "    vec3 val = mandelbrot(c, level);"
                       "    outColor = vec4(val, 1.0);"
                       "}";


unsigned int shaderProgram;

unsigned int currentTexture;
unsigned int nextTexture;

unsigned int texWidth, texHeight;
unsigned char* data;

unsigned int FBO;

unsigned int window_width, window_height;

bool fail = false;

double posX = 0.0f;
double posY = 0.0f;

float zoom = 1.0f;

unsigned int pos_index;
unsigned int scale_index;

void initGraphics(int width, int height){

    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);

    ///////////////
    /// Shaders //////
    /////////////////////
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);
    int compile;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compile);
    if(!compile)
        fail = true;

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compile);
    if(!compile)
        fail = true;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /// Uniforms

    pos_index = glGetUniformLocation(shaderProgram, "pos");
    scale_index = glGetUniformLocation(shaderProgram, "scale");

    ///////////////
    /// Buffers //////
    /////////////////////

    /// Vertex Buffer Object (VBO)

    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    /// Vertex Array Object (VAO)

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    /// Element Buffer Object (EBO)

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ////////////////
    /// Textures //////
    //////////////////////

    /// NOTE: For pixel alignment
    /// Took me an hour to figure out why the image generated looked weird, this was it, set to 4 or 8 for some weird as reason
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &currentTexture);
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    double ratio = (double)width/height;
    texWidth = width/4; texHeight = height/4;
    unsigned int colorWidth = 3;
    data = new unsigned char[texWidth*texHeight*colorWidth];
    for(int y = 0; y < texHeight; y++){
        for(int x = 0; x < texWidth; x++){
            data[(y * texWidth + x) * colorWidth + 0] = 255*(y/float(texHeight));
            data[(y * texWidth + x) * colorWidth + 1] = 0;
            data[(y * texWidth + x) * colorWidth + 2] = 255*(x/float(texWidth));
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    ///
    /// Framebuffers
    ///
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &nextTexture);
    glBindTexture(GL_TEXTURE_2D, nextTexture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

}

#define PI 3.14159265359
using namespace std::chrono;
double last_sec = duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count()/1000.0;

void updateGraphics(){
    // Time

    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    double sec = ms.count()/1000.0;
    // Background
    glClearColor(0.5f*sin(sec)+0.5f, 0.5f*sin(sec+(2*PI/3))+0.5f, 0.5f*sin(sec+(4*PI/3))+0.5f, 1.0f);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glUniform2f(pos_index, -posY, -posX);
    glUniform1f(scale_index, zoom);

    bool step = false;

    if(last_sec + 0.016 < sec) {
        last_sec = sec;
        step = true;
    }

    // Draw Quad
    glViewport(0, 0, window_width, window_height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, currentTexture);
    if(!fail)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw FBO
    if(step) {
        glViewport(0, 0, texWidth, texHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextTexture, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap
        unsigned int tmp = currentTexture;
        currentTexture = nextTexture;
        nextTexture = tmp;
    }
}

void scale(float scaleFactor, double scaleX, double scaleY){
    posX = (posX)*(scaleFactor / zoom);//(2*scaleX/window_width-1) * (zoom - scaleFactor);
    posY = (posY)*(scaleFactor / zoom);//(2*scaleY/window_height-1) * (zoom - scaleFactor);
    zoom = scaleFactor;
}

void touch(double dx, double dy){
    posX += dx/window_width; posY += dy/window_height;
}