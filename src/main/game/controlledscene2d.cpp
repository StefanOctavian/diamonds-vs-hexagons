#include <iostream>
#include "controlledscene2d.h"
#include "transform2d.h"
#include "../wisteria_engine/material.h"

using namespace engine;

ControlledScene2D::ControlledScene2D()
{
    logicSpace = {0, 0, 100, 50};
    gameObjects.reserve(70);
    toDestroy.reserve(10);
}

ControlledScene2D::~ControlledScene2D()
{
    for (auto &gameObject : gameObjects)
        delete gameObject;
    for (auto &pair : transparentGameObjects)
        delete pair.second;
}

void ControlledScene2D::LoadShader(const std::string &name, const std::string &vertexShader,
                                   const std::string &fragmentShader)
{
    Shader *shader = new Shader(name.c_str());
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::MAIN, vertexShader.c_str()), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::MAIN, fragmentShader.c_str()), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}


// 2D visualization matrix - compute logic space to view space transformation
glm::mat3 ControlledScene2D::VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    smin = sx < sy ? sx : sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

// Computes the inverse of the 2D visualization matrix but faster
glm::mat3 ControlledScene2D::InvVizualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    smin = sx < sy ? sx : sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        1.0f / smin, 0.0f, -tx / smin,
        0.0f, 1.0f / smin, -ty / smin,
        0.0f, 0.0f, 1.0f));
}

// Converts screen coordinates to logic coordinates
glm::vec2 ControlledScene2D::ScreenCoordsToLogicCoords(int screenX, int screenY)
{
    int posScreenY = viewSpace.height - screenY;
    return invVisMatrix * glm::vec3(screenX, posScreenY, 1);
}

void ControlledScene2D::SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 clearColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

void ControlledScene2D::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable blending so that we can have transparent colors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ControlledScene2D::Init()
{
    auto camera = GetSceneCamera();
    // set camera position in logic (world, global) coordinates
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Set the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute the 2D window to viewport transformation matrix
    visMatrix = VisualizationTransf2D(logicSpace, viewSpace);
    invVisMatrix = InvVizualizationTransf2D(logicSpace, viewSpace);

    this->Initialize();
}

void ControlledScene2D::Destroy(GameObject2D *gameObject)
{
    toDestroy.insert(gameObject);
}

void ControlledScene2D::Update(float deltaTimeSeconds)
{
    deltaTime = deltaTimeSeconds * timeScale;
    unscaledDeltaTime = deltaTimeSeconds;

    // Draw the objects from the scene
    glEnable(GL_DEPTH_TEST);
    for (auto gameObject : gameObjects) {
        DrawGameObject(gameObject, glm::mat3(1));
    }
    for (auto &pair : transparentGameObjects) {
        DrawGameObject(pair.second, glm::mat3(1));
    }
    glDisable(GL_DEPTH_TEST);

    Tick();
    for (auto gameObject : toDestroy) {
        if (gameObjects.find(gameObject) != gameObjects.end())
            gameObjects.erase(gameObject);
        // there aren't usually many (or any) transparent game objects
        // optimizations are not worth it for now
        else for (auto &pair : transparentGameObjects) {
            if (pair.second == gameObject) {
                transparentGameObjects.erase(pair);
                break;
            }
        }
        delete gameObject;
    }
    toDestroy.clear();
}

void ControlledScene2D::RenderMeshCustomMaterial(Mesh *mesh, Material material, const glm::mat3 &modelViewMatrix)
{
    if (!mesh || !material.shader || !material.shader->GetProgramID())
        return;

    material.Use();
    // RenderMesh2D already sets MVP matrices and binds the VAO
    RenderMesh2D(mesh, material.shader, modelViewMatrix);
}

void ControlledScene2D::DrawGameObject(GameObject2D *gameObject, glm::mat3 parentModelMatrix)
{
    glm::mat3 modelMatrix = parentModelMatrix * 
        transform2D::Translate(gameObject->GetLocalPosition().x, gameObject->GetLocalPosition().y) *
        transform2D::Rotate(gameObject->GetLocalRotation()) *
        transform2D::Scale(gameObject->GetLocalScale().x, gameObject->GetLocalScale().y);

    if (gameObject->mesh) {
        if (gameObject->material.shader) {
            RenderMeshCustomMaterial(gameObject->mesh, gameObject->material, visMatrix * modelMatrix);
        } else {
            RenderMesh2D(gameObject->mesh, shaders["VertexColor"], visMatrix * modelMatrix);
        }
    }

    float objectDeltaTime = gameObject->useUnscaledTime ? unscaledDeltaTime : deltaTime;
    if (gameObject->velocity != glm::vec2(0)) {
        glm::vec2 position = gameObject->GetLocalPosition();
        position += gameObject->velocity * objectDeltaTime;
        gameObject->SetLocalPosition(position);
    }
    if (gameObject->angularVelocity != 0) {
        float rotation = gameObject->GetLocalRotation();
        rotation += gameObject->angularVelocity * objectDeltaTime;
        gameObject->SetLocalRotation(rotation);
    }

    for (auto &child : gameObject->GetChildren()) {
        DrawGameObject(child, modelMatrix);
    }
}

void ControlledScene2D::OnWindowResize(int width, int height)
{
    // Set the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute the 2D window to viewport transformation matrix
    visMatrix = VisualizationTransf2D(logicSpace, viewSpace);
    invVisMatrix = InvVizualizationTransf2D(logicSpace, viewSpace);

    this->OnResizeWindow(width, height);
}
