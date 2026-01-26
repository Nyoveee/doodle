//
// Created by Nyove on 1/26/2026.
//

#include "DoodleGame.h"
#include "../Graphics/Camera.h"

#include "GameObject/Player.h"
#include "GameObject/Platform.h"

#include "../Engine.h"

DoodleGame::DoodleGame(Engine& engine, Camera& camera) :
        engine { engine },
        camera { camera }
{
    // create player..
    playerIndex = static_cast<int>(gameObjects.size());

    gameObjects.push_back(std::make_unique<Player>(
            glm::vec2{0, 0}, glm::vec2{ 200, 200 }, engine.getTextureId("android_robot.png")
    ));

    // create 2 platforms.
    gameObjects.push_back(std::make_unique<Platform>(
            glm::vec2{-50, 100}, glm::vec2{ 200, 50 }, glm::vec4{ 0.1f, 0.8f, 0.9f, 1.f }
    ));

    gameObjects.push_back(std::make_unique<Platform>(
            glm::vec2{50, 300}, glm::vec2{ 220, 50 }
    ));
}

Player& DoodleGame::getPlayer() {
    // the player must be valid.
    return *dynamic_cast<Player*>(gameObjects[playerIndex].get());
}

void DoodleGame::update(float deltaTime) {
    camera.position.y += deltaTime * 10;

    for(auto& gameObjectPtr : gameObjects) {
        gameObjectPtr->rotation += 100.f * deltaTime;
    }
}

std::vector<std::unique_ptr<GameObject>> const& DoodleGame::getGameObjects() {
    return gameObjects;
}