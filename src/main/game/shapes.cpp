#include <vector>

#include "shapes.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace game;

Mesh *shapes::CreateSquare(const std::string& name, glm::vec3 color, float zIndex, bool fill)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.5, -0.5, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.5, -0.5, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.5, 0.5, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(-0.5, 0.5, zIndex), color, glm::vec3(0))
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        3, 0, 2
    };
    Mesh *square = new Mesh(name);
    if (!fill) {
        indices.resize(4);
        square->SetDrawMode(GL_LINE_LOOP);
    }
    square->InitFromData(vertices, indices);

    return square;
}

Mesh *shapes::CreateHexagon(const std::string& name, glm::vec3 color, float zIndex)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 0, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(-0.5, 0, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(-0.25, -sqrt3 / 4, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.25, -sqrt3 / 4, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.5, 0, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.25, sqrt3 / 4, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(-0.25, sqrt3 / 4, zIndex), color, glm::vec3(0)),
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5, 6, 1 };
    Mesh *hexagon = new Mesh(name);
    hexagon->SetDrawMode(GL_TRIANGLE_FAN);
    hexagon->InitFromData(vertices, indices);

    return hexagon;
}

Mesh *shapes::CreateRhombusGun(const std::string& name, glm::vec3 color, float zIndex)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.25, 0, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0, -0.5, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.25, 0, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0, 0.5, zIndex), color, glm::vec3(0)),

        VertexFormat(glm::vec3(0, 0.1, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0, -0.1, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.4, 0.1, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.4, -0.1, zIndex), color, glm::vec3(0))
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        5, 7, 6
    };
    Mesh *rhombusGun = new Mesh(name);
    rhombusGun->InitFromData(vertices, indices);

    return rhombusGun;
}

Mesh *shapes::CreateStar(const std::string& name, glm::vec3 color, float zIndex)
{
    std::vector<VertexFormat> vertices;
    vertices.reserve(11);
    vertices.push_back(VertexFormat(glm::vec3(0, 0, zIndex), color, glm::vec3(0)));
    for (int i = 0; i < 10; i += 2) {
        double outerAngle = pi / 2 + i * starAngleStep;
        double innerAngle = 7 * pi / 10 + i * starAngleStep;
        vertices.push_back(VertexFormat(
            glm::vec3(0.5 * glm::cos(outerAngle), 
                      0.5 * glm::sin(outerAngle), zIndex), 
            color, glm::vec3(0)));
        vertices.push_back(VertexFormat(
            glm::vec3(innerStarRadius * glm::cos(innerAngle), 
                      innerStarRadius * glm::sin(innerAngle), zIndex), 
            color, glm::vec3(0)));
    }
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1 };
    Mesh *star = new Mesh(name);
    star->SetDrawMode(GL_TRIANGLE_FAN);
    star->InitFromData(vertices, indices);

    return star;
}

Mesh *shapes::CreateHeart(const std::string &name, glm::vec3 color, float zIndex)
{
    const double leftmostX = -9. / 20;
    const double leftmostY = 1. / 10;
    const int numSteps = 20;

    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.225, 0.175, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0.225, 0.175, zIndex), color, glm::vec3(0)),
        VertexFormat(glm::vec3(0, 0.25, zIndex), color, glm::vec3(0)),
    };
    vertices.reserve((numSteps + 1) * 2);

    std::vector<unsigned int> indices = {
        0, 2, 3,
        2, 1, 4
    };
    indices.reserve((numSteps + 1) * 6);

    const double maxAngle = pi + arcsin06;
    const double angleStep = maxAngle / numSteps;

    for (int i = 1; i < numSteps; ++i) {
        double angle = i * angleStep;
        vertices.push_back(VertexFormat(glm::vec3(
            -0.25 + 0.25 * glm::cos(angle), 
             0.25 + 0.25 * glm::sin(angle), zIndex), 
            color, glm::vec3(0)));
        vertices.push_back(VertexFormat(glm::vec3(
            0.25 - 0.25 * glm::cos(angle), 
            0.25 + 0.25 * glm::sin(angle), zIndex), 
            color, glm::vec3(0)));
        indices.push_back(0);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 3);

        indices.push_back(2 * i + 2);
        indices.push_back(1);
        indices.push_back(2 * i + 4);
    }
    vertices.push_back(VertexFormat(glm::vec3(leftmostX, leftmostY, zIndex), color, glm::vec3(0)));
    vertices.push_back(VertexFormat(glm::vec3(-leftmostX, leftmostY, zIndex), color, glm::vec3(0)));
    vertices.push_back(VertexFormat(glm::vec3(0, -0.5, zIndex), color, glm::vec3(0)));
    indices.push_back(2 * numSteps + 1);
    indices.push_back(2 * numSteps + 2);
    indices.push_back(2);
    indices.push_back(2 * numSteps + 1);
    indices.push_back(2 * numSteps + 3);
    indices.push_back(2 * numSteps + 2);

    Mesh *heart = new Mesh(name);
    heart->InitFromData(vertices, indices);

    return heart;
}