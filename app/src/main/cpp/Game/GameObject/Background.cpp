//
// Created by Ang Zhi Wei on 11/2/2026.
//

#include "Background.h"

Background::Background(glm::vec2 position, glm::vec2 scale, GLuint textureId) :
        GameObject {position, scale, GameObjectType::Environment, textureId }
{}

Background::Background(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier) :
        GameObject {position, scale, GameObjectType::Environment, colorMultiplier }
{}

Background::Background(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId) :
        GameObject {position, scale, GameObjectType::Environment, colorMultiplier, textureId }
{}
