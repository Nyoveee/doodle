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
    glm::vec2 velocity{};
    float movementAcceleration{500};
    float jumpVelocity{1500};
};


#endif //DOODLE_PLAYER_H
