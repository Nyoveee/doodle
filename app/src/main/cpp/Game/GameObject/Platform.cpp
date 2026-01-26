//
// Created by Nyove on 1/26/2026.
//

#include "Platform.h"

Platform::Platform(glm::vec2 position, glm::vec2 scale, GLuint textureId) :
    GameObject { position, scale, Type::Platform, textureId }
{}

Platform::Platform(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier) :
    GameObject { position, scale, Type::Platform, colorMultiplier }
{}

Platform::Platform(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId) :
    GameObject { position, scale, Type::Platform, colorMultiplier, textureId }
{}
