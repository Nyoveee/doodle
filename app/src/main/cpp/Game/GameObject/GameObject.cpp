//
// Created by Nyove on 1/26/2026.
//

#include "GameObject.h"

GameObject::GameObject(glm::vec2 position, glm::vec2 scale, Type type, GLuint textureId) :
    position        { position },
    scale           { scale },
    rotation        { 0.f },
    type            { type },
    colorMultiplier { 1.0f, 1.0f, 1.0f, 1.0f },
    textureId       { textureId }
{}

GameObject::GameObject(glm::vec2 position, glm::vec2 scale, Type type, glm::vec4 colorMultiplier) :
        position        { position },
        scale           { scale },
        rotation        { 0.f },
        type            { type },
        colorMultiplier { colorMultiplier },
        textureId       { NO_TEXTURE }
{}

GameObject::GameObject(glm::vec2 position, glm::vec2 scale, Type type, glm::vec4 colorMultiplier, GLuint textureId) :
        position        { position },
        scale           { scale },
        rotation        { 0.f },
        type            { type },
        colorMultiplier { colorMultiplier },
        textureId       { textureId }
{}

// Compulsory virtual destructor definition,
// even if it's empty
GameObject::~GameObject() = default;

