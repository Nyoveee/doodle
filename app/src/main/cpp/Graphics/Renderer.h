//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_RENDERER_H
#define DOODLE_RENDERER_H

struct android_app;

#include <EGL/egl.h>
#include <numeric>

#include "Model.h"
#include "Shader.h"
#include "config.h"
#include "Camera.h"

#include <glm/mat4x4.hpp>

class Engine;
class GameObject;

class Renderer {
public:
    explicit Renderer(Engine& engine, android_app *pApp) :
            engine(engine),
            app_(pApp),
            display_(EGL_NO_DISPLAY),
            surface_(EGL_NO_SURFACE),
            context_(EGL_NO_CONTEXT),
            width_(0),
            height_(0),
            shaderNeedsNewProjectionMatrix_(true){
        initRenderer();
    }

    ~Renderer();

public:
    void render();

    GLuint getTextureId(std::string const& filepath);

public:
    Camera camera;

private:
    /*!
     * Performs necessary OpenGL initialization. Customize this if you want to change your EGL
     * context or application-wide settings.
     */
    void initRenderer();

    /*!
     * @brief we have to check every frame to see if the framebuffer has changed in size. If it has,
     * update the viewport accordingly
     */
    void updateRenderArea();

    // calculate the given model matrix for a game object.
    static glm::mat4 calculateModelMatrix(GameObject const& gameObject);

private:
    Engine& engine;
    android_app *app_;
    EGLDisplay display_;
    EGLSurface surface_;
    EGLContext context_;
    EGLint width_;
    EGLint height_;

    bool shaderNeedsNewProjectionMatrix_;

    std::unique_ptr<Shader> mainShader;

    // owns all the texture.
    std::shared_ptr<TextureAsset> noneTexture;                      // none texture is a 1x1 white texture.
    std::vector<std::shared_ptr<TextureAsset>> textures;            // owns all the textures

    std::unordered_map<std::string, GLuint> textureFilepathToId;    // maps all filepath to the corresponding texture id.. (NOT INDEX)
                                                                    // we can do this because we are not unloading our textures..
};


#endif //DOODLE_RENDERER_H
