#pragma once
#include "utils/glm_utils.h"

namespace engine
{
    class GameObject2D;
    struct RectHitArea;
    struct CircleHitArea;

    struct HitArea2DShape
    {
        virtual ~HitArea2DShape() = default;
        HitArea2DShape() = default;

        // visitor pattern
        virtual bool Contains(glm::vec2 point) = 0;
        virtual bool Collides(RectHitArea *other, GameObject2D *support,
                              GameObject2D *otherSupport) = 0;
        virtual bool Collides(CircleHitArea *other, GameObject2D *support, 
                              GameObject2D *otherSupport) = 0;
        virtual bool CollidesWith(HitArea2DShape *other, GameObject2D *support, 
                                  GameObject2D *otherSupport) = 0;
    };

    struct RectHitArea : public HitArea2DShape
    {
        float width, height;
        RectHitArea(float width, float height) : width(width), height(height) {};
        bool Contains(glm::vec2 point) override;
        bool Collides(RectHitArea *other, GameObject2D *support,
                      GameObject2D *otherSupport) override;
        bool Collides(CircleHitArea *other, GameObject2D *support, 
                      GameObject2D *otherSupport) override;
        bool CollidesWith(HitArea2DShape *other, GameObject2D *support,
                          GameObject2D *otherSupport) override;
    };

    struct CircleHitArea : public HitArea2DShape
    {
        float radius;
        CircleHitArea(float radius) : radius(radius) {};
        bool Contains(glm::vec2 point) override;
        bool Collides(RectHitArea *other, GameObject2D *support,
                      GameObject2D *otherSupport) override;
        bool Collides(CircleHitArea *other, GameObject2D *support, 
                      GameObject2D *otherSupport) override;
        bool CollidesWith(HitArea2DShape *other, GameObject2D *support,
                          GameObject2D *otherSupport) override;
    };

    struct HitArea2D
    {
        GameObject2D *support;
        HitArea2DShape *shape;
        HitArea2D() : support(nullptr), shape(nullptr) {};
        HitArea2D(GameObject2D *support, HitArea2DShape *shape)
            : support(support), shape(shape) {};
    };
}

// IF YOU MOVE THIS INCLUDE TO THE TOP OF THE FILE, ALL HELL BREAKS LOOSE
// This is because of the circular dependency between hitarea.h and gameobject.h
#include "gameobject2d.h"