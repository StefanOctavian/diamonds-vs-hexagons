#include <iostream>
#include "game.h"
#include "gameobject2d.h"
#include "shapes.h"
#include "transform2d.h"
#include "coloreditem.h"

using namespace game;
using namespace engine;

Game::Game()
{
    grid.assign(3, std::vector<std::pair<GameObject2D *, RhombusGun *>>(3));
    meshes["background"] = shapes::CreateSquare("background", glm::vec3(0.9, 0.9, 0.8), -10);
    meshes["gridEnd"] = shapes::CreateSquare("gridEnd", glm::vec3(0.5, 0.4, 0.2), -1);
    meshes["gridSquare"] = shapes::CreateSquare("gridSquare", glm::vec3(0.2, 0.7, 0.2), -1);
    meshes["box"] = shapes::CreateSquare("box", glm::vec3(0, 0, 0), -1, false);
    meshes["lifepoint"] = shapes::CreateHeart("lifepoint", glm::vec3(1, 0.4, 0.757), 0);
    meshes["starpoint"] = shapes::CreateStar("star", glm::vec3(0.8, 0.54, 0.23));
    meshes["star"] = shapes::CreateStar("star", glm::vec3(0.6, 0.25, 0.83), 2);
    meshes["pausemenubg"] = shapes::CreateSquare("pausemenubg", glm::vec3(0.34, 0.34, 0.44), 3);
    meshes["pausebutton"] = shapes::CreateSquare("pausebutton", glm::vec3(0.30, 0.30, 0.40), 3);
    meshes["pauseline"] = shapes::CreateSquare("pausebtnline", glm::vec3(0.8, 0.8, 0.8), 4);
    meshes["sliderthumb"] = shapes::CreateSquare("sliderthumb", glm::vec3(0.34, 0.20, 0.34), 8);
    meshes["sliderpoint"] = shapes::CreateSquare("sliderpoint", glm::vec3(0.20, 0.20, 0.34), 7);
    RhombusGun::InitMeshes();
    HexagonEnemy::InitMeshes();
    ProjectileStar::InitMeshes();

    std::random_device rd;
    rng = std::mt19937(rd());
    starXGenerator = std::uniform_real_distribution<float>(4, logicSpace.width - 4);
    starYGenerator = std::uniform_real_distribution<float>(4, logicSpace.height - 4);
    secondsGeneratorStars = std::binomial_distribution<int>(8, 0.5);
    secondsGeneratorHexagons = std::binomial_distribution<int>(10, 0.7);
    hexagonLaneGenerator = std::uniform_int_distribution<int>(0, 2);
    colorsGenerator = std::discrete_distribution<int>({ 3, 2, 1, 1 });

    cannonBoxes.reserve(4);
    stars.reserve(10);
    lanes.reserve(3);
    lanesHexagons.assign(3, std::unordered_map<ColoredItem::Color, int>({}));
    std::cout << lanesHexagons[0][ColoredItem::Frost] << "\n";
    livesObjects.reserve(3);

    disappearingPhantoms = new GameObject2D(glm::vec2(0), glm::vec2(1));
    gameObjects.insert(disappearingPhantoms);

    LoadShader("RoundedCorners", 
        PATH_JOIN("Game", "shaders", "VertexShader.glsl"), 
        PATH_JOIN("Game", "shaders", "RoundedCornersFragmentShader.glsl"));
}

Game::~Game()
{
    for (auto &gameObject : gameObjects)
        delete gameObject;
    gameObjects.clear();
    stars.clear();
    grid.clear();
    lanes.clear();
    lanesHexagons.clear();
    livesObjects.clear();
    cannonBoxes.clear();
    RhombusGun::ClearMeshes();
    HexagonEnemy::ClearMeshes();
    ProjectileStar::ClearMeshes();
}

void Game::Initialize()
{
    // Setup permanent objects in the scene
    gameObjects.insert(new GameObject2D(
        meshes["background"], 
        glm::vec2(logicSpace.width / 2, logicSpace.height / 2),
        glm::vec2(logicSpace.width, logicSpace.height)
    ));

    SetupBoard();
    SetupGUI();
    SetupPauseMenu();

    secondsToNextStar = (float)secondsGeneratorStars(rng);
    secondsToNextHexagon = (float)secondsGeneratorHexagons(rng);
}

void Game::SetupBoard()
{
    GameObject2D *gridEnd = new GameObject2D(
        meshes["gridEnd"], 
        glm::vec2(4, 17),
        glm::vec2(4, 30)
    );
    gameObjects.insert(gridEnd);

    for (int i = 0; i < 3; ++i) {
        int laneCenter = 28 - 11 * i;
        for (int j = 0; j < 3; ++j) {
            int columnCenter = 12 + 11 * j;
            GameObject2D *tile = new GameObject2D(
                meshes["gridSquare"], 
                glm::vec2(columnCenter, laneCenter),
                glm::vec2(8, 8)
            );
            tile->SetRectHitArea(1, 1, glm::vec2(0));
            gameObjects.insert(tile);
            grid[i][j] = {tile, nullptr};
        }
        GameObject2D *lane = new GameObject2D(
            glm::vec2(logicSpace.x + logicSpace.width, laneCenter),
            glm::vec2(-1, 1)
        );
        gameObjects.insert(lane);
        lanes.push_back(lane);
    }
}

void Game::SetupGUI()
{
    guiContainer = new GameObject2D(glm::vec2(3, 35));

    // cannons
    for (int i = 0; i < 4; ++i) {
        ColoredItem::Color color = (ColoredItem::Color)i;
        GameObject2D *cannonBox = GameObject2D::CreateChild(
            guiContainer,
            meshes["box"], 
            glm::vec2(6 + 14 * i, 10),
            glm::vec2(8, 8), 0
        );
        cannonBox->SetRectHitArea(1, 1, glm::vec2(0));
        GameObject2D *rhombusGun = new RhombusGun(color, glm::vec2(6 + 14 * i, 10), guiContainer);
        cannonBoxes[color] = cannonBox;

        for (int j = 0; j < neededStars(color); ++j) {
            GameObject2D *star = GameObject2D::CreateChild(
                guiContainer,
                meshes["starpoint"], 
                glm::vec2(3 + 14 * i + 3 * j, 4),
                glm::vec2(3, 3), 0
            );
        }
    }

    // lives
    for (int i = 0; i < 3; ++i) {
        GameObject2D *lifepoint = GameObject2D::CreateChild(
            guiContainer,
            meshes["lifepoint"], 
            glm::vec2(62 + 8 * i, 10),
            glm::vec2(6, 5.4), 0
        );
        lifepoint->name = "lifepoint";
        livesObjects.push_back(lifepoint);
    }

    gameObjects.insert(guiContainer);
}

void Game::SetupPauseMenu()
{
    pauseButton = new GameObject2D(
        meshes["pausebutton"], glm::vec2(93, 45), glm::vec2(8, 8)
    );
    pauseButton->SetRectHitArea(1, 1, glm::vec2(0));
    engine::Material pauseButtonMaterial = engine::Material(shaders["RoundedCorners"]);
    pauseButtonMaterial.SetFloat("roundnessX", 0.2f);
    pauseButtonMaterial.SetFloat("roundnessY", 0.2f);
    pauseButton->material = pauseButtonMaterial;
    transparentGameObjects.insert(std::make_pair(2, pauseButton));

    for (int i = 0; i <= 1; ++i) {
        GameObject2D *pauseButtonLine = GameObject2D::CreateChild(
            pauseButton, meshes["pauseline"], 
            glm::vec2(0.3 * i - 0.15, 0), glm::vec2(0.1, 0.5), 0
        );
    }

    glm::vec2 menuScale = glm::vec2(logicSpace.width * 0.7, logicSpace.height * 0.7);
    pauseMenu = new GameObject2D(
        meshes["pausemenubg"], glm::vec2(logicSpace.width / 2, logicSpace.height / 2),
        menuScale
    );
    engine::Material pauseMenuMaterial = engine::Material(shaders["RoundedCorners"]);
    pauseMenuMaterial.SetFloat("roundnessX", 0.1f);
    pauseMenuMaterial.SetFloat("roundnessY", 0.2f);
    pauseMenu->material = pauseMenuMaterial;

    sliders.reserve(4);
    for (int i = 1; i <= 4; ++i) {
        GameObject2D *sliderLineBg = GameObject2D::CreateChild(
            pauseMenu, meshes["pauseline"], 
            glm::vec2(0.1, 0.5 - 0.2 * i), glm::vec2(0.7, 0.03)
        );
        engine::Material sliderLineBgMaterial = engine::Material(shaders["RoundedCorners"]);
        sliderLineBgMaterial.SetFloat("roundnessX", 0.03f);
        sliderLineBgMaterial.SetFloat("roundnessY", 1.0f);
        sliderLineBg->material = sliderLineBgMaterial;
        GameObject2D *sliderLine = GameObject2D::CreateChild(sliderLineBg, 
            glm::vec2(-0.5 + 0.02, 0), glm::vec2(0.96, 1));
        GameObject2D *hexagon = GameObject2D::CreateChild(
            pauseMenu, HexagonEnemy::meshesTopSecondary[(ColoredItem::Color)(i - 1)],
            glm::vec2(-0.35, 0.5 - 0.2 * i)
        );
        hexagon->SetPseudoScale(glm::vec2(5));
        GameObject2D::CreateChild(hexagon, 
            HexagonEnemy::meshesTop[(ColoredItem::Color)(i - 1)], glm::vec2(0),
            glm::vec2(0.7f));

        float sliderX = propabilityWeightsHexagons[i - 1] / 5.0f;
        GameObject2D *sliderThumb = GameObject2D::CreateChild(
            sliderLine, meshes["sliderthumb"], glm::vec2(sliderX, 0.0f)
        );
        sliderThumb->SetPseudoScale(glm::vec2(2));
        sliderThumb->SetCircleHitArea(0.5);
        engine::Material sliderThumbMaterial = engine::Material(shaders["RoundedCorners"]);
        sliderThumbMaterial.SetFloat("roundnessX", 1.0f);
        sliderThumbMaterial.SetFloat("roundnessY", 1.0f);
        sliderThumb->material = sliderThumbMaterial;
        sliders.push_back(sliderThumb);

        for (float px = 0.0f; px <= 1.0f; px += 0.2f) {
            GameObject2D *sliderPoint = GameObject2D::CreateChild(
                sliderLine, meshes["sliderpoint"], glm::vec2(px, 1.5f), glm::vec2(0.01f, 2.0f)
            );
        }
    }
}

// helpers

inline int Game::neededStars(ColoredItem::Color color)
{
    return color + (color <= 2);
}

inline ColoredItem::Color Game::randomColor()
{
    int index = colorsGenerator(rng);
    index = index > 3 ? 3 : index;
    return (ColoredItem::Color)index;
}

inline void Game::hexagonsProbabilityDistribution()
{
    colorsGenerator = std::discrete_distribution<int>(
        propabilityWeightsHexagons.begin(), propabilityWeightsHexagons.end());
}

inline void Game::DestroyHexagon(HexagonEnemy *hexagon)
{
    GameObject2D *phantom = hexagon->InertDeepCopy();
    disappearingPhantoms->AddChild(phantom);
    Destroy(hexagon);
    lanesHexagons[hexagon->laneNumber][hexagon->color] -= 1;
}

// Gameplay

void Game::AddStars()
{
    secondsToNextStar -= deltaTime;
    if (secondsToNextStar > 0) 
        return;

    secondsToNextStar = (float)secondsGeneratorStars(rng);
    // don't spawn stars on top of the pause button
    glm::vec2 forbiddenArea = pauseButton->GetPosition() - glm::vec2(5.0f, 5.0f);
    glm::vec2 position;
    do {
        position = glm::vec2(
            viewSpace.x + starXGenerator(rng), 
            viewSpace.y +  starYGenerator(rng)
        );
        
    } while (position.x > forbiddenArea.x && position.y > forbiddenArea.y);

    GameObject2D *star = new GameObject2D(meshes["star"], position, glm::vec2(5, 5));
    star->name = "star";
    star->SetCircleHitArea(0.5);
    stars.insert(star);
    gameObjects.insert(star);
}

void Game::AddHexagons()
{
    if (HexagonEnemy::speed < 5.9f) {
        secondsToDifficultyIncrease -= deltaTime;
        if (secondsToDifficultyIncrease < 0) {
            secondsToDifficultyIncrease = 10.f;
            HexagonEnemy::speed += 0.4f;
        }
    }

    secondsToNextHexagon -= deltaTime;
    if (secondsToNextHexagon > 0)
        return;

    secondsToNextHexagon = (float)secondsGeneratorHexagons(rng);
    ColoredItem::Color color = randomColor();
    int laneNumber = hexagonLaneGenerator(rng);
    HexagonEnemy *hexagon = new HexagonEnemy(color, glm::vec2(-3.5, 0), lanes[laneNumber]);
    hexagon->laneNumber = laneNumber;
    lanesHexagons[laneNumber][color] += 1;
    hexagon->name = "hexagon";
    hexagon->onPositionChange = [&, hexagon]() {
        if (paused) return;
        if (hexagon->GetLocalPosition().x > logicSpace.width - 5) {
            lives -= 1;
            Destroy(livesObjects[lives]);
            livesObjects.pop_back();
            DestroyHexagon(hexagon);
            if (lives == 0) {
                std::cout << "Game over!\n";
                exit(0);
            }
        }
        for (auto &cannonGameObject : grid[hexagon->laneNumber]) {
            auto &[tile, rhombusGun] = cannonGameObject;
            if (rhombusGun != nullptr && rhombusGun->Collides(hexagon)) {
                Destroy(rhombusGun);
                rhombusGun = nullptr;
                return;
            }
        }
    };
}

void Game::CheckCannons()
{
    for (int laneNumber = 0; laneNumber < 3; ++laneNumber) {
        for (auto &[tile, rhombusGun] : grid[laneNumber]) {
            if (rhombusGun != nullptr && lanesHexagons[laneNumber][rhombusGun->color] > 0) {
                ShootProjectile(rhombusGun, deltaTime);
            }
        }
    }
}

void Game::ShootProjectile(RhombusGun *cannon, float deltaTimeSeconds)
{
    cannon->timeSinceLastShot += deltaTimeSeconds;
    if (cannon->timeSinceLastShot < RhombusGun::rechargeTime)
        return;

    cannon->timeSinceLastShot = 0;
    ColoredItem::Color color = cannon->color;
    glm::vec2 position = cannon->GetPosition() + glm::vec2(2.8f, 0);
    ProjectileStar *projectile = new ProjectileStar(color, position);
    int laneNumber = cannon->laneNumber;
    projectile->onPositionChange = [=]() {
        for (auto &hexagonGameObject : lanes[laneNumber]->GetChildren()) {
            HexagonEnemy *hexagon = (HexagonEnemy *)hexagonGameObject;
            if (hexagon->color == projectile->color && hexagon->Collides(projectile)) {
                Destroy(projectile);
                hexagon->lives -= 1;
                if (hexagon->lives == 0)
                    DestroyHexagon(hexagon);
                return;
            }
            if (projectile->GetPosition().x > logicSpace.width + 2)
                Destroy(projectile);
        }
    };
    gameObjects.insert(projectile);
}

void Game::HandlePhantoms()
{
    const float disappearSpeed = 12.f;
    for (auto phantom : disappearingPhantoms->GetChildren()) {
        glm::vec2 scale = phantom->GetLocalScale();
        scale -= glm::vec2(disappearSpeed * deltaTime);
        scale = glm::max(scale, glm::vec2(0));
        phantom->SetLocalScale(scale);
        if (scale.x == 0 || scale.y == 0) {
            Destroy(phantom);
        }
    }
}

// Main Loop

void Game::Tick()
{
    AddStars();
    AddHexagons();
    CheckCannons();
    HandlePhantoms();
}

// Mouse actions

void Game::PlaceCannon(glm::vec2 mousePos)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            auto [tile, rhombusGun] = grid[i][j];
            if (tile->Contains(mousePos) && rhombusGun == nullptr) {
                rhombusGun = new RhombusGun(draggedObject->color, tile->GetPosition());
                rhombusGun->laneNumber = i;
                gameObjects.insert(rhombusGun);
                grid[i][j].second = rhombusGun;
                int cost = neededStars(draggedObject->color);
                starsCollected -= cost;
                for (int k = 0; k < cost; ++k) {
                    Destroy(starsCounterObjects.top());
                    starsCounterObjects.pop();
                }
                break;
            }
        }
    }
    Destroy(draggedObject);
    draggedObject = nullptr;
    dragging = false;
}

void Game::RemoveCannon(glm::vec2 mousePos)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            auto [tile, rhombusGun] = grid[i][j];
            if (tile->Contains(mousePos) && rhombusGun != nullptr) {
                GameObject2D *phantomCopy = rhombusGun->InertDeepCopy();
                gameObjects.erase(rhombusGun);
                delete rhombusGun;
                grid[i][j].second = nullptr;
                disappearingPhantoms->AddChild(phantomCopy);
                break;
            }
        }
    }
}

void Game::CollectStar(glm::vec2 mousePos)
{
    if (!hitStar->Contains(mousePos))
        return;

    starsCollected += 1;
    stars.erase(hitStar);
    gameObjects.erase(hitStar);
    delete hitStar;
    hitStar = nullptr;
    
    int starCol = (starsCollected - 1) % 10;
    int starRow = (starsCollected - 1) / 10;

    GameObject2D *counterStar = GameObject2D::CreateChild(
        guiContainer,
        meshes["starpoint"], 
        glm::vec2(60 + starCol * 3, 4 - 3 * starRow),
        glm::vec2(3, 3), 0
    );
    starsCounterObjects.push(counterStar);
}

void Game::TogglePause()
{
    paused = !paused;
    timeScale = 1 - timeScale;

    // when unpausing, remove dragged object if any
    // we can't really freeze the dragging action without cancelling it, since
    // otherwise the user could 'teleport' the cannon by dragging it while paused
    if (!paused && dragging) {
        Destroy(draggedObject);
        draggedObject = nullptr;
        dragging = false;
    }
}

void Game::TogglePauseMenu()
{
    pauseMenuActive = !pauseMenuActive;
    if (pauseMenuActive) {
        transparentGameObjects.insert(std::make_pair(3, pauseMenu));
    } else {
        transparentGameObjects.erase(std::make_pair(3, pauseMenu));
    }
}

// Handler functions

void Game::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (!paused && dragging) {
        glm::vec2 mousePos = ScreenCoordsToLogicCoords(mouseX, mouseY);
        draggedObject->SetPosition(mousePos);
    }

    if (pauseMenuActive && slidingIndex != -1) {
        glm::vec2 mousePos = ScreenCoordsToLogicCoords(mouseX, mouseY);
        sliders[slidingIndex]->SetPosition(mousePos);
        float sliderX = sliders[slidingIndex]->GetLocalPosition().x;
        sliderX = glm::clamp(sliderX, 0.0f, 1.0f);
        sliders[slidingIndex]->SetLocalPosition(glm::vec2(sliderX, 0));
    }
}

void Game::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (!IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
        return;

    glm::vec2 mousePos = ScreenCoordsToLogicCoords(mouseX, mouseY);

    if (pauseButton->Contains(mousePos)) {
        if (!paused || pauseMenuActive) TogglePause();
        TogglePauseMenu();
        return;
    }

    if (pauseMenuActive) {
        for (int i = 0; i < 4; ++i) {
            if (sliders[i]->Contains(mousePos)) {
                slidingIndex = i;
                break;
            }
        }
    }

    if (dragging || paused)
        return;

    for (auto star : stars) {
        if (star->Contains(mousePos)) {
            hitStar = star;
            break;
        }
    }

    if (hitStar != nullptr)
        return;

    for (auto [color, cannonBox] : cannonBoxes) {
        if (starsCollected >= neededStars(color) && cannonBox->Contains(mousePos)) {
            dragging = true;
            draggedObject = new RhombusGun(color, mousePos);
            gameObjects.insert(draggedObject);
            break;
        }
    }
}

void Game::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    bool left = IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT);
    bool right = IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT);

    if (!left && !right)
        return;

    if (pauseMenuActive && slidingIndex != -1) {
        float sliderX = sliders[slidingIndex]->GetLocalPosition().x;
        for (float px = 0; px <= 1; px += 0.2f)
            if (sliderX >= px - 0.1f && sliderX < px + 0.1f)
                sliderX = px;
        sliders[slidingIndex]->SetLocalPosition(glm::vec2(sliderX, 0));
        propabilityWeightsHexagons[slidingIndex] = (int)(sliderX * 5);
        hexagonsProbabilityDistribution();
        slidingIndex = -1;
        return;
    }

    if (paused)
        return;

    glm::vec2 mousePos = ScreenCoordsToLogicCoords(mouseX, mouseY);

    if (left) {
        if (dragging) {
            PlaceCannon(mousePos);
        } else if (hitStar != nullptr) {
            CollectStar(mousePos);
        }
    }

    if (right && !dragging)
        RemoveCannon(mousePos);
}

void Game::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_P && !pauseMenuActive)
        TogglePause();
}