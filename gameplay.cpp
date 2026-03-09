#include "gameplay.h"
#include <glm/gtx/norm.hpp>
#include <random>
#include <cmath>

Gameplay::Gameplay()
    : collectionRadius(1.2f), playerHasWon(false)
{
    initOrbs();
}

void Gameplay::initOrbs()
{
    orbs.clear();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> radiusDist(14.5f, 17.0f);
    std::uniform_real_distribution<float> yDist(-0.15f, 0.15f);

    const int orbCount = 4;

    for (int i = 0; i < orbCount; i++)
    {
        bool validPosition = false;
        glm::vec3 pos(0.0f);

        while (!validPosition)
        {
            float angle = angleDist(gen);
            float radius = radiusDist(gen);

            pos.x = std::cos(angle) * radius;
            pos.z = std::sin(angle) * radius;
            pos.y = -0.2f + yDist(gen);

            validPosition = true;

            for (const auto& orb : orbs)
            {
                float dx = orb.position.x - pos.x;
                float dz = orb.position.z - pos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < 6.25f) // minimum spacing of about 2.5 units
                {
                    validPosition = false;
                    break;
                }
            }
        }

        orbs.push_back(Orb(pos));
    }
}

void Gameplay::update(const glm::vec3& playerPos)
{
    float radiusSq = collectionRadius * collectionRadius;

    for (auto& orb : orbs)
    {
        if (orb.collected)
            continue;

        float distSq = glm::distance2(playerPos, orb.position);
        if (distSq <= radiusSq)
        {
            orb.collected = true;
        }
    }
}

void Gameplay::checkSwordWin(const glm::vec3& playerPos, const glm::vec3& swordPos, float swordRadius)
{
    if (playerHasWon || !allOrbsCollected())
        return;

    glm::vec3 diff = playerPos - swordPos;
    float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

    if (distSq <= swordRadius * swordRadius)
    {
        playerHasWon = true;
    }
}

const std::vector<Orb>& Gameplay::getOrbs() const
{
    return orbs;
}

int Gameplay::getCollectedCount() const
{
    int count = 0;
    for (const auto& orb : orbs)
    {
        if (orb.collected)
            count++;
    }
    return count;
}

int Gameplay::getTotalOrbCount() const
{
    return static_cast<int>(orbs.size());
}

bool Gameplay::allOrbsCollected() const
{
    return getCollectedCount() == getTotalOrbCount();
}

bool Gameplay::hasWon() const
{
    return playerHasWon;
}