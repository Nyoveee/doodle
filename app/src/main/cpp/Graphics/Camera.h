//
// Created by Nyove on 1/26/2026.
//

#ifndef DOODLE_CAMERA_H
#define DOODLE_CAMERA_H

#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"

class Camera {
public:
    glm::mat4 getViewProjection() const;

public:
    glm::vec2 position;
    glm::vec2 scale;

private:
//    glm::mat4 viewProjectionMatrix;
};


#endif //DOODLE_CAMERA_H
