#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_uiuc_gummyworm_GummyWorm_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "print this to the app thingy";

    return env->NewStringUTF(hello.c_str());
}