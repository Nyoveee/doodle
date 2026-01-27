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
        camera { camera },
        gravity{980}
{
    // create player..
    playerIndex = static_cast<int>(gameObjects.size());

    gameObjects.push_back(std::make_unique<Player>(
            glm::vec2{0, 0}, glm::vec2{ 200, 200 }, engine.getTextureId("android_robot.png")
    ));
}

Player& DoodleGame::getPlayer() {
    // the player must be valid.
    return *dynamic_cast<Player*>(gameObjects[playerIndex].get());
}

void DoodleGame::update(float deltaTime) {
    Player &player{getPlayer()};

    // Update Player Velocity and Position
    player.velocity.x += deltaTime * -engine.GetAccelerometerAcceleration().x * player.movementAcceleration;
    //player.velocity.y += deltaTime * -gravity;
    player.position += deltaTime * player.velocity;
    float gameWidth{engine.renderer.getScreenDimensions().x};
    float min = -gameWidth/2 + player.scale.x/2;
    float max = gameWidth/2 - player.scale.x/2;
    player.position.x = std::clamp(player.position.x, min, max);

    // Stops the player from moving at the edge
    if(player.position.x == min || player.position.x == max)
        player.velocity.x = 0;
}

std::vector<std::unique_ptr<GameObject>> const& DoodleGame::getGameObjects() {
    return gameObjects;
}

void DoodleGame::SpawnPlatform(float yPosition) {
    float gameWidth{engine.renderer.getScreenDimensions().x};
    gameObjects.push_back(std::make_unique<Platform>(
            glm::vec2{rand() * gameWidth - gameWidth/2, yPosition}, glm::vec2{ 220, 50 }
    ));
}

bool DoodleGame::IsPlayerTouchingPlatform() {
    return false;
}

void DoodleGame::PlayerJump() {
    getPlayer().velocity.y = jumpVelocity;
}
