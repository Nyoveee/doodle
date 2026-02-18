//
// Created by 10988 on 19/02/2026.
//

#include <android/native_window_jni.h>
#include "JNI_Bridge.h"
#include "Engine.h"

extern Engine* g_engine; // forward-declared in main.cpp as static; declare extern here

extern "C" {

JNIEXPORT jlong JNICALL Java_com_example_doodle_NativeBridge_nativeGetEnginePtr(JNIEnv* env, jobject thiz) {
    // Return pointer to the current Engine if available, otherwise 0.
    if (g_engine) return reinterpret_cast<jlong>(g_engine);
    return 0;
}

JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onSurfaceCreated(JNIEnv *env, jobject thiz, jobject surface) {
    if (g_engine) {
        ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
        g_engine->renderer.setWindow(window);
    }
}

JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onSurfaceDestroyed(JNIEnv *env, jobject thiz) {
    if (g_engine) {
        g_engine->renderer.setWindow(nullptr);
    }
}

JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onTouchEvent(JNIEnv *env, jobject thiz, jint action, jfloat x, jfloat y) {
    if (g_engine) {
        g_engine->pushTouchEvent(action, x, y);
    }
}


} // extern "C"
