//
// Created by Ang Zhi Wei on 11/2/2026.
//

#ifndef DOODLE_BACKGROUND_H
#define DOODLE_BACKGROUND_H

#include "GameObject.h"
class Background : public GameObject{
public:
    Background(glm::vec2 position, glm::vec2 scale, GLuint textureId = NO_TEXTURE);
    Background(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier);
    Background(glm::vec2 position, glm::vec2 scale, glm::vec4 colorMultiplier, GLuint textureId);
};


#endif //DOODLE_BACKGROUND_H
