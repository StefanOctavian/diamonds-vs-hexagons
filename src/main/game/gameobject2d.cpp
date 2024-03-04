#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "hitarea2d.h"
#include "gameobject2d.h"
#include "transform2d.h"

using namespace engine;

// private constructor
GameObject2D::GameObject2D(GameObject2D *parent, Mesh *mesh, glm::vec2 position, 
                                        glm::vec2 scale, float rotation)
{
    this->mesh = mesh;
    this->parent = parent;
    if (parent != nullptr)
        parent->children.insert(this);

    SetLocalPosition(position);
    SetLocalScale(scale);
    SetLocalRotation(rotation);
}

GameObject2D::GameObject2D()
{
    position = glm::vec2(0);
    localPosition = glm::vec2(0);
    rotation = 0;
    localRotation = 0;
    pseudoScale = glm::vec2(1);
    localScale = glm::vec2(1);
    parent = nullptr;
    mesh = nullptr;
}

GameObject2D::GameObject2D(Mesh *mesh, glm::vec2 position, glm::vec2 scale, float rotation)
    : GameObject2D(nullptr, mesh, position, scale, rotation) {}

GameObject2D::GameObject2D(glm::vec2 position, glm::vec2 scale, float rotation)
    : GameObject2D(nullptr, nullptr, position, scale, rotation) {}

GameObject2D::~GameObject2D()
{
    if (parent != nullptr && !willBeDetached) {
        parent->DetachChild(this);
    }
    for (auto child : children) {
        child->willBeDetached = true;
        delete child;
    }
    children.clear();
}

GameObject2D *GameObject2D::CreateChild(GameObject2D *parent, Mesh *mesh, glm::vec2 position,
                                        glm::vec2 scale, float rotation) 
{
    return new GameObject2D(parent, mesh, position, scale, rotation);
}

GameObject2D *GameObject2D::CreateChild(GameObject2D *parent, glm::vec2 position,
                                        glm::vec2 scale, float rotation)
{
    return new GameObject2D(parent, nullptr, position, scale, rotation);
}

std::unordered_set<GameObject2D *> &GameObject2D::GetChildren()
{
    return children;
}

void GameObject2D::AddChild(GameObject2D *child, bool keepWorldPosition)
{
    if (child->parent != nullptr)
        child->parent->DetachChild(child);

    children.insert(child);

    child->parent = this;
    if (keepWorldPosition) {
        // trigger a recalculation of the child's local position and rotation
        child->SetPosition(child->position);
        child->SetRotation(child->rotation);
        child->SetPseudoScale(child->pseudoScale);
    } else {
        // trigger a recalculation of the child's world position and rotation
        child->SetLocalPosition(child->localPosition);
        child->SetLocalRotation(child->localRotation);
        child->SetLocalScale(child->localScale);
    }
}

void GameObject2D::DetachChild(GameObject2D *child)
{
    children.erase(child);
    child->parent = nullptr;
}

glm::vec2 GameObject2D::GetLocalPosition() { return localPosition; }
float GameObject2D::GetLocalRotation() { return localRotation; }
glm::vec2 GameObject2D::GetLocalScale() { return localScale; }
glm::vec2 GameObject2D::GetPosition() { return position; }
float GameObject2D::GetRotation() { return rotation; }
glm::vec2 GameObject2D::GetPseudoScale() { return pseudoScale; }

void GameObject2D::SetLocalPosition(glm::vec2 newLocalPos)
{
    localPosition = newLocalPos;
    if (parent == nullptr) {
        position = newLocalPos;
    } else {
        glm::vec2 disp = parent->pseudoScale * newLocalPos;
        float angle = parent->rotation;
        disp.x = disp.x * glm::cos(angle) - disp.y * glm::sin(angle);
        disp.y = disp.x * glm::sin(angle) + disp.y * glm::cos(angle);
        position = parent->position + disp;
    }

    for (auto child : children)
    {
        child->SetLocalPosition(child->localPosition);
    }
    if (onPositionChange)
        onPositionChange();
}

void GameObject2D::SetPosition(glm::vec2 newPosition)
{
    position = newPosition;
    if (parent == nullptr) {
        localPosition = newPosition;
    } else {
        glm::vec2 disp = newPosition - parent->position;
        float angle = -parent->rotation;
        disp.x = disp.x * glm::cos(angle) - disp.y * glm::sin(angle);
        disp.y = disp.x * glm::sin(angle) + disp.y * glm::cos(angle);
        localPosition = disp / parent->pseudoScale;
    }

    for (auto child : children)
    {
        child->SetLocalPosition(child->localPosition);
    }
    if (onPositionChange)
        onPositionChange();
}

void GameObject2D::SetLocalScale(glm::vec2 newLocalScale)
{
    localScale = newLocalScale;
    pseudoScale = (parent == nullptr) ? newLocalScale : newLocalScale * parent->pseudoScale;
    for (auto child : children)
    {
        child->SetLocalScale(child->localScale);
    }
}

void GameObject2D::SetPseudoScale(glm::vec2 newPseudoScale)
{
    pseudoScale = newPseudoScale;
    localScale = (parent == nullptr) ? newPseudoScale : newPseudoScale / parent->pseudoScale;
    for (auto child : children)
    {
        child->SetPseudoScale(child->pseudoScale);
    }
}

void GameObject2D::SetLocalRotation(float newLocalRotation)
{
    localRotation = newLocalRotation;
    rotation = (parent == nullptr) ? newLocalRotation : parent->rotation + newLocalRotation;
    for (auto child : children)
    {
        if (child->fixedRotation)
            child->SetRotation(child->rotation);
        else
            child->SetLocalRotation(child->localRotation);
    }
}

void GameObject2D::SetRotation(float newRotation)
{
    rotation = newRotation;
    localRotation = (parent == nullptr) ? newRotation : newRotation - parent->rotation;
    for (auto child : children)
    {
        if (child->fixedRotation)
            child->SetRotation(child->rotation);
        else
            child->SetLocalRotation(child->localRotation);
    }
}

glm::mat3 GameObject2D::ObjectToWorldMatrix()
{
    return ((parent == nullptr) ? glm::mat3(1) : parent->ObjectToWorldMatrix()) *
           transform2D::Translate(localPosition.x, localPosition.y) *
           transform2D::Rotate(localRotation) *
           transform2D::Scale(localScale.x, localScale.y);
}

glm::mat3 GameObject2D::WorldToObjectMatrix()
{
    return transform2D::Scale(1.f / localScale.x, 1.f / localScale.y) *
           transform2D::Rotate(-localRotation) *
           transform2D::Translate(-localPosition.x, -localPosition.y) *
           ((parent == nullptr) ? glm::mat3(1) : parent->WorldToObjectMatrix());
}

// added for completeness; not used
glm::vec2 GameObject2D::ObjectToWorldPosition(glm::vec2 point)
{
    // Optimization: if the object is not rotated, we can skip the matrix multiplication
    if (rotation == 0 || fixedRotation)
    {
        glm::vec2 p = glm::vec2(point.x * localScale.x + localPosition.x,
                                point.y * localScale.y + localPosition.y);
        return (parent == nullptr) ? p : parent->ObjectToWorldPosition(p);
    }
    return ObjectToWorldMatrix() * glm::vec3(point, 1);
}

glm::vec2 GameObject2D::WorldToObjectPosition(glm::vec2 point)
{
    // Optimization: if the object is not rotated, we can skip the matrix multiplication
    if (rotation == 0 || fixedRotation)
    {
        glm::vec2 p = (parent == nullptr) ? point : parent->WorldToObjectPosition(point);
        return glm::vec2((p.x - localPosition.x) / localScale.x,
                         (p.y - localPosition.y) / localScale.y);
    }
    return WorldToObjectMatrix() * glm::vec3(point, 1);
}

HitArea2D GameObject2D::GetHitArea() { return hitArea; }

void GameObject2D::SetHitArea(HitArea2DShape *shape, glm::vec2 offset, 
                              glm::vec2 scale, float rotation)
{
    GameObject2D *support = new GameObject2D(this, nullptr, offset, scale, rotation);
    support->name = "hitArea";
    hitArea = HitArea2D(support, shape);
}

bool GameObject2D::Contains(glm::vec2 point)
{
    if (hitArea.support == nullptr)
        return false;
    glm::vec2 objectPoint = hitArea.support->WorldToObjectPosition(point);
    return hitArea.shape->Contains(objectPoint);
}

// This method is strongly limited as proper collision detection requires a lot
// more research and work. The user must respect the following contract:
// - NO transformations (scale, rotation) done on the object should change the shape
//  of the hit area (e.g. no non-uniform scaling, no rotation)
// - This includes the parent's transformations
// - Mirroring is OK
// Failure to respect this contract will result in incorrect collision detection.
bool GameObject2D::Collides(GameObject2D *other)
{
    if (hitArea.support == nullptr || other->hitArea.support == nullptr)
        return false;

    return hitArea.shape->CollidesWith(other->hitArea.shape, hitArea.support, 
                                       other->hitArea.support);
}

void GameObject2D::SetRectHitArea(float width, float height, glm::vec2 offset, 
                                  glm::vec2 scale, float rotation)
{
    SetHitArea(new RectHitArea(width, height), offset, scale, rotation);
}

void GameObject2D::SetCircleHitArea(float radius, glm::vec2 offset)
{
    SetHitArea(new CircleHitArea(radius), offset);
    hitArea.support->fixedRotation = true;
}

GameObject2D *GameObject2D::InertDeepCopy(bool keepWorldPosition)
{
    GameObject2D *copy = keepWorldPosition ? 
        new GameObject2D(mesh, position, localScale, rotation) : 
        new GameObject2D(mesh, localPosition, localScale, localRotation);

    for (auto child : children)
        copy->AddChild(child->InertDeepCopy(false), false);
    return copy;
}