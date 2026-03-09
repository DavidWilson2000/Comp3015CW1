#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class ObjModel
{
private:
    GLuint vao;
    GLuint vbo;
    int vertexCount;

public:
    ObjModel();
    bool load(const std::string& filename);
    void render() const;
};

#endif