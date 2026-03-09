#include "sword.h"
#include <glm/gtc/matrix_transform.hpp>

Sword::Sword()
    : pulled(false),
    pulling(false),
    pullProgress(0.0f),
    pullHeight(0.0f),
    basePosition(glm::vec3(0.0f, -0.1f, 0.0f))
{
}

bool Sword::loadModel(const char* path)
{
    return model.load(path);
}

void Sword::triggerPull()
{
    if (!pulled && !pulling)
        pulling = true;
}

void Sword::update(float deltaTime)
{
    if (pulling)
    {
        pullProgress += deltaTime * 0.5f;

        if (pullProgress >= 1.0f)
        {
            pullProgress = 1.0f;
            pulling = false;
            pulled = true;
        }

        pullHeight = pullProgress * 2.5f;
    }
}

bool Sword::isPulled() const
{
    return pulled;
}

void Sword::render(GLSLProgram& prog, float angle)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Place sword into altar
    modelMatrix = glm::translate(modelMatrix, glm::vec3(basePosition.x, basePosition.y + pullHeight, basePosition.z));


    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 10.0f, 0.0f));

    // Slight tilt while embedded
    modelMatrix = glm::rotate(modelMatrix, glm::radians(8.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Spin a little once pulled
    if (pulled)
        modelMatrix = glm::rotate(modelMatrix, angle * 0.4f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Scale to fit shrine
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.12f));

    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

    prog.setUniform("Model", modelMatrix);
    prog.setUniform("NormalMatrix", normalMatrix);
    prog.setUniform("TileFactor", 1.0f);

    model.render();
}