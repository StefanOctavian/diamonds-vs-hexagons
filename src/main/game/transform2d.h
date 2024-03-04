#pragma once

#include "utils/glm_utils.h"

namespace engine::transform2D
{
    glm::mat3 Translate(float translateX, float translateY);
    glm::mat3 Scale(float scaleX, float scaleY);
    glm::mat3 Rotate(float radians);
}