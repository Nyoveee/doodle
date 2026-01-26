#ifndef ANDROIDGLINVESTIGATIONS_RENDERER_H
#define ANDROIDGLINVESTIGATIONS_RENDERER_H

#include <memory>

#include "Game/DoodleGame.h"
#include "Graphics/Renderer.h"

using GLuint = unsigned int;

struct android_app;

class Engine {
public:
    /*!
     * @param pApp the android_app this Engine belongs to, needed to configure GL
     */
    explicit Engine(android_app *pApp) :
            app_        (pApp),
            renderer    (*this, pApp),
            game        (*this, renderer.camera) {}

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

public:
    android_app *app_;              // reference to the original android app.
    Renderer renderer;              // responsible for graphics
    DoodleGame game;                // holds all the game objects and are in charge of their logic.
};

#endif //ANDROIDGLINVESTIGATIONS_RENDERER_H