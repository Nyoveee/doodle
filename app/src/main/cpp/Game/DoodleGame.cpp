//
// Created by Nyove on 1/26/2026.
//

#include "DoodleGame.h"
#include "../Graphics/Camera.h"

#include "GameObject/Player.h"
#include "GameObject/Platform.h"
#include "GameObject/Background.h"
#include "Utils.h"
#include "../Engine.h"
#include "../JNI_Bridge.h"

DoodleGame::DoodleGame(Engine& engine, Camera& camera) :
        engine { engine },
        camera { camera },
        gravity{2000},
        nextPlatformSpawn{400}, // Start with some offset
        distanceBetweenPlatforms{150},
        hasGameRunOnce{false},
        isGameOver{false},
        gameState{GameState::Start}
{
//    // create player..
//    playerIndex = static_cast<int>(gameObjects.size());
//    gameObjects.push_back(std::make_unique<Player>(
//            glm::vec2{0,-camera.scale.y/2.f + 120},
//            glm::vec2{ 150, 150 },
//            engine.getTextureId("Player.png")
//    ));
//    getPlayer().prevPos = getPlayer().position;
//    // Create Background
//    backgroundIndex = static_cast<int>(gameObjects.size());
//    gameObjects.push_back(std::make_unique<Background>(
//            glm::vec2{0,0},
//            glm::vec2{camera.scale.x,camera.scale.y * 3.f},
//            engine.getTextureId("Scrolling Background.png")
//    ));
//    // Starting Platform
//    SpawnPlatform(0, -camera.scale.y/2.f);
//    (gameObjects.end()-1)->get()->scale.x = camera.scale.x;
//    nextPlatformSpawn += gameObjects[playerIndex]->position.y;
//    PlayerJump();
//
//    isGameOver = false;
//    //UI init
//    score = 0;
//    basePos = getPlayer().position;

    engine.getTextureId("Player.png");
    engine.getTextureId("Scrolling Background.png");
    engine.getTextureId("Platform 1.png");
}

Player& DoodleGame::getPlayer() {
    // the player must be valid.
    return *dynamic_cast<Player*>(gameObjects[playerIndex].get());
}

void DoodleGame::update(float deltaTime) {


    switch (gameState) {
        case GameState::Start:
            //State controlled by UI currently
            break;
        case GameState::Playing:
            PlayTime(deltaTime);
            break;
        case GameState::GameOver:
            break;

    }


}

std::vector<std::unique_ptr<GameObject>> const& DoodleGame::getGameObjects() {
    return gameObjects;
}

void DoodleGame::SpawnPlatform(float xPosition, float yPosition) {
    const std::string platformPath[] = {
         "Platform 1.png",
         "Platform 2.png",
         "Platform 3.png",
         "Platform 4.png",
         "Platform 5.png"
    };
    float gameWidth{camera.scale.x};
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
    if(aMin.x > bMax.x || aMin.y > bMax.y)
        return false;
    if(aMax.x < bMin.x || aMax.y < bMin.y)
        return false;
    return true;
}

void DoodleGame::PlayerJump() {
    Player &player{getPlayer()};
    player.velocity.y = player.jumpVelocity;
    // Everytime we jump, roll a 101 dice[0-100]
    int roll = rand() % 101;
    if(roll <= player.rotationChance)
        player.currentRotationTime = player.maxRotationTime;
}

Background& DoodleGame::getCurrentBackground() {
    return *dynamic_cast<Background*>(gameObjects[backgroundIndex].get());
}

void DoodleGame::updateUI(float deltaTime) {

    //game loops keeps running, reference error if engine has not init on start screen.
    if(gameState == GameState::Playing) {
        //calculate top score
        float currentHeight = (getPlayer().position.y - basePos.y) * 0.5f;
        score = std::max(score, currentHeight);

        JNI_UpdateScore(engine.app_, static_cast<int>(score));
    }
}


void DoodleGame::StartGame() {
    if(hasGameRunOnce)
    {
        ResetGame();
    }
    else
    {
        InitPlay();
        hasGameRunOnce = true;
    }

}


void DoodleGame::InitPlay() {

    // create player..
    playerIndex = static_cast<int>(gameObjects.size());
    gameObjects.push_back(std::make_unique<Player>(
            glm::vec2{0,-camera.scale.y/2.f + 120},
            glm::vec2{ 150, 150 },
            engine.getTextureId("Player.png")
    ));
    getPlayer().prevPos = getPlayer().position;
    // Create Background
    backgroundIndex = static_cast<int>(gameObjects.size());
    gameObjects.push_back(std::make_unique<Background>(
            glm::vec2{0,0},
            glm::vec2{camera.scale.x,camera.scale.y * 3.f},
            engine.getTextureId("Scrolling Background.png")
    ));
    // Starting Platform
    SpawnPlatform(0, -camera.scale.y/2.f);
    (gameObjects.end()-1)->get()->scale.x = camera.scale.x;
    nextPlatformSpawn += gameObjects[playerIndex]->position.y;
    PlayerJump();

    isGameOver = false;
    //UI init
    score = 0;
    basePos = getPlayer().position;
    gameState = GameState::Playing;
}

void DoodleGame::PlayTime(float deltaTime) {
    Player &player{getPlayer()};
    float gameWidth{camera.scale.x};
    float gameHeight{camera.scale.y};

    // Update Player Velocity and Position
    player.velocity.x +=
            deltaTime * -engine.GetAccelerometerAcceleration().x * player.movementAcceleration;
    player.velocity.x = std::clamp(player.velocity.x, -player.maxMovementSpeed,
                                   player.maxMovementSpeed);
    player.velocity.y += deltaTime * -gravity;
    player.position += deltaTime * player.velocity;
    float playerScreenXmin = -gameWidth / 2.f + player.scale.x / 2.f;
    float playerScreenXmax = gameWidth / 2.f - player.scale.x / 2.f;
    player.position.x = std::clamp(player.position.x, playerScreenXmin, playerScreenXmax);

    // Wrap around
    if (player.velocity.x < 0 && player.position.x <= playerScreenXmin)
        player.position.x = playerScreenXmax;
    else if (player.velocity.x > 0 && player.position.x >= playerScreenXmax)
        player.position.x = playerScreenXmin;

    // Jump
    for (int i{}; i < gameObjects.size(); ++i) {
        GameObject &go{*gameObjects[i]};
        if (go.getType() != GameObjectType::Platform)
            continue;
        if (IsPlayerTouchingPlatform(go)) {
            PlayerJump();
            break;
        }
    }
    player.prevPos = player.position;

    // Rotation
    player.currentRotationTime = std::max(0.f, player.currentRotationTime - deltaTime);
    player.rotation = Utils::Lerp(
            std::signbit(player.velocity.x) ? 360 : -360,
            0,
            player.currentRotationTime / player.maxRotationTime
    );

    // Increase Camera Height
    if (player.position.y > camera.position.y)
        camera.position.y = player.position.y;

    // Platform despawning
    auto canDespawnPlatform = [&](std::unique_ptr<GameObject> const &go) {
        if (go->getType() != GameObjectType::Platform)
            return false;
        float platformMaxY = go->position.y + go->scale.y / 2;
        return platformMaxY < camera.position.y - gameHeight / 2;
    };
    std::vector<std::unique_ptr<GameObject>>::iterator iter
            = std::remove_if(std::begin(gameObjects), std::end(gameObjects),
                             canDespawnPlatform);
    if (iter != gameObjects.end())
        gameObjects.erase(iter, gameObjects.end());

    // Platform spawning
    for (nextPlatformSpawn; nextPlatformSpawn < camera.position.y + gameHeight /
                                                                    2; nextPlatformSpawn += distanceBetweenPlatforms) {
        float randomX = rand() % static_cast<int>(gameWidth - platformScale.x) - gameWidth / 2 +
                        platformScale.x / 2;
        SpawnPlatform(randomX, nextPlatformSpawn);
    }


    // Scrolling Background
    Background &background = getCurrentBackground();
    if (background.position.y <= camera.position.y - camera.scale.y / 2.f) {
        background.position.y += camera.scale.y;
    }

    //Set game over state if player falls below the screen
    if (!isGameOver && player.position.y < camera.position.y - camera.scale.y / 2.f) {
        isGameOver = true;
        JNI_GameOver(engine.app_, static_cast<int>(score));
        gameState = GameState::GameOver;
    }
}

void DoodleGame::ResetGame() {
    //gameObjects.clear();
    nextPlatformSpawn = 400;
    //camera reset
    glm::vec2 currentScale = camera.scale;
    camera = Camera(); // Reset camera position and scale
    camera.scale = currentScale;

    //despawn all plaforms except the starting one
    auto iter = std::remove_if(gameObjects.begin(), gameObjects.end(),
                               [](const std::unique_ptr<GameObject>& go) {
                                   return go->getType() == GameObjectType::Platform;
                               });
    gameObjects.erase(iter, gameObjects.end());

    //reset player position and velocity
    Player &player{getPlayer()};
    player.position =  glm::vec2{0,-camera.scale.y/2.f};
    getPlayer().prevPos = getPlayer().position;
    player.velocity = glm::vec2{0,0};
    player.rotation = 0;
    PlayerJump();


    //large platform
    gameObjects[backgroundIndex]->position = glm::vec2{0,0};

    // Starting Platform
    SpawnPlatform(0, -camera.scale.y/2.f);
    (gameObjects.end()-1)->get()->scale.x = camera.scale.x;
    nextPlatformSpawn += gameObjects[playerIndex]->position.y;

    isGameOver = false;
    //UI init
    score = 0;
    basePos = getPlayer().position;
    gameState = GameState::Playing;
}


