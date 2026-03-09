#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <glm/glm.hpp>
#include <vector>

struct Orb
{
    glm::vec3 position;
    bool collected;

    Orb(const glm::vec3& pos)
        : position(pos), collected(false) {
    }
};

class Gameplay
{
private:
    std::vector<Orb> orbs;
    float collectionRadius;
    bool playerHasWon;

public:
    Gameplay();
    void reset();
    void initOrbs();
    void update(const glm::vec3& playerPos);
    void checkSwordWin(const glm::vec3& playerPos, const glm::vec3& swordPos, float swordRadius);

    const std::vector<Orb>& getOrbs() const;
    int getCollectedCount() const;
    int getTotalOrbCount() const;
    bool allOrbsCollected() const;
    bool hasWon() const;
};

#endif