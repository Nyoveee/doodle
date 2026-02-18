//
// Created by Nyove on 1/26/2026.
//

#include "DoodleGame.h"
#include "../Graphics/Camera.h"
#include "../Engine.h"
#include "Utils.h"
#include <algorithm> // for std::remove_if

DoodleGame::DoodleGame(Engine& engine, Camera& camera) :
        engine { engine },
        camera { camera },
        gravity{2000},
        nextPlatformSpawn{400}, // Start with some offset
        distanceBetweenPlatforms{150},
        isInitialized{false}
{
    // Constructor no longer calls GL methods.
}

void DoodleGame::init() {
    if (isInitialized) return;

    // create player..
    playerIndex = static_cast<int>(gameObjects.size());
    // Texture IDs need active GL context
    gameObjects.push_back(std::make_unique<Player>(
            glm::vec2{0,-camera.scale.y/2.f + 120},
            glm::vec2{ 150, 150 },
            engine.getTextureId("Player.png")
    ));

    // Create Background
    backgroundIndex = static_cast<int>(gameObjects.size());
    gameObjects.push_back(std::make_unique<Background>(
            glm::vec2{0,0},
            glm::vec2{camera.scale.x,camera.scale.y * 3.f},
            engine.getTextureId("Scrolling Background.png")
    ));

    // Starting Platform
    SpawnPlatform(0, -camera.scale.y/2.f);
    if (!gameObjects.empty()) {
       (gameObjects.end()-1)->get()->scale.x = camera.scale.x;
    }

    PlayerJump();

    isInitialized = true;
}

Player& DoodleGame::getPlayer() {
    // Safety check?
    if (gameObjects.empty()) {
        // Fallback or crash
        init(); // Attempt just in case
    }
    return *dynamic_cast<Player*>(gameObjects[playerIndex].get());
}

Background& DoodleGame::getCurrentBackground() {
    if (gameObjects.empty()) init();
    return *dynamic_cast<Background*>(gameObjects[backgroundIndex].get());
}

void DoodleGame::update(float deltaTime) {
    if (!isInitialized) {
        init(); // Try to init if context is ready now?
        if (!isInitialized) return; // If engine texture loading fails, returns NO_TEXTURE, so it won't crash here ideally
    }

    Player &player{getPlayer()};
    float gameWidth{camera.scale.x};
    float gameHeight{camera.scale.y};

    // Update Player Velocity and Position
    // player.velocity.x += deltaTime * -engine.GetAccelerometerAcceleration().x * player.movementAcceleration;
    // Sensor disabled for now
    player.velocity.x += 0;

    player.velocity.x = std::clamp(player.velocity.x, -player.maxMovementSpeed, player.maxMovementSpeed);
    player.velocity.y += deltaTime * -gravity;
    player.position += deltaTime * player.velocity;
    float playerScreenXmin = -gameWidth/2.f + player.scale.x/2.f;
    float playerScreenXmax = gameWidth/2.f - player.scale.x/2.f;
    player.position.x = std::clamp(player.position.x, playerScreenXmin, playerScreenXmax);

    // Wrap around
    if(player.velocity.x < 0 && player.position.x <= playerScreenXmin)
        player.position.x = playerScreenXmax;
    else if(player.velocity.x > 0 && player.position.x >= playerScreenXmax)
        player.position.x = playerScreenXmin;

    // Jump
    for(int i{}; i < gameObjects.size();++i){
        GameObject& go{*gameObjects[i]};
        if(go.getType() != GameObjectType::Platform)
            continue;
        if(IsPlayerTouchingPlatform(go)){
            PlayerJump();
            break;
        }
    }
    player.prevPos = player.position;

    // Rotation
    player.currentRotationTime = std::max(0.f, player.currentRotationTime-deltaTime);
    player.rotation = Utils::Lerp(
            std::signbit(player.velocity.x)? 360: -360,
            0,
            player.currentRotationTime / player.maxRotationTime
    );

    // Increase Camera Height
    if(player.position.y > camera.position.y)
        camera.position.y = player.position.y;

    // Platform despawning
    auto canDespawnPlatform = [&](std::unique_ptr<GameObject> const& go){
        if(go->getType() != GameObjectType::Platform)
            return false;
        float platformMaxY = go->position.y + go->scale.y/2;
        return platformMaxY < camera.position.y - gameHeight/2;
    };
    std::vector<std::unique_ptr<GameObject>>::iterator iter
        = std::remove_if(std::begin(gameObjects), std::end(gameObjects),canDespawnPlatform);
    if(iter!= gameObjects.end())
        gameObjects.erase(iter, gameObjects.end());

    // Platform spawning
    for(nextPlatformSpawn; nextPlatformSpawn < camera.position.y + gameHeight/2; nextPlatformSpawn += distanceBetweenPlatforms){
        float randomX = rand() % static_cast<int>(gameWidth - platformScale.x) - gameWidth/2 + platformScale.x/2;
        SpawnPlatform(randomX, nextPlatformSpawn);
    }

    // Scrolling Background
    Background& background = getCurrentBackground();
    if(background.position.y <= camera.position.y - camera.scale.y/2.f){
        background.position.y += camera.scale.y;
    }
}

std::vector<std::unique_ptr<GameObject>> const& DoodleGame::getGameObjects() {
    return gameObjects;
}

void DoodleGame::SpawnPlatform(float xPosition, float yPosition) {
    if (!isInitialized) return;
    const std::string platformPath[] = {
         "Platform 1.png", "Platform 2.png", "Platform 3.png", "Platform 4.png", "Platform 5.png"
    };
    gameObjects.push_back(std::make_unique<Platform>(
            glm::vec2{xPosition, yPosition},
            platformScale,
            engine.getTextureId(platformPath[rand()%5])));
}

bool DoodleGame::IsPlayerTouchingPlatform(GameObject const& platform) {
    Player &player{getPlayer()};
    if(player.velocity.y >= 0)
        return false;
    glm::vec2 playerMin{player.position - player.scale/2.f};
    glm::vec2 playerMax{player.position + player.scale/2.f};
    glm::vec2 platformMin{platform.position - platform.scale/2.f};
    glm::vec2 platformMax{platform.position + platform.scale/2.f};
    return (player.prevPos.y - player.scale.y/2.f) > platformMax.y
        && SimpleAABB(playerMin,playerMax,platformMin,platformMax);
}

bool DoodleGame::SimpleAABB(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax){
    if(aMin.x > bMax.x || aMin.y > bMax.y) return false;
    if(aMax.x < bMin.x || aMax.y < bMin.y) return false;
    return true;
}

void DoodleGame::PlayerJump() {
     Player &player{getPlayer()};
    player.velocity.y = player.jumpVelocity;
    int roll = rand() % 101;
    if(roll <= player.rotationChance)
        player.currentRotationTime = player.maxRotationTime;
}
