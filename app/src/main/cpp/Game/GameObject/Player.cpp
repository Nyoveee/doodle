//
// Created by Nyove on 1/26/2026.
//

#include "Player.h"

Player::Player(glm::vec2 position, glm::vec2 scale, GLuint textureId) :
    GameObject {position, scale, GameObjectType::Player, textureId }
{}

Player::Player(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier) :
    GameObject {position, scale, GameObjectType::Player, colorMultiplier }
{}

Player::Player(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId) :
    GameObject {position, scale, GameObjectType::Player, colorMultiplier, textureId }
{}
