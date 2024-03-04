#pragma once

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace game::shapes
{
    const double sqrt3 = sqrt(3);
    const double pi = glm::pi<double>();
    const double starAngleStep = 2 * pi / 5;
    const double innerStarRadius = 0.5 * glm::sin(9 * pi / 10) / glm::sin(7 * pi / 10);

    const double arcsin06 = glm::asin(0.6);

    // square of side 1, centered in (0, 0)
    Mesh *CreateSquare(const std::string &name, glm::vec3 color, float zIndex = 0, bool fill = true);
    // regular hexagon of diameter 1, centered in (0, 0)
    Mesh *CreateHexagon(const std::string &name, glm::vec3 color, float zIndex = 0);
    // rhombus of height 1 and width 0.5, centered in (0, 0) &
    // rectangle of height 0.1 and width 0.4, left middle point in (0, 0)
    Mesh *CreateRhombusGun(const std::string &name, glm::vec3 color, float zIndex = 0);
    // regular star of diameter 1, centered in (0, 0)
    Mesh *CreateStar(const std::string &name, glm::vec3 color, float zIndex = 0);
    // heart of width and height 1, centered in (0, 0)
    Mesh *CreateHeart(const std::string &name, glm::vec3 color, float zIndex = 0);
}