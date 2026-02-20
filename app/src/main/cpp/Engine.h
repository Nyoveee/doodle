#ifndef ANDROIDGLINVESTIGATIONS_RENDERER_H
#define ANDROIDGLINVESTIGATIONS_RENDERER_H

#include <memory>
#include <android/sensor.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>
#include "Game/DoodleGame.h"
#include "Graphics/Renderer.h"
#include "AudioManager.h"

#define LOG_TAG "DoodleEngine" // This is the 'Tag' you will search for in Logcat
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using GLuint = unsigned int;

struct android_app;

class Engine {
public:
    /*!
     * @param pApp the android_app this Engine belongs to, needed to configure GL
     */
    Engine(android_app *pApp);

    ~Engine();
    /*!
     * Handles input from the android_app.
     *
     * Note: this will clear the input queue
     */
    void handleInput();

    /*!
     * Renders all the models in the renderer
     */
    void render();

    /*!
     * Game's main update function
     */
    void update(float deltaTime);

    GLuint getTextureId(std::string const& filepath);

    // Data from Gyroscope
    glm::vec3 GetAccelerometerAcceleration() const;
private:
    static void Callback_OnSensorEvent(android_app* pApp,android_poll_source* pSource);
    void OnSensorEvent();

public:
    android_app *app_;              // reference to the original android app.
    Renderer renderer;              // responsible for graphics
    DoodleGame game;                // holds all the game objects and are in charge of their logic.
    AudioManager getAudioManager();
    void playAudio(const char* path, bool loopBool);
private:
    // Sensor Variables
    ASensorManager* sensorManager;
    ASensorEventQueue* sensorEventQueue;
    android_poll_source sensorPollSource;
    const ASensor* accelerometer;
    glm::vec3 acceleration;
    AudioManager audioManager;
};

#endif //ANDROIDGLINVESTIGATIONS_RENDERER_H