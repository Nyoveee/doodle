//
// Created by Nyove on 1/26/2026.
//

#include "Renderer.h"
#include "../Game/GameObject/GameObject.h"
#include "../AndroidUtils/AndroidOut.h"
#include "../Engine.h"

#include <android/native_window.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <android/imagedecoder.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Constructor / Destructor
Renderer::Renderer(Engine& engine) :
        engine(engine),
        window_(nullptr),
        display_(EGL_NO_DISPLAY),
        surface_(EGL_NO_SURFACE),
        context_(EGL_NO_CONTEXT),
        width_(0),
        height_(0),
        shaderNeedsNewProjectionMatrix_(true),
        assetManager_(nullptr) {
}

Renderer::~Renderer() {
    // Ensure EGL and window are cleaned up
    terminateEGL();
    if (window_) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }
}

void Renderer::initEGL() {
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

    // Use the stored window_ pointer (set from JNI). If not present, fail init.
    if (!window_) {
        aout << "initEGL called but no window_ available" << std::endl;
        return;
    }

    EGLSurface surface = eglCreateWindowSurface(display, config, window_, nullptr);

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

    // Load shader and textures using assetManager_ (passed from JNI). If not set, skip and log a warning.
    if (assetManager_) {
        mainShader = std::unique_ptr<Shader>(
                Shader::loadShader("main.vert", "main.frag", assetManager_));
        assert(mainShader);

        // Note: activating shader can remain, but guard against null
        if (mainShader) mainShader->activate();

        // setup any other gl related global states
        glClearColor(CORNFLOWER_BLUE);

        // enable alpha globally for now
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // initialise camera..
        camera.position = {0, 0};
        updateRenderArea();

        // initialise none texture..
        noneTexture = TextureAsset::loadAsset(assetManager_, "None.png");
    } else {
        aout << "Warning: assetManager_ not set; skipping shader/texture load" << std::endl;
        // still set GL states so we present a clear color
        glClearColor(CORNFLOWER_BLUE);
    }
}

void Renderer::updateRenderArea() {
    if (display_ == EGL_NO_DISPLAY || surface_ == EGL_NO_SURFACE) return;

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
    // If no EGL context yet, attempt to lazily init on this render thread
    if (context_ == EGL_NO_CONTEXT) {
        initEGL();
    }

    if (context_ == EGL_NO_CONTEXT) return; // nothing to do

    updateRenderArea();

    // If shader not loaded, draw clear color only
    if (!mainShader || !noneTexture) {
        glClear(GL_COLOR_BUFFER_BIT);
        if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE) {
            eglSwapBuffers(display_, surface_);
        }
        return;
    }

    // get camera's projection matrix
    mainShader->setMatrix("viewProjection", camera.getViewProjection()) ;
    mainShader->setImageUniform("uTexture", 0);

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Render all game objects.
    renderLayer(static_cast<int>(GameObjectType::Environment));
    renderLayer(static_cast<int>(GameObjectType::Platform));
    renderLayer(static_cast<int>(GameObjectType::Player));

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
    if (!assetManager_) {
        aout << "No AssetManager available to load texture: " << filepath << std::endl;
        return NO_TEXTURE;
    }
    auto texture = TextureAsset::loadAsset(assetManager_, filepath);

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

void Renderer::terminateEGL() {
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

void Renderer::setWindow(ANativeWindow* window) {
    std::lock_guard<std::mutex> lock(windowMutex_);

    // If there's an existing window, release it and teardown EGL
    if (window_ && window_ != window) {
        // tear down GLES resources tied to previous window
        terminateEGL();
        ANativeWindow_release(window_);
        window_ = nullptr;
    }

    // adopt the new window (caller often passed a reference from ANativeWindow_fromSurface)
    window_ = window;

    // EGL initialization is deferred to render() (render thread) which will call initEGL/create context
}

void Renderer::renderLayer(int type) {
    for(auto& gameObjectPtr : engine.game.getGameObjects()) {
        GameObject& gameObject = *gameObjectPtr;
        if(static_cast<int>(gameObject.getType()) != type)
            continue;
        // Setup the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gameObject.textureId != NO_TEXTURE ? gameObject.textureId : noneTexture->getTextureID());

        mainShader->setMatrix("model", calculateModelMatrix(gameObject));
        mainShader->setVec4("colorMultiplier", gameObject.colorMultiplier);

        // VBO-less draw.
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}