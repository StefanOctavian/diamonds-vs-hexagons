#include <unordered_map>

#include "coloreditem.h"
#include "shapes.h"

using namespace engine;
using namespace game;

const std::unordered_map<ColoredItem::Color, glm::vec3> ColoredItem::colors = {
    {Frost, glm::vec3(0.878, 1, 0.992)},
    {Chartreuse, glm::vec3(0.875, 0.992, 0.024)},
    {Crimson, glm::vec3(0.616, 0.133, 0.208)},
    {Lavander, glm::vec3(0.914, 0.796, 1)}};

const std::unordered_map<ColoredItem::Color, glm::vec3> ColoredItem::secondaryColors = {
    {Frost, glm::vec3(0.216, 0.408, 0.576)},
    {Chartreuse, glm::vec3(0.706, 0.631, 0.184)},
    {Crimson, glm::vec3(0.333, 0, 0.067)},
    {Lavander, glm::vec3(0.412, 0.22, 0.451)}};

ColoredItem::ColoredItem(Color color, Mesh *mesh, glm::vec2 position, glm::vec2 scale,
                         float rotation, GameObject2D *parent)
    : GameObject2D(parent, mesh, position, scale, rotation)
{
    this->color = color;
}

const float RhombusGun::rechargeTime = 1.5f;
RhombusGun::RhombusGun(Color color, glm::vec2 position, GameObject2D *parent)
    : ColoredItem(color, meshes[color], position, glm::vec2(7, 7), 0, parent)
{
    SetRectHitArea(0.65f, 1, glm::vec2(0.075, 0));
}

std::unordered_map<RhombusGun::Color, Mesh *> RhombusGun::meshes;
void RhombusGun::InitMeshes()
{
    Mesh *frostMesh = shapes::CreateRhombusGun("rhombusGunFrost", ColoredItem::colors.at(Frost), 0);
    Mesh *chartreuseMesh = shapes::CreateRhombusGun("rhombusGunChartreuse", ColoredItem::colors.at(Chartreuse), 0);
    Mesh *crimsonMesh = shapes::CreateRhombusGun("rhombusGunCrimson", ColoredItem::colors.at(Crimson), 0);
    Mesh *lavanderMesh = shapes::CreateRhombusGun("rhombusGunLavander", ColoredItem::colors.at(Lavander), 0);

    meshes = {
        {Frost, frostMesh},
        {Chartreuse, chartreuseMesh},
        {Crimson, crimsonMesh},
        {Lavander, lavanderMesh}};
}

void RhombusGun::ClearMeshes()
{
    for (auto &mesh : meshes)
    {
        delete mesh.second;
    }
    meshes.clear();
}

float HexagonEnemy::speed = 3.f;
HexagonEnemy::HexagonEnemy(Color color, glm::vec2 position, GameObject2D *parent)
    : ColoredItem(color, meshesSecondary[color], position, glm::vec2(7, 7), 0, parent)
{
    SetCircleHitArea(0.5f);
    velocity = glm::vec2(HexagonEnemy::speed, 0);
    angularVelocity = -1.f;
    GameObject2D *inside = 
    GameObject2D::CreateChild(this, meshes[color], glm::vec2(0, 0), glm::vec2(0.7f), 0);
};

std::unordered_map<HexagonEnemy::Color, Mesh *> HexagonEnemy::meshes;
std::unordered_map<HexagonEnemy::Color, Mesh *> HexagonEnemy::meshesSecondary;
std::unordered_map<HexagonEnemy::Color, Mesh *> HexagonEnemy::meshesTop;
std::unordered_map<HexagonEnemy::Color, Mesh *> HexagonEnemy::meshesTopSecondary;
void HexagonEnemy::InitMeshes()
{
    Mesh *frostMesh = shapes::CreateHexagon("hexagonFrost", ColoredItem::colors.at(Frost), 1);
    Mesh *chartreuseMesh = shapes::CreateHexagon("hexagonChartreuse", ColoredItem::colors.at(Chartreuse), 1);
    Mesh *crimsonMesh = shapes::CreateHexagon("hexagonCrimson", ColoredItem::colors.at(Crimson), 1);
    Mesh *lavanderMesh = shapes::CreateHexagon("hexagonLavander", ColoredItem::colors.at(Lavander), 1);

    Mesh *frostMesh2 = shapes::CreateHexagon("rhombusGunFrost2", ColoredItem::secondaryColors.at(Frost), 0);
    Mesh *chartreuseMesh2 = shapes::CreateHexagon("rhombusGunChartreuse2", ColoredItem::secondaryColors.at(Chartreuse), 0);
    Mesh *crimsonMesh2 = shapes::CreateHexagon("rhombusGunCrimson2", ColoredItem::secondaryColors.at(Crimson), 0);
    Mesh *lavanderMesh2 = shapes::CreateHexagon("rhombusGunLavander2", ColoredItem::secondaryColors.at(Lavander), 0);

    meshes = {
        {Frost, frostMesh},
        {Chartreuse, chartreuseMesh},
        {Crimson, crimsonMesh},
        {Lavander, lavanderMesh}};

    meshesSecondary = {
        {Frost, frostMesh2},
        {Chartreuse, chartreuseMesh2},
        {Crimson, crimsonMesh2},
        {Lavander, lavanderMesh2}
    };

    Mesh *frostMesh3 = shapes::CreateHexagon("hexagonFrost3", ColoredItem::colors.at(Frost), 5);
    Mesh *chartreuseMesh3 = shapes::CreateHexagon("hexagonChartreuse3", ColoredItem::colors.at(Chartreuse), 5);
    Mesh *crimsonMesh3 = shapes::CreateHexagon("hexagonCrimson3", ColoredItem::colors.at(Crimson), 5);
    Mesh *lavanderMesh3 = shapes::CreateHexagon("hexagonLavander3", ColoredItem::colors.at(Lavander), 5);

    Mesh *frostMesh4 = shapes::CreateHexagon("rhombusGunFrost4", ColoredItem::secondaryColors.at(Frost), 4);
    Mesh *chartreuseMesh4 = shapes::CreateHexagon("rhombusGunChartreuse4", ColoredItem::secondaryColors.at(Chartreuse), 4);
    Mesh *crimsonMesh4 = shapes::CreateHexagon("rhombusGunCrimson4", ColoredItem::secondaryColors.at(Crimson), 4);
    Mesh *lavanderMesh4 = shapes::CreateHexagon("rhombusGunLavander4", ColoredItem::secondaryColors.at(Lavander), 4);

    meshesTop = {
        {Frost, frostMesh3},
        {Chartreuse, chartreuseMesh3},
        {Crimson, crimsonMesh3},
        {Lavander, lavanderMesh3}};

    meshesTopSecondary = {
        {Frost, frostMesh4},
        {Chartreuse, chartreuseMesh4},
        {Crimson, crimsonMesh4},
        {Lavander, lavanderMesh4}};
}

void HexagonEnemy::ClearMeshes()
{
    for (auto &mesh : meshes)
    {
        delete mesh.second;
    }
    meshes.clear();
}

ProjectileStar::ProjectileStar(Color color, glm::vec2 position, GameObject2D *parent)
    : ColoredItem(color, meshes[color], position, glm::vec2(2.8f), 0, parent)
{
    SetCircleHitArea(0.5f);
    velocity = glm::vec2(28, 0);
    angularVelocity = -2.f;
};

std::unordered_map<ProjectileStar::Color, Mesh *> ProjectileStar::meshes;
void ProjectileStar::InitMeshes()
{
    Mesh *frostMesh = shapes::CreateStar("starFrost", ColoredItem::secondaryColors.at(Frost), 1);
    Mesh *chartreuseMesh = shapes::CreateStar("starChartreuse", ColoredItem::secondaryColors.at(Chartreuse), 1);
    Mesh *crimsonMesh = shapes::CreateStar("starCrimson", ColoredItem::secondaryColors.at(Crimson), 1);
    Mesh *lavanderMesh = shapes::CreateStar("starLavander", ColoredItem::secondaryColors.at(Lavander), 1);

    meshes = {
        {Frost, frostMesh},
        {Chartreuse, chartreuseMesh},
        {Crimson, crimsonMesh},
        {Lavander, lavanderMesh}
    };
}

void ProjectileStar::ClearMeshes()
{
    for (auto &mesh : meshes)
    {
        delete mesh.second;
    }
    meshes.clear();
}