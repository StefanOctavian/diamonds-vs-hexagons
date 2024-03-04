#pragma once
#include <set>
#include <unordered_set>
#include <unordered_map>
#include "gameobject2d.h"

#include "components/simple_scene.h"

namespace engine
{
    class ControlledScene2D : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

        ControlledScene2D();
        ~ControlledScene2D();
        void Init() override;
        void LoadShader(const std::string &name, const std::string &vertexShader, 
                        const std::string &fragmentShader);

    protected:
        virtual void Initialize() {}; 
        virtual void Tick() {};
        virtual void OnResizeWindow(int width, int height) {};

        glm::vec2 ScreenCoordsToLogicCoords(int screenX, int screenY);
        void Destroy(GameObject2D *gameObject);

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        // void FrameEnd() override;

        void OnWindowResize(int width, int height) override;

        // Matrix calculations
        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        glm::mat3 InvVizualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        // Set the scene
        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor, bool clear);
        void RenderMeshCustomMaterial(Mesh *mesh, Material material, const glm::mat3 &modelViewMatrix);
        void DrawGameObject(GameObject2D *gameObject, glm::mat3 parentModelMatrix);

    protected:
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        // set of all root game objects
        std::unordered_set<GameObject2D *> gameObjects;
        std::set<std::pair<int, GameObject2D *>, 
                 std::greater<std::pair<int, GameObject2D *>>> transparentGameObjects;
        float deltaTime;
        float unscaledDeltaTime;
        float timeScale = 1;

    private:
        glm::mat3 visMatrix;
        glm::mat3 invVisMatrix;
        std::unordered_set<GameObject2D *> toDestroy;
    };
} // namespace engine