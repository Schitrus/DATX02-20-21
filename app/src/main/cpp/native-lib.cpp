#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_datx02_120_121_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello;
    for(int i = 0; i< 10; i++)
        hello += "Hello from C++ and Kalle again! ";
    return env->NewStringUTF(hello.c_str());
}
