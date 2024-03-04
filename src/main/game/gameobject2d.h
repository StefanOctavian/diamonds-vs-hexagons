#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "../wisteria_engine/material.h"
#include "hitarea2d.h"
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace engine
{
    class GameObject2D
    {
    public:
        GameObject2D();
        GameObject2D(Mesh *mesh, glm::vec2 position, glm::vec2 scale = glm::vec2(1),
                     float rotation = 0);
        GameObject2D(glm::vec2 position, glm::vec2 scale = glm::vec2(1),
                     float rotation = 0);
        virtual ~GameObject2D();

        static GameObject2D *CreateChild(GameObject2D *parent, Mesh *mesh,
                                         glm::vec2 position, glm::vec2 scale = glm::vec2(1),
                                         float rotation = 0);
        static GameObject2D *CreateChild(GameObject2D *parent, glm::vec2 position,
                                         glm::vec2 scale = glm::vec2(1), float rotation = 0);

        Mesh *mesh = nullptr;
        std::string name = "";
        // if true, this gameobject will not change its world rotation when
        // its parent gameobject is transformed
        bool fixedRotation = false;
        glm::vec2 velocity = glm::vec2(0);
        float angularVelocity = 0;
        bool useUnscaledTime = false;
        Material material;

        // events
        std::function<void()> onPositionChange;

        // children
        std::unordered_set<GameObject2D *> &GetChildren();
        void AddChild(GameObject2D *child, bool keepWorldPosition = true);
        void DetachChild(GameObject2D *child);

        // position, scale, rotation
        glm::vec2 GetPosition();
        void SetPosition(glm::vec2 position);
        float GetRotation();
        void SetRotation(float rotation);
        glm::vec2 GetPseudoScale();
        void SetPseudoScale(glm::vec2 scale);

        glm::vec2 GetLocalPosition();
        void SetLocalPosition(glm::vec2 position);
        glm::vec2 GetLocalScale();
        void SetLocalScale(glm::vec2 scale);
        float GetLocalRotation();
        void SetLocalRotation(float rotation);

        // world transformations
        glm::mat3 ObjectToWorldMatrix();
        glm::mat3 WorldToObjectMatrix();
        glm::vec2 ObjectToWorldPosition(glm::vec2 point);
        glm::vec2 WorldToObjectPosition(glm::vec2 point);

        // hit area
        HitArea2D GetHitArea();
        void SetHitArea(HitArea2DShape *shape, glm::vec2 offset = glm::vec2(0),
                        glm::vec2 scale = glm::vec2(1), float rotation = 0);
        bool Contains(glm::vec2 point);
        bool Collides(GameObject2D *other);
        void SetRectHitArea(float width, float height, glm::vec2 offset = glm::vec2(0),
                            glm::vec2 scale = glm::vec2(1), float rotation = 0);
        void SetCircleHitArea(float radius, glm::vec2 offset = glm::vec2(0));

        GameObject2D *InertDeepCopy(bool keepWorldPosition = true);

    protected:

        GameObject2D(GameObject2D *parent, Mesh *mesh, glm::vec2 position, 
                     glm::vec2 scale = glm::vec2(1), float rotation = 0);

        glm::vec2 localPosition;
        glm::vec2 localScale;
        float localRotation;
        glm::vec2 position;
        float rotation;
        glm::vec2 pseudoScale;
        bool willBeDetached = false;

        GameObject2D *parent = nullptr;
        HitArea2D hitArea;
        std::unordered_set<GameObject2D *> children;
    };
}