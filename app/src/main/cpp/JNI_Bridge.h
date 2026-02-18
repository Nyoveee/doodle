//
// Created by 10988 on 19/02/2026.
//

#ifndef DOODLE_JNI_BRIDGE_H
#define DOODLE_JNI_BRIDGE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_example_doodle_NativeBridge_nativeGetEnginePtr(JNIEnv* env, jobject thiz);
JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onSurfaceCreated(JNIEnv *env, jobject thiz, jobject surface);
JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onSurfaceDestroyed(JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL Java_com_example_doodle_NativeBridge_onTouchEvent(JNIEnv *env, jobject thiz, jint action, jfloat x, jfloat y);

#ifdef __cplusplus
}
#endif

#endif //DOODLE_JNI_BRIDGE_H
