//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_PLATFORM_H
#define DOODLE_PLATFORM_H

#include "GameObject.h"

class Platform  : public GameObject {
public:
    Platform(glm::vec2 position, glm::vec2 scale, GLuint textureId = NO_TEXTURE);
    Platform(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier);
    Platform(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId);
};


#endif //DOODLE_PLATFORM_H
