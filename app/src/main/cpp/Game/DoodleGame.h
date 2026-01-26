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

class DoodleGame {
public:
    explicit DoodleGame(Engine& engine, Camera& camera);

public:
    void update(float deltaTime);

    // retrieve all game objects..
    std::vector<std::unique_ptr<GameObject>> const& getGameObjects();

    // get player..
    // Doodle Game assumes that there is always one valid player, if not it segfaults.
    // if you want error handling, change it to pointer.
    Player& getPlayer();

private:
    // unique pointer for polymorphism.
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // holds the index of the player game object in the array of gameObjects..
    int playerIndex;

    // reference to renderer's camera.
    Camera& camera;

    // reference to engine.
    Engine& engine;
};


#endif //DOODLE_DOODLEGAME_H
