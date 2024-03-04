#include "transform2d.h"
#include "utils/glm_utils.h"

using namespace engine;

glm::mat3 transform2D::Translate(float translateX, float translateY)
{
    return glm::mat3(
        1, 0, 0,
        0, 1, 0,
        translateX, translateY, 1);
}

glm::mat3 transform2D::Scale(float scaleX, float scaleY)
{
    return glm::mat3(
        scaleX, 0, 0,
        0, scaleY, 0,
        0, 0, 1);
}

glm::mat3 transform2D::Rotate(float radians)
{
    return glm::mat3(
        glm::cos(radians), glm::sin(radians), 0,
        -glm::sin(radians), glm::cos(radians), 0,
        0, 0, 1);
}