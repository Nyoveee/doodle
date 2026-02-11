//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_PLAYER_H
#define DOODLE_PLAYER_H

#include "GameObject.h"

class Player : public GameObject {
public:
    Player(glm::vec2 position, glm::vec2 scale, GLuint textureId = NO_TEXTURE);
    Player(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier);
    Player(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId);
public:
    const float maxRotationTime{0.5f};
    const int rotationChance{60}; // out of 100
    const float maxMovementSpeed{2000};
    const float movementAcceleration{750};
    const float jumpVelocity{1750};

    float currentRotationTime{0};
    glm::vec2 prevPos{};
    glm::vec2 velocity{};
};


#endif //DOODLE_PLAYER_H
