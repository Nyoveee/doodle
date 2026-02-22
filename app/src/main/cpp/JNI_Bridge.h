#ifndef DOODLE_JNI_BRIDGE_H
#define DOODLE_JNI_BRIDGE_H

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <jni.h>




// Engine to UI Helper Functions
void JNI_UpdateScore(android_app* app, int score);
void JNI_GameOver(android_app* app, int score);

// UI to Engine (The JNI Export Declaration)
extern "C" {
JNIEXPORT void JNICALL
Java_com_example_doodle_MainActivity_startGameNative(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_example_doodle_MainActivity_playMenuBGM(JNIEnv *env, jobject thiz);
}

#endif //DOODLE_JNI_BRIDGE_H
