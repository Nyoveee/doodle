//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_RENDERER_H
#define DOODLE_RENDERER_H

#include <EGL/egl.h>
#include <numeric>
#include <android/asset_manager.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "Model.h"
#include "Shader.h"
#include "config.h"
#include "Camera.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

// Forward decl
struct ANativeWindow;

class Engine;
class GameObject;

class Renderer {
public:
    explicit Renderer(Engine& engine);
    ~Renderer();

public:
    void render();
    void renderLayer(int type);
    GLuint getTextureId(std::string const& filepath);

    // Manage window and EGL context on the render thread
    void setWindow(ANativeWindow* window);
    void setAssetManager(AAssetManager* assetManager) { assetManager_ = assetManager; }

    // EGL management needs to protect resources and only touch GL from render thread
    void initEGL();
    void terminateEGL();
    bool isReady() const { return context_ != EGL_NO_CONTEXT; }

public:
    Camera camera;

private:
    void updateRenderArea();
    static glm::mat4 calculateModelMatrix(GameObject const& gameObject);

private:
    Engine& engine;

    // Thread safety for window changes
    std::mutex windowMutex_;
    ANativeWindow* window_ = nullptr;

    EGLDisplay display_;
    EGLSurface surface_;
    EGLContext context_;
    EGLint width_;
    EGLint height_;

    bool shaderNeedsNewProjectionMatrix_;

    std::unique_ptr<Shader> mainShader;
    std::shared_ptr<TextureAsset> noneTexture;
    std::vector<std::shared_ptr<TextureAsset>> textures;
    std::unordered_map<std::string, GLuint> textureFilepathToId;

    // We need asset manager access now without android_app
    AAssetManager* assetManager_ = nullptr;
};


#endif //DOODLE_RENDERER_H
