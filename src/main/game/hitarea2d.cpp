#include "hitarea2d.h"
#include "gameobject2d.h"
#include <iostream>

using namespace engine;

bool RectHitArea::Contains(glm::vec2 point)
{
    return point.x >= -width / 2 && point.x <= width / 2 &&
           point.y >= -height / 2 && point.y <= height / 2;
}

bool RectHitArea::Collides(RectHitArea *other, GameObject2D *support, GameObject2D *otherSupport)
{
    glm::vec2 center = support->GetPosition();
    glm::vec2 otherCenter = otherSupport->GetPosition();
    float thisW = width * support->GetPseudoScale().x;
    float thisH = height * support->GetPseudoScale().y;
    float otherW = otherSupport->GetPseudoScale().x * other->width;
    float otherH = otherSupport->GetPseudoScale().y * other->height;

    return center.x - thisW / 2 <= otherCenter.x + otherW / 2 &&
           center.x + thisW / 2 >= otherCenter.x - otherW / 2 &&
           center.y - thisH / 2 <= otherCenter.y + otherH / 2 &&
           center.y + thisH / 2 >= otherCenter.y - otherH / 2;
}

bool RectHitArea::Collides(CircleHitArea *other, GameObject2D *support, GameObject2D *otherSupport)
{
    glm::vec2 center = support->GetPosition();
    glm::vec2 otherCenter = otherSupport->GetPosition();
    float radius = glm::abs(otherSupport->GetPseudoScale().x) * other->radius;
    float thisW = width * support->GetPseudoScale().x;
    float thisH = height * support->GetPseudoScale().y;

    glm::vec2 closestPoint = glm::vec2(
        glm::clamp(otherCenter.x, center.x - thisW / 2, center.x + thisW / 2),
        glm::clamp(otherCenter.y, center.y - thisH / 2, center.y + thisH / 2));
    return glm::distance(closestPoint, otherCenter) <= radius;
}

bool RectHitArea::CollidesWith(HitArea2DShape *other, GameObject2D *support, GameObject2D *otherSupport)
{
    return other->Collides(this, otherSupport, support);
}

bool CircleHitArea::Contains(glm::vec2 point)
{
    return glm::distance(point, glm::vec2(0)) <= radius;
}

bool CircleHitArea::Collides(RectHitArea *other, GameObject2D *support, GameObject2D *otherSupport)
{
    return other->Collides(this, otherSupport, support);
}

bool CircleHitArea::Collides(CircleHitArea *other, GameObject2D *support, GameObject2D *otherSupport)
{
    glm::vec2 center = support->GetPosition();
    glm::vec2 otherCenter = otherSupport->GetPosition();
    float radius = glm::abs(support->GetPseudoScale().x) * this->radius;
    float otherRadius = glm::abs(otherSupport->GetPseudoScale().x) * other->radius;

    return glm::distance(center, otherCenter) <= radius + otherRadius;
}

bool CircleHitArea::CollidesWith(HitArea2DShape *other, GameObject2D *support, GameObject2D *otherSupport)
{
    return other->Collides(this, otherSupport, support);
}
