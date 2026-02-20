//
// Created by Vernon on 18/2/2026.
//
#include "AudioManager.h"

AudioManager::AudioManager(android_app *pApplication):
        mAssetManager(pApplication->activity->assetManager),
        mEngineObj(nullptr),
        mEngine(nullptr),
        mOutputMixObj(nullptr),
        mBGMPlayerObj(nullptr),
        mBGMPlayer(nullptr),
        mBGMPlayerSeek(nullptr)
{}

AudioManager::~AudioManager() {
    stop();
}

status AudioManager::start() {
    SLresult result;

    //create engine
    result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //realize (allocate memory) the engine
    result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //get engine interface
    result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //create output mix
    result = (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, 0, nullptr, nullptr);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //realize output mix
    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    return STATUS_OK;
}

void AudioManager::stop() {
    stopBGM();

    //destroy output mix
    if(mOutputMixObj != nullptr){
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = nullptr;
    }
    //destroy engine
    if(mEngineObj != nullptr){
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
        mEngine = nullptr;
    }
}

ResourceDescriptor AudioManager::descriptor(const char *path) {
    ResourceDescriptor lDescriptor {-1,0,0};
    if(mAssetManager == nullptr){
        return lDescriptor;
    }
    //opens the asset using default setting
    AAsset* lAsset = AAssetManager_open(mAssetManager, path,AASSET_MODE_UNKNOWN);
    if(lAsset!= nullptr){
        lDescriptor.mDescriptor = AAsset_openFileDescriptor(lAsset, &lDescriptor.mStart, &lDescriptor.mLength);
        AAsset_close(lAsset);
    }
    return lDescriptor;
}

status AudioManager::playBGM(const char* path, bool loopBool) {
    SLresult result;

    //get ResourceDescriptor based on path
    ResourceDescriptor descriptor = AudioManager::descriptor(path);
    if(descriptor.mDescriptor < 0){
        return STATUS_KO;
    }

    //specifies to the engine where to find the audio file
    SLDataLocator_AndroidFD dataLocatorIn;
    dataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDFD;
    dataLocatorIn.fd = descriptor.mDescriptor;
    dataLocatorIn.offset = descriptor.mStart;
    dataLocatorIn.length = descriptor.mLength;

    //inform the engine what kind of audio data it is dealing with
    SLDataFormat_MIME dataFormat;
    dataFormat.formatType = SL_DATAFORMAT_MIME;
    dataFormat.mimeType = nullptr;
    dataFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    //describes the audio input channel
    SLDataSource dataSource;
    dataSource.pLocator = &dataLocatorIn;
    dataSource.pFormat = &dataFormat;

    //define the audio output destination
    SLDataLocator_OutputMix dataLocatorOut;
    dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    dataLocatorOut.outputMix = mOutputMixObj;

    //describes the audio output channel
    SLDataSink dataSink;
    dataSink.pLocator = &dataLocatorOut;
    dataSink.pFormat = nullptr;

    //create OpenSL ES audio player
    const SLuint32 bgmPlayerIIDCount = 2;
    const SLInterfaceID bgmPlayerIIDs[] = {
            SL_IID_PLAY, SL_IID_SEEK};
    const SLboolean bgmPlayerReqs[] =
            {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*mEngine)->CreateAudioPlayer
            (mEngine, &mBGMPlayerObj, &dataSource, &dataSink,
             bgmPlayerIIDCount, bgmPlayerIIDs, bgmPlayerReqs);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    // Realize the player
    result = (*mBGMPlayerObj)->Realize(mBGMPlayerObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //get player interface (play)
    result = (*mBGMPlayerObj)->GetInterface(mBGMPlayerObj, SL_IID_PLAY, &mBGMPlayer);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //get player interface (looping)
    result = (*mBGMPlayerObj)->GetInterface(mBGMPlayerObj,SL_IID_SEEK, &mBGMPlayerSeek);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //configure if player should loop
    result = (*mBGMPlayerSeek)->SetLoop(mBGMPlayerSeek, loopBool? SL_BOOLEAN_TRUE: SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    //set the player to play
    result = (*mBGMPlayer)->SetPlayState(mBGMPlayer, SL_PLAYSTATE_PLAYING);
    if(result != SL_RESULT_SUCCESS){
        return STATUS_KO;
    }

    return  STATUS_OK;
}

void AudioManager::stopBGM() {

    //Destroy player
    if(mBGMPlayer != nullptr){
        SLuint32 bgmPlayerState;
        (*mBGMPlayerObj)->GetState(mBGMPlayerObj, &bgmPlayerState);
        if(bgmPlayerState == SL_OBJECT_STATE_REALIZED){
            (*mBGMPlayer)->SetPlayState(mBGMPlayer, SL_PLAYSTATE_PAUSED);
            (*mBGMPlayerObj)->Destroy(mBGMPlayerObj);
            mBGMPlayerObj = nullptr;
            mBGMPlayer = nullptr;
            mBGMPlayerSeek = nullptr;
        }
    }
}
