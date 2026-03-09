#include "objmodel.h"

#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;
using std::ifstream;
using std::stringstream;
using std::cerr;
using std::endl;

ObjModel::ObjModel() : vao(0), vbo(0), vertexCount(0) {}

bool ObjModel::load(const std::string& filename)
{
    vector<glm::vec3> positions;
    vector<glm::vec2> texcoords;
    vector<glm::vec3> normals;

    vector<float> vertexData;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open OBJ file: " << filename << endl;
        return false;
    }

    string line;
    while (std::getline(file, line)) {
        stringstream ss(line);
        string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::vector<std::string> faceVerts;
            std::string vert;

            while (ss >> vert) {
                faceVerts.push_back(vert);
            }

            if (faceVerts.size() < 3)
                continue;

            auto pushVertex = [&](const std::string& vstr)
                {
                    std::stringstream vs(vstr);
                    std::string pStr, tStr, nStr;

                    std::getline(vs, pStr, '/');
                    std::getline(vs, tStr, '/');
                    std::getline(vs, nStr, '/');

                    int pIndex = std::stoi(pStr) - 1;
                    int tIndex = tStr.empty() ? -1 : std::stoi(tStr) - 1;
                    int nIndex = nStr.empty() ? -1 : std::stoi(nStr) - 1;

                    glm::vec3 pos = positions[pIndex];
                    glm::vec2 uv = (tIndex >= 0) ? texcoords[tIndex] : glm::vec2(0.0f);
                    glm::vec3 normal = (nIndex >= 0) ? normals[nIndex] : glm::vec3(0.0f, 1.0f, 0.0f);

                    vertexData.push_back(pos.x);
                    vertexData.push_back(pos.y);
                    vertexData.push_back(pos.z);

                    vertexData.push_back(normal.x);
                    vertexData.push_back(normal.y);
                    vertexData.push_back(normal.z);

                    vertexData.push_back(uv.x);
                    vertexData.push_back(uv.y);
                };

            for (size_t i = 1; i + 1 < faceVerts.size(); ++i) {
                pushVertex(faceVerts[0]);
                pushVertex(faceVerts[i]);
                pushVertex(faceVerts[i + 1]);
            }
        }
    }

    vertexCount = static_cast<int>(vertexData.size() / 8);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    return true;
}

void ObjModel::render() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}