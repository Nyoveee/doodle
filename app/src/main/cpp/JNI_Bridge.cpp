#include "JNI_Bridge.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <jni.h>
#include "AndroidUtils/AndroidOut.h" // For logging
#include "Engine.h"

extern Engine* g_Engine;


void JNI_UpdateScore(android_app* app, int score) {
    if (!app || !app->activity || !app->activity->vm) {
        return;
    }

    JNIEnv* env = nullptr;
    JavaVM* vm = app->activity->vm;

    // 1. Attach current thread (Game Loop) to the Java VM
    jint result = vm->AttachCurrentThread(&env, nullptr);
    if (result != JNI_OK) {
        aout << "Failed to attach thread for JNI" << std::endl;
        return;
    }

    // 2. Get the Activity instance
    jobject activityCallback = app->activity->javaGameActivity;

    // 3. Find our MainActivity class
    jclass activityClass = env->GetObjectClass(activityCallback);

    // 4. Get the Kotlin method ID: fun setScore(score: Int)
    // Signature (I)V means takes Integer, returns Void
    jmethodID methodId = env->GetMethodID(activityClass, "setScore", "(I)V");

    if (methodId != nullptr) {
        // 5. Call the method
        env->CallVoidMethod(activityCallback, methodId, score);
    } else {
        // This often happens if the method name/signature in Kotlin doesn't match exactly
        // clearing exception is important so app doesn't crash on next JNI call
        env->ExceptionClear();
        aout << "Could not find method setScore in MainActivity" << std::endl;
    }

    // 6. Clean up references
    env->DeleteLocalRef(activityClass);

    // 7. Detach thread (Crucial for performance and stability)
    vm->DetachCurrentThread();
}

void JNI_GameOver(android_app* app, int score) {
    if (!app || !app->activity || !app->activity->vm) return;

    JNIEnv* env = nullptr;
    JavaVM* vm = app->activity->vm;

    if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;

    jobject activityCallback = app->activity->javaGameActivity;
    jclass activityClass = env->GetObjectClass(activityCallback);

    // Look for "gameOver" instead of "setScore"
    jmethodID methodId = env->GetMethodID(activityClass, "gameOver", "(I)V");

    if (methodId != nullptr) {
        env->CallVoidMethod(activityCallback, methodId, score);
    } else {
        env->ExceptionClear();
        aout << "Could not find method gameOver in MainActivity" << std::endl;
    }

    env->DeleteLocalRef(activityClass);
    vm->DetachCurrentThread();
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_doodle_MainActivity_restartGameNative(JNIEnv *env, jobject thiz) {
    if (g_Engine) {
        // Now this works because we included Engine.h and extern g_Engine
        g_Engine->game.ResetGame();
    }
}

JNIEXPORT void JNICALL
Java_com_example_doodle_MainActivity_startGameNative(JNIEnv *env, jobject thiz) {
    if (g_Engine) {
    // Now this works because we included Engine.h and extern g_Engine
    g_Engine->game.StartGame();
    }
}

}

void Java_com_example_doodle_MainActivity_playMenuBGM(JNIEnv *env, jobject thiz) {
    if(g_Engine){
        g_Engine->playAudio("menuBGM.mp3", true);
    }
}
