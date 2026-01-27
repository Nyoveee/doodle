//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_GAMEOBJECT_H
#define DOODLE_GAMEOBJECT_H

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

using GLuint = unsigned int;
constexpr inline static GLuint NO_TEXTURE = std::numeric_limits<GLuint>::max();

enum class Type {
    Player,
    Platform
};

// abstract class..
class GameObject {
public:
    GameObject(glm::vec2 position, glm::vec2 scale, Type type, GLuint textureId);
    GameObject(glm::vec2 position, glm::vec2 scale, Type type, glm::vec4 colorMultiplier);
    GameObject(glm::vec2 position, glm::vec2 scale, Type type, glm::vec4 colorMultiplier, GLuint textureId);

    virtual ~GameObject() = 0;

public:
    glm::vec2 velocity;
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
    glm::vec4 colorMultiplier;

    GLuint textureId;
private:
    Type type;
};

#endif //DOODLE_GAMEOBJECT_H
