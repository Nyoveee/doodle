//
// Created by Nyove on 1/26/2026.
//

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewProjection() const {
    // build the orthogonal matrix..
    return glm::ortho(
            position.x - scale.x / 2.f,
            position.x + scale.x / 2.f,
            position.y - scale.y / 2.f,
            position.y + scale.y / 2.f, -1.f, 1.f);

//    return glm::mat4(1.f);
}