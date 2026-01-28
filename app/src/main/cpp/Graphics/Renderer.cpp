//
// Created by Nyove on 1/26/2026.
//

#include "Renderer.h"
#include "../Game/GameObject/GameObject.h"
#include "../AndroidUtils/AndroidOut.h"
#include "../Engine.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <android/imagedecoder.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::initRenderer() {
    // Choose your render attributes
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    // The default display is probably what you want on Android
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // figure out how many configs there are
    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    // get the list of configurations
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    // Find a config we like.
    // Could likely just grab the first if we don't care about anything else in the config.
    // Otherwise hook in your own heuristic
    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    aout << "Found config with " << red << ", " << green << ", " << blue << ", "
                         << depth << std::endl;
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

    aout << "Found " << numConfigs << " configs" << std::endl;
    aout << "Chose " << config << std::endl;

    // create the proper window surface
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    EGLSurface surface = eglCreateWindowSurface(display, config, app_->window, nullptr);

    // Create a GLES 3 context
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

    // get some window metrics
    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    assert(madeCurrent);

    display_ = display;
    surface_ = surface;
    context_ = context;

    // make width and height invalid so it gets updated the first frame in @a updateRenderArea()
    width_ = -1;
    height_ = -1;

    PRINT_GL_STRING(GL_VENDOR);
    PRINT_GL_STRING(GL_RENDERER);
    PRINT_GL_STRING(GL_VERSION);
    PRINT_GL_STRING_AS_LIST(GL_EXTENSIONS);

    mainShader = std::unique_ptr<Shader>(
            Shader::loadShader("main.vert", "main.frag", app_->activity->assetManager));
    assert(mainShader);

    // Note: there's only one shader in this demo, so I'll activate it here. For a more complex game
    // you'll want to track the active shader and activate/deactivate it as necessary
    mainShader->activate();

    // setup any other gl related global states
    glClearColor(CORNFLOWER_BLUE);

    // enable alpha globally for now, you probably don't want to do this in a game
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialise camera..
    camera.position = {0, 0};
    updateRenderArea();

    // initialise none texture..
    auto assetManager = app_->activity->assetManager;
    noneTexture = TextureAsset::loadAsset(assetManager, "None.png");
}

void Renderer::updateRenderArea() {
    EGLint width;
    eglQuerySurface(display_, surface_, EGL_WIDTH, &width);

    EGLint height;
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);

    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);
        camera.scale = {width_, height_ };
    }
}

void Renderer::render() {
    // Check to see if the surface has changed size. This is _necessary_ to do every frame when
    // using immersive mode as you'll get no other notification that your renderable area has
    // changed.
    updateRenderArea();

    // get camera's projection matrix.. (camera will always be moving, no point lazy calculating it..)
    mainShader->setMatrix("viewProjection", camera.getViewProjection()) ;
    mainShader->setImageUniform("uTexture", 0);

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Render all game objects.
    for(auto& gameObjectPtr : engine.game.getGameObjects()) {
        GameObject& gameObject = *gameObjectPtr;

        // Setup the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gameObject.textureId != NO_TEXTURE ? gameObject.textureId : noneTexture->getTextureID());

        mainShader->setMatrix("model", calculateModelMatrix(gameObject));
        mainShader->setVec4("colorMultiplier", gameObject.colorMultiplier);

        // VBO-less draw.
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Present the rendered image. This is an implicit glFlush.
    auto swapResult = eglSwapBuffers(display_, surface_);
    assert(swapResult == EGL_TRUE);
}

glm::mat4 Renderer::calculateModelMatrix(GameObject const& gameObject) {
    glm::mat4 modelMatrix { 1.f };

    modelMatrix = glm::translate(modelMatrix, glm::vec3{ gameObject.position, 0.f });
    modelMatrix = glm::rotate(modelMatrix, glm::radians(gameObject.rotation), {0.0f, 0.0f, 1.0f});     // Because this is 2D, we rotate in the Z-axis.
    modelMatrix = glm::scale(modelMatrix, glm::vec3{ gameObject.scale, 1.f });

    return modelMatrix;
}

GLuint Renderer::getTextureId(std::string const& filepath) {
    // First we check if this texture is already loaded..
    auto iterator = textureFilepathToId.find(filepath);

    // This texture is already loaded..
    if(iterator != textureFilepathToId.end()) {
        return iterator->second;
    }

    // Not loaded, let's attempt to load it.
    auto assetManager = app_->activity->assetManager;
    auto texture = TextureAsset::loadAsset(assetManager, filepath);

    // loading successful..
    if(texture) {
        GLuint textureId = texture->getTextureID();
        textures.push_back(std::move(texture)); // move ownership to from local variable to renderer
        return textureId;
    }
    else {
        aout << "Failed to load texture: " << filepath << std::endl;
        return NO_TEXTURE;
    }
}
Renderer::~Renderer() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
            context_ = EGL_NO_CONTEXT;
        }
        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
            surface_ = EGL_NO_SURFACE;
        }
        eglTerminate(display_);
        display_ = EGL_NO_DISPLAY;
    }
}

