#include "Engine.h"

#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <android/imagedecoder.h>

#include "AndroidUtils/AndroidOut.h"

Engine::Engine(android_app *pApp) :
        app_        (pApp),
        renderer    (*this, pApp),
        game        (*this, renderer.camera),
        audioManager (pApp)
{
    // Initialize the Sensor Manager and poll source
    sensorPollSource.id = LOOPER_ID_USER;
    sensorPollSource.app = pApp;
    sensorPollSource.process = Callback_OnSensorEvent;
    sensorManager = ASensorManager_getInstance();
    if(!sensorManager){
        aout<< "Unable to get Sensor Manager";
        return;
    }
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, pApp->looper,
            LOOPER_ID_USER, NULL, &sensorPollSource);
    // Intialize the Accelerometer
    accelerometer = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    if(!accelerometer){
        aout<< "Unable to get Accelerometer";
        return;
    }
    if(ASensorEventQueue_enableSensor(sensorEventQueue,accelerometer) < 0){
        aout << "Unable to enable Accelerometer";
        return;
    }
    int32_t minDelay = ASensor_getMinDelay(accelerometer);
    if(ASensorEventQueue_setEventRate(sensorEventQueue,accelerometer,minDelay) < 0){
        aout << "Unable to set Accelerometer Rate";
        return;
    }

    if(audioManager.start() == STATUS_KO){
        aout << "Failed to start audioManager";
    }
}

Engine::~Engine() {
    // Free the Accelerometer
    if(accelerometer){
        ASensorEventQueue_disableSensor(sensorEventQueue, accelerometer);
        accelerometer = nullptr;
    }
}


void Engine::render() {
    renderer.render();
}

void Engine::update(float deltaTime) {
    game.update(deltaTime);
}

GLuint Engine::getTextureId(std::string const& filepath) {
    return renderer.getTextureId(filepath);
}

void Engine::handleInput() {
    // handle all queued inputs
    auto *inputBuffer = android_app_swap_input_buffers(app_);
    if (!inputBuffer) {
        // no inputs yet.
        return;
    }

    // handle motion events (motionEventsCounts can be 0).
    for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
        auto &motionEvent = inputBuffer->motionEvents[i];
        auto action = motionEvent.action;

        // Find the pointer index, mask and bitshift to turn it into a readable value.
        auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        aout << "Pointer(s): ";

        // get the x and y position of this event if it is not ACTION_MOVE.
        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);

        // determine the action type and process the event accordingly.
        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                     << "Pointer Down";
                break;

            case AMOTION_EVENT_ACTION_CANCEL:
                // treat the CANCEL as an UP event: doing nothing in the app, except
                // removing the pointer from the cache if pointers are locally saved.
                // code pass through on purpose.
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                     << "Pointer Up";
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                // There is no pointer index for ACTION_MOVE, only a snapshot of
                // all active pointers; app needs to cache previous active pointers
                // to figure out which ones are actually moved.
                for (auto index = 0; index < motionEvent.pointerCount; index++) {
                    pointer = motionEvent.pointers[index];
                    x = GameActivityPointerAxes_getX(&pointer);
                    y = GameActivityPointerAxes_getY(&pointer);
                    aout << "(" << pointer.id << ", " << x << ", " << y << ")";

                    if (index != (motionEvent.pointerCount - 1)) aout << ",";
                    aout << " ";
                }
                aout << "Pointer Move";
                break;
            default:
                aout << "Unknown MotionEvent Action: " << action;
        }
        aout << std::endl;
    }
    // clear the motion input count in this buffer for main thread to re-use.
    android_app_clear_motion_events(inputBuffer);

    // handle input key events.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
        aout << "Key: " << keyEvent.keyCode <<" ";
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
                aout << "Key Down";
                break;
            case AKEY_EVENT_ACTION_UP:
                aout << "Key Up";
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                // Deprecated since Android API level 29.
                aout << "Multiple Key Actions";
                break;
            default:
                aout << "Unknown KeyEvent Action: " << keyEvent.action;
        }
        aout << std::endl;
    }
    // clear the key input count too.
    android_app_clear_key_events(inputBuffer);
}

void Engine::Callback_OnSensorEvent(android_app *pApp, android_poll_source *pSource) {
    Engine& engine{*(Engine*)pApp->userData};
    engine.OnSensorEvent();
}

void Engine::OnSensorEvent() {
    ASensorEvent event;
    while(ASensorEventQueue_getEvents(sensorEventQueue,&event,1) > 0)
        if(event.type == ASENSOR_TYPE_ACCELEROMETER)
            acceleration = glm::vec3{event.acceleration.x,event.acceleration.y, event.acceleration.z};
}

glm::vec3 Engine::GetAccelerometerAcceleration() const { return acceleration; }

AudioManager Engine::getAudioManager() {
    return audioManager;
}

void Engine::playAudio(const char *path, bool loopBool) {
    audioManager.playBGM(path, loopBool);
}
