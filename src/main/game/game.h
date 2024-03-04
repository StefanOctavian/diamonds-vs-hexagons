#pragma once
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <random>
#include "controlledscene2d.h"
#include "gameobject2d.h"
#include "coloreditem.h"

#include "components/simple_scene.h"

namespace game
{
    class Game : public engine::ControlledScene2D
    {
    public:
        Game();
        ~Game();

    private:
        void Initialize() override;
        void Tick() override;

        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnKeyPress(int key, int mods) override;

        // Set the scene
        void SetupBoard();
        void SetupGUI();
        void SetupPauseMenu();

        // Mouse actions
        void PlaceCannon(glm::vec2 mousePos);
        void RemoveCannon(glm::vec2 mousePos);
        void CollectStar(glm::vec2 mousePos);

        // Gameplay
        void AddStars();
        void AddHexagons();
        void CheckCannons();
        void ShootProjectile(RhombusGun *cannon, float deltaTimeSeconds);
        void HandlePhantoms();
        void TogglePause();
        void TogglePauseMenu();

        // helpers
        inline int neededStars(ColoredItem::Color color);
        inline ColoredItem::Color randomColor();
        inline void hexagonsProbabilityDistribution();
        inline void DestroyHexagon(HexagonEnemy *hexagon);

    protected:
        // gui & board
        GameObject2D *guiContainer = nullptr;
        std::unordered_map<ColoredItem::Color, GameObject2D *> cannonBoxes;
        std::vector<std::vector<std::pair<GameObject2D *, RhombusGun *>>> grid;
        GameObject2D *disappearingPhantoms;

        // drag & drop
        RhombusGun *draggedObject = nullptr;
        bool dragging = false;

        // random number generators
        std::mt19937 rng;
        std::uniform_real_distribution<float> starXGenerator;
        std::uniform_real_distribution<float> starYGenerator;
        std::binomial_distribution<int> secondsGeneratorStars;
        std::binomial_distribution<int> secondsGeneratorHexagons;
        std::uniform_int_distribution<int> hexagonLaneGenerator;
        std::discrete_distribution<int> colorsGenerator;

        // stars
        std::unordered_set<GameObject2D *> stars;
        std::stack<GameObject2D *> starsCounterObjects;
        int starsCollected = 0;
        GameObject2D *hitStar = nullptr;
        float secondsToNextStar;

        // hexagons
        std::vector<std::unordered_map<ColoredItem::Color, int>> lanesHexagons;
        std::vector<GameObject2D *> lanes;
        float secondsToNextHexagon;
        std::vector<int> propabilityWeightsHexagons { 3, 2, 1, 1 };
        float secondsToDifficultyIncrease = 30.f;

        // lives
        int lives = 3;
        std::vector<GameObject2D *> livesObjects;

        // pause
        bool paused = false;
        bool pauseMenuActive = false;
        GameObject2D *pauseButton = nullptr;
        GameObject2D *pauseMenu = nullptr;
        std::vector<GameObject2D *> sliders;
        int slidingIndex = -1;
    };
} // namespace game