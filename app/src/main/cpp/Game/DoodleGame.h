//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_DOODLEGAME_H
#define DOODLE_DOODLEGAME_H

#include <vector>
#include <memory>
#include "GameObject/GameObject.h"

class Camera;
class Player;
class Engine;
class Background;

class DoodleGame {
public:
    explicit DoodleGame(Engine& engine, Camera& camera);

public:
    void update(float deltaTime);
    void updateUI(float deltaTime);

    // retrieve all game objects..
    std::vector<std::unique_ptr<GameObject>> const& getGameObjects();

    // get player..
    // Doodle Game assumes that there is always one valid player, if not it segfaults.
    // if you want error handling, change it to pointer.
    Player& getPlayer();
    Background& getCurrentBackground();
public:
    void SpawnPlatform(float xPosition, float yPosition);
    bool IsPlayerTouchingPlatform(GameObject const& platform);
    bool SimpleAABB(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax);
    void PlayerJump();
    void InitPlay();
    void PlayTime(float deltaTime);
    void ResetGame();
public:
    enum class GameState{
        Start,
        Playing,
        GameOver
    };

private:

    const glm::vec2 platformScale = glm::vec2{ 175, 20 };
    // unique pointer for polymorphism.
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    // holds the index of the player game object in the array of gameObjects..
    int playerIndex;
    int backgroundIndex;
    // reference to renderer's camera.
    Camera& camera;
    glm::vec2 cameraPos;


    // reference to engine.
    Engine& engine;

    // Game Stuff
    float nextPlatformSpawn;
    float gravity;
    float distanceBetweenPlatforms;
    bool  isGameOver;

    GameState gameState;

    //UI Tracking
    float score;
    glm::vec2 basePos;

};


#endif //DOODLE_DOODLEGAME_H
