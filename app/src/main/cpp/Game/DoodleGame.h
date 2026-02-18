//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_DOODLEGAME_H
#define DOODLE_DOODLEGAME_H

#include <vector>
#include <memory>
#include "GameObject/Player.h"
#include "GameObject/Platform.h"
#include "GameObject/Background.h"

// Forward decl
class Camera;
class Engine;

class DoodleGame {
public:
    // Decoupled constructor: doesn't need full engine init immediately for texture loading?
    // Actually, texture ID loading needs initialized GL context.
    // So DoodleGame creation must happen *after* EGL init.
    // Passing Engine& is fine, but we must ensure we don't call GL methods in constructor
    // OR we defer initialization.
    explicit DoodleGame(Engine& engine, Camera& camera);

public:
    void update(float deltaTime);

    // retrieve all game objects..
    std::vector<std::unique_ptr<GameObject>> const& getGameObjects();

    // get player..
    Player& getPlayer();
    Background& getCurrentBackground();
public:
    void SpawnPlatform(float xPosition, float yPosition);
    bool IsPlayerTouchingPlatform(GameObject const& platform);
    bool SimpleAABB(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax);
    void PlayerJump();

    // Helper to init game objects once GL is ready
    void init();

private:
    const glm::vec2 platformScale = glm::vec2{ 175, 20 };
    // unique pointer for polymorphism.
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    // holds the index of the player game object in the array of gameObjects..
    int playerIndex;
    int backgroundIndex;
    // reference to renderer's camera.
    Camera& camera;

    // reference to engine.
    Engine& engine;

    // Game Stuff
    float nextPlatformSpawn;
    float gravity;
    float distanceBetweenPlatforms;
    bool isInitialized = false;

};

#endif //DOODLE_DOODLEGAME_H
