#pragma once
#include <unordered_map>

#include "gameobject2d.h"

using namespace engine;

namespace game
{
    class ColoredItem : public GameObject2D
    {
    public:
        enum Color
        {
            Frost = 0,
            Chartreuse = 1,
            Crimson = 2,
            Lavander = 3
        };

        ColoredItem(Color color) : GameObject2D()
        {
            this->color = color;
        }
        ColoredItem(Color color, Mesh *mesh, glm::vec2 position,
                    glm::vec2 scale = glm::vec2(1), float rotation = 0,
                    GameObject2D *parent = nullptr);

        const static std::unordered_map<Color, glm::vec3> colors;
        const static std::unordered_map<Color, glm::vec3> secondaryColors;

        Color color;
    };

    class RhombusGun : public ColoredItem
    {
    public:
        static std::unordered_map<Color, Mesh *> meshes;
        static const float rechargeTime;
        static void InitMeshes();
        static void ClearMeshes();

        RhombusGun(Color color, glm::vec2 position, GameObject2D *parent = nullptr);

        float timeSinceLastShot = 0;
        int laneNumber = -1;
    };

    class HexagonEnemy : public ColoredItem
    {
    public:
        static float speed;
        static std::unordered_map<Color, Mesh *> meshes;
        static std::unordered_map<Color, Mesh *> meshesSecondary;
        static std::unordered_map<Color, Mesh *> meshesTop;
        static std::unordered_map<Color, Mesh *> meshesTopSecondary;
        static void InitMeshes();
        static void ClearMeshes();

        HexagonEnemy(Color color, glm::vec2 position, GameObject2D *parent = nullptr);

        int laneNumber = -1;
        int lives = 3;
    };

    class ProjectileStar : public ColoredItem
    {
    public:
        static std::unordered_map<Color, Mesh *> meshes;
        static void InitMeshes();
        static void ClearMeshes();

        ProjectileStar(Color color, glm::vec2 position, GameObject2D *parent = nullptr);
    };
}