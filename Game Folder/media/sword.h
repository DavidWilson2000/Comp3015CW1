#ifndef SWORD_H
#define SWORD_H

#include <glm/glm.hpp>
#include "helper/glslprogram.h"
#include "objmodel.h"

class Sword
{
private:
    bool pulled;
    bool pulling;

    float pullProgress;
    float pullHeight;

    glm::vec3 basePosition;
    ObjModel model;

public:
    Sword();

    bool loadModel(const char* path);
    void triggerPull();
    void update(float deltaTime);
    bool isPulled() const;

    void render(GLSLProgram& prog, float angle);
};

#endif