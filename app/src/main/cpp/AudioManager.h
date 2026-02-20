//
// Created by Vernon on 18/2/2026.
//

#ifndef DOODLE_AUDIOMANAGER_H
#define DOODLE_AUDIOMANAGER_H

#include <stdint.h>
#include <sys/types.h>
#include <SLES/OpenSLES.h>
#include <android/asset_manager.h>
#include <SLES/OpenSLES_Android.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

struct android_app;

enum status{
    STATUS_OK = 0,
    STATUS_KO = -1
};

struct ResourceDescriptor{
    int32_t mDescriptor;
    off_t mStart;
    off_t mLength;
};

class AudioManager{
public:
    /*!
    * AudioManager constructor, initializes required variables
    */
    AudioManager(android_app* pApplication);
    /*!
    * AudioManager destructor, calls stop()
    */
    ~AudioManager();
    /*!
    * Returns the ResourceDescriptor that points to chosen file
    */
    ResourceDescriptor descriptor(const char* path);
    /*!
    * Initialize the OpenSL engine, output mixer, audio players
    */
    status start();
    /*!
    * Stop BGM
    */
    void stop();
    /*!
    * play BGM with the given filename
    */
    status playBGM(const char* path, bool loopBool);
    /*!
    * Stop BGM and destroy player
    */
    void stopBGM();
private:
    AAssetManager* mAssetManager;
    SLObjectItf mEngineObj;
    SLEngineItf mEngine;
    SLObjectItf mOutputMixObj;

    SLObjectItf mBGMPlayerObj;
    SLPlayItf mBGMPlayer;
    SLSeekItf mBGMPlayerSeek;
};
#endif //DOODLE_AUDIOMANAGER_H
