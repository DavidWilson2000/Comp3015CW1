#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
using std::string;
#include <iostream>
using std::cerr;
using std::endl;
using std::cout;

#include "helper/glutils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "helper/stb/stb_image.h"

static void logGLCheck(const std::string& label)
{
    GLenum err;
    bool foundError = false;

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL Error] " << label << " | code: " << err << std::endl;
        foundError = true;
    }

    if (!foundError)
    {
        std::cout << "[OK] " << label << std::endl;
    }
}

static void logSection(const std::string& label)
{
    std::cout << "\n========== " << label << " ==========" << std::endl;
}
SceneBasic_Uniform::SceneBasic_Uniform()
    : angle(0.0f),
    framebuffer(0),
    colorBufferTex(0),
    rboDepth(0),
    quadVAO(0),
    quadVBO(0),
    fogEnabled(true),
    moonlightEnabled(true),
    altarLightEnabled(true),
    animationEnabled(true),
    swordAutoPulled(false),
    gammaEnabled(false),
    previousCollectedCount(0),
    orbPickupPulseTimer(0.0f),
    winOverlayAmount(0.0f),
    rPressedLastFrame(false),
    camera(glm::vec3(0.0f, 3.0f, 8.0f)),
    lastTime(0.0f),
    deltaTime(0.0f),
    keyTex(0),
    firstMouse(true),
    lastX(400.0f),
    lastY(300.0f),
    normalMapTex(0),
    treeTex(0),
    normalMapEnabled(true),
    brightColorBufferTex(0),
    bloomEnabled(true),
    bloomStrength(1.0f)
{
    {
        std::cout << "[Init] SceneBasic_Uniform constructed." << std::endl;
    }
}

void SceneBasic_Uniform::initScene()
{
    compile();

    logSection("initScene started");
    std::cout << "[Init] Beginning scene setup..." << std::endl;

    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.04f, 0.04f, 0.06f, 1.0f);
    logGLCheck("Basic OpenGL state setup");

    float positions[] = {
        // Front
        -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,

        // Back
         1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,

         // Left
         -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
         -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,

         // Right
          1.0f,-1.0f, 1.0f,   1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,
          1.0f,-1.0f, 1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f,

          // Top
          -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,
          -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,

          // Bottom
          -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,
          -1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f
    };

    float normals[] = {
        // Front
         0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,

         // Back
          0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,
          0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,

          // Left
          -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
          -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

          // Right
           1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
           1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,

           // Top
            0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,

            // Bottom
             0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,
             0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f
    };

    float texCoords[] = {
        // Front
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,

        // Back
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,

        // Left
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,

        // Right
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,

        // Top
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,

        // Bottom
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f
    };

    float tangents[] = {
        // Front
         1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,

         // Back
         -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

         // Left
          0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
          0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,

          // Right
           0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,
           0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,

           // Top
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,

            // Bottom
             1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f
    };

    GLuint vboHandles[4];
    glGenBuffers(4, vboHandles);
    GLuint positionBufferHandle = vboHandles[0];
    GLuint normalBufferHandle = vboHandles[1];
    GLuint texCoordBufferHandle = vboHandles[2];
    GLuint tangentBufferHandle = vboHandles[3];


    glBindBuffer(GL_ARRAY_BUFFER, tangentBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

#ifdef __APPLE__
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferHandle);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glBindBuffer(GL_ARRAY_BUFFER, tangentBufferHandle);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

#else
    glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(1, normalBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(2, texCoordBufferHandle, 0, sizeof(GLfloat) * 2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(2, 2);

    glBindVertexBuffer(3, tangentBufferHandle, 0, sizeof(GLfloat) * 3);
    glVertexAttribFormat(3, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(3, 3);

#endif

    glBindVertexArray(0);

    std::cout << "[Init] Main cube geometry buffers created." << std::endl;
    logGLCheck("Main cube VAO/VBO setup");

    // -------- Load shrine stone texture --------
    int texWidth, texHeight, texChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load("assets/stone.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!image) {
        cerr << "Failed to load texture: assets/stone.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    GLenum format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);

    std::cout << "[Init] Loaded texture: assets/stone.jpg ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Stone texture upload");

    // -------- Load moss texture --------
    unsigned char* mossImage = stbi_load("assets/moss.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!mossImage) {
        cerr << "Failed to load texture: assets/moss.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &mossTex);
    glBindTexture(GL_TEXTURE_2D, mossTex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, mossImage);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(mossImage);

    std::cout << "[Init] Loaded texture: assets/moss.jpg ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Moss texture upload");

    // -------- Load stone normal map --------
    unsigned char* normalImage = stbi_load("assets/stone_normal.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!normalImage) {
        cerr << "Failed to load texture: assets/stone_normal.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &normalMapTex);
    glBindTexture(GL_TEXTURE_2D, normalMapTex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, normalImage);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(normalImage);

    std::cout << "[Init] Loaded normal map: assets/stone_normal.jpg ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Normal map upload");

    // -------- Load tree texture --------
    unsigned char* treeImage = stbi_load("assets/treecolorpallet.png", &texWidth, &texHeight, &texChannels, 0);

    if (!treeImage) {
        cerr << "Failed to load texture: assets/treecolorpallet.png" << endl;
        exit(EXIT_FAILURE);
    }

    format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &treeTex);
    glBindTexture(GL_TEXTURE_2D, treeTex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, treeImage);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(treeImage);

    std::cout << "[Init] Loaded texture: assets/treecolorpallet.png ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Tree texture upload");

    // -------- Load key texture --------
    unsigned char* keyImage = stbi_load("assets/key.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!keyImage) {
        cerr << "Failed to load texture: assets/key.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &keyTex);
    glBindTexture(GL_TEXTURE_2D, keyTex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, keyImage);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(keyImage);

    std::cout << "[Init] Loaded texture: assets/key.jpg ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Key texture upload");

    // -------- Load win screen overlay --------
    unsigned char* winImage = stbi_load("assets/youwin.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!winImage) {
        cerr << "Failed to load texture: assets/youwin.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    format = GL_RGB;
    if (texChannels == 1) format = GL_RED;
    else if (texChannels == 3) format = GL_RGB;
    else if (texChannels == 4) format = GL_RGBA;

    glGenTextures(1, &winScreenTex);
    glBindTexture(GL_TEXTURE_2D, winScreenTex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, winImage);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(winImage);

    std::cout << "[Init] Loaded overlay texture: assets/youwin.jpg ("
        << texWidth << "x" << texHeight << ", channels=" << texChannels << ")" << std::endl;
    logGLCheck("Win overlay texture upload");

    prog.use();
    prog.setUniform("DiffuseTex", 0);
    prog.setUniform("MossTex", 1);
    prog.setUniform("NormalMap", 2);
    prog.setUniform("TreeTex", 3);
    prog.setUniform("KeyTex", 4);


    // -------- Skybox geometry --------
    float skyboxVertices[] = {
        -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,

        -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,

        -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // -------- Load sky texture --------
    int skyWidth, skyHeight, skyChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* skyData = stbi_load("assets/sky.jpg", &skyWidth, &skyHeight, &skyChannels, 0);

    if (!skyData) {
        cerr << "Failed to load sky texture: assets/sky.jpg" << endl;
        exit(EXIT_FAILURE);
    }

    GLenum skyFormat = GL_RGB;
    if (skyChannels == 1) skyFormat = GL_RED;
    else if (skyChannels == 3) skyFormat = GL_RGB;
    else if (skyChannels == 4) skyFormat = GL_RGBA;

    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, skyFormat, skyWidth, skyHeight, 0, skyFormat, GL_UNSIGNED_BYTE, skyData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(skyData);

    std::cout << "[Init] Loaded sky texture: assets/sky.jpg ("
        << skyWidth << "x" << skyHeight << ", channels=" << skyChannels << ")" << std::endl;
    logGLCheck("Sky texture upload");

    skyProg.use();
    skyProg.setUniform("SkyTexture", 0);

    // -------- Framebuffer setup --------
    setupMainFramebuffer();
    setupBloomBuffers();

    std::cout << "[Init] Framebuffers created." << std::endl;
    logGLCheck("Framebuffer setup complete");

    // -------- Screen quad --------
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    screenProg.use();
    screenProg.setUniform("ScreenTexture", 0);


    if (!sword.loadModel("assets/sword.obj")) {
        cerr << "Failed to load sword model." << endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Init] Loaded model: assets/sword.obj" << std::endl;

    if (!tree.load("assets/tree.obj")) {
        cerr << "Failed to load tree model!" << endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Init] Loaded model: assets/tree.obj" << std::endl;

    if (!keyModel.load("assets/Key.obj")) {
        cerr << "Failed to load key model!" << endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Init] Loaded model: assets/Key.obj" << std::endl;

    previousCollectedCount = gameplay.getCollectedCount();

    std::cout << "[Init] Initial orb count: " << previousCollectedCount << std::endl;
    logGLCheck("Final initScene check");
    std::cout << "[Init] Scene setup complete." << std::endl;
}

void SceneBasic_Uniform::resetScene()
{
    gameplay.reset();

    sword = Sword();
    if (!sword.loadModel("assets/sword.obj"))
    {
        std::cerr << "[Gameplay] Failed to reload sword model during reset." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Gameplay] Sword reset and reloaded." << std::endl;

    swordAutoPulled = false;

    camera = Camera(glm::vec3(0.0f, 3.0f, 8.0f));
    firstMouse = true;

    previousCollectedCount = gameplay.getCollectedCount();
    orbPickupPulseTimer = 0.0f;
    winOverlayAmount = 0.0f;

    angle = 0.0f;

    std::cout << "[Gameplay] Scene reset complete. Orb count reset to "
        << previousCollectedCount << std::endl;
}
void SceneBasic_Uniform::setupMainFramebuffer()
{
    logSection("setupMainFramebuffer");
    std::cout << "[FBO] Creating main framebuffer at " << width << "x" << height << std::endl;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Main scene color
    glGenTextures(1, &colorBufferTex);
    glBindTexture(GL_TEXTURE_2D, colorBufferTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorBufferTex,
        0
    );

    // Bright parts for bloom
    glGenTextures(1, &brightColorBufferTex);
    glBindTexture(GL_TEXTURE_2D, brightColorBufferTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D,
        brightColorBufferTex,
        0
    );

    GLuint attachments[2] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };

    glDrawBuffers(2, attachments);

    // Depth buffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        rboDepth
    );

    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[FBO] Main framebuffer not complete! Status: " << fbStatus << std::endl;
    }
    else
    {
        std::cout << "[FBO] Main framebuffer complete." << std::endl;
    }

    logGLCheck("setupMainFramebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::setupBloomBuffers()
{
    logSection("setupBloomBuffers");
    std::cout << "[Bloom] Creating ping-pong buffers at " << width << "x" << height << std::endl;

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            NULL
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            pingpongColorbuffers[i],
            0
        );

        GLenum pingStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (pingStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[Bloom] Ping-pong framebuffer " << i
                << " not complete! Status: " << pingStatus << std::endl;
        }
        else
        {
            std::cout << "[Bloom] Ping-pong framebuffer " << i << " complete." << std::endl;
        }
    }
    logGLCheck("setupBloomBuffers");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::compile()
{
    logSection("Shader compilation");

    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        std::cout << "[Shader] Linked basic shader program." << std::endl;

        skyProg.compileShader("shader/skybox.vert");
        skyProg.compileShader("shader/skybox.frag");
        skyProg.link();
        std::cout << "[Shader] Linked skybox shader program." << std::endl;

        screenProg.compileShader("shader/screen.vert");
        screenProg.compileShader("shader/screen.frag");
        screenProg.link();
        std::cout << "[Shader] Linked screen shader program." << std::endl;

        blurProg.compileShader("shader/blur.vert");
        blurProg.compileShader("shader/blur.frag");
        blurProg.link();
        std::cout << "[Shader] Linked blur shader program." << std::endl;

        bloomFinalProg.compileShader("shader/bloom_final.vert");
        bloomFinalProg.compileShader("shader/bloom_final.frag");
        bloomFinalProg.link();
        std::cout << "[Shader] Linked bloom final shader program." << std::endl;

        logGLCheck("Shader compilation/linking");
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    GLFWwindow* window = glfwGetCurrentContext();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true;
    }

    deltaTime = t - lastTime;
    lastTime = t;

    // -------- Scene toggle keys --------
    static bool fPressedLastFrame = false;
    static bool mPressedLastFrame = false;
    static bool lPressedLastFrame = false;
    static bool spacePressedLastFrame = false;
    static bool gPressedLastFrame = false;
    static bool nPressedLastFrame = false;
    static bool bPressedLastFrame = false;

    bool rPressedNow = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    bool fPressedNow = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    bool mPressedNow = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
    bool lPressedNow = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
    bool spacePressedNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool gPressedNow = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    bool nPressedNow = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
    bool bPressedNow = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;

    if (rPressedNow && !rPressedLastFrame)
    {
        std::cout << "[Input] R pressed - restarting scene." << std::endl;
        resetScene();
    }

    rPressedLastFrame = rPressedNow;
    if (gPressedNow && !gPressedLastFrame)
        gammaEnabled = !gammaEnabled;

    if (nPressedNow && !nPressedLastFrame)
        normalMapEnabled = !normalMapEnabled;

    if (bPressedNow && !bPressedLastFrame)
        bloomEnabled = !bloomEnabled;


    gPressedLastFrame = gPressedNow;
    nPressedLastFrame = nPressedNow;
    bPressedLastFrame = bPressedNow;

    if (fPressedNow && !fPressedLastFrame)
        fogEnabled = !fogEnabled;

    if (mPressedNow && !mPressedLastFrame)
        moonlightEnabled = !moonlightEnabled;

    if (lPressedNow && !lPressedLastFrame)
        altarLightEnabled = !altarLightEnabled;

    if (spacePressedNow && !spacePressedLastFrame)
        animationEnabled = !animationEnabled;

    fPressedLastFrame = fPressedNow;
    mPressedLastFrame = mPressedNow;
    lPressedLastFrame = lPressedNow;
    spacePressedLastFrame = spacePressedNow;

    // -------- WASD movement --------
    bool forward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool backward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    camera.ProcessKeyboard(forward, backward, left, right, deltaTime);

    // -------- Mouse look --------
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse)
        {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
        }

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos;

        lastX = (float)xpos;
        lastY = (float)ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    gameplay.update(camera.Position);

    int currentCollectedCount = gameplay.getCollectedCount();
    
    if (currentCollectedCount > previousCollectedCount)
    {
        orbPickupPulseTimer = 0.35f;
        std::cout << "[Gameplay] Orb collected. Count: "
            << currentCollectedCount << " / "
            << gameplay.getTotalOrbCount() << std::endl;
    }
    previousCollectedCount = currentCollectedCount;

    if (gameplay.allOrbsCollected() && !sword.isPulled() && !swordAutoPulled)
    {
        std::cout << "[Gameplay] All orbs collected. Triggering sword pull." << std::endl;
        sword.triggerPull();
        swordAutoPulled = true;
    }

    glm::vec3 swordWinPos = glm::vec3(0.0f, 1.8f, 0.0f);
    gameplay.checkSwordWin(camera.Position, swordWinPos, 1.4f);

    if (orbPickupPulseTimer > 0.0f)
    {
        orbPickupPulseTimer -= deltaTime;
        if (orbPickupPulseTimer < 0.0f)
            orbPickupPulseTimer = 0.0f;
    }

    static bool winLogged = false;
    if (gameplay.hasWon() && !winLogged)
    {
        std::cout << "[Gameplay] Player reached sword. WIN state triggered." << std::endl;
        winLogged = true;
    }
    if (!gameplay.hasWon())
    {
        winLogged = false;
    }

    float targetOverlay = gameplay.hasWon() ? 1.0f : 0.0f;
    winOverlayAmount += (targetOverlay - winOverlayAmount) * deltaTime * 3.5f;

    std::string title = "Mystical Shrine - Orbs: " +
        std::to_string(gameplay.getCollectedCount()) + " / " +
        std::to_string(gameplay.getTotalOrbCount());

    if (gameplay.hasWon())
        title += " | YOU WIN - Press R to Restart";
    else if (gameplay.allOrbsCollected())
        title += " | Sword Unlocked!";

    glfwSetWindowTitle(window, title.c_str());

    if (animating() && animationEnabled)
    {
        float rotationSpeed = sword.isPulled() ? 0.5f : 0.35f;
        angle = t * rotationSpeed;
    }

    sword.update(deltaTime);
}

void SceneBasic_Uniform::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 cameraPos = camera.Position;
    view = camera.GetViewMatrix();

    projection = glm::perspective(
        glm::radians(60.0f),
        (float)width / height,
        0.1f,
        100.0f
    );

    // -------- Draw skybox first --------
    glDepthMask(GL_FALSE);

    skyProg.use();

    glm::mat4 skyView = glm::mat4(glm::mat3(view));
    skyProg.setUniform("View", skyView);
    skyProg.setUniform("Projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);
    glActiveTexture(GL_TEXTURE1);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);

    // -------- Draw shrine scene --------
    prog.use();

    prog.setUniform("EmissiveEnabled", false);
    prog.setUniform("EmissiveColor", glm::vec3(0.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHandle);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mossTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalMapTex);

    prog.setUniform("UseAlpha", false);
    prog.setUniform("Alpha", 1.0f);
    prog.setUniform("NormalMapEnabled", normalMapEnabled);
    prog.setUniform("UseFlatColor", false);
    prog.setUniform("UseKeyTexture", false);
    prog.setUniform("UseTreeTexture", false);
    prog.setUniform("FlatColor", glm::vec3(1.0f));
    prog.setUniform("View", view);
    prog.setUniform("Projection", projection);
    prog.setUniform("ViewPos", cameraPos);
    prog.setUniform("AmbientColor", glm::vec3(0.06f, 0.06f, 0.09f));
    prog.setUniform("Shininess", 32.0f);
    prog.setUniform("FogColor", glm::vec3(0.04f, 0.04f, 0.06f));
    prog.setUniform("FogDensity", fogEnabled ? 0.06f : 0.0f);
    prog.setUniform("GammaEnabled", gammaEnabled);
    prog.setUniform("Gamma", 2.2f);
    prog.setUniform("DirLightDirection", glm::vec3(-0.6f, -1.0f, -0.35f));
    if (moonlightEnabled)
        prog.setUniform("DirLightColor", glm::vec3(0.35f, 0.42f, 0.55f));
    else
        prog.setUniform("DirLightColor", glm::vec3(0.0f));

    float flicker = 0.85f + 0.15f * sin(angle * 6.0f);
    prog.setUniform("PointLightPos", glm::vec3(0.0f, 0.8f, 0.0f));
    if (altarLightEnabled)
        prog.setUniform("PointLightColor", glm::vec3(1.0f, 0.58f, 0.22f) * flicker);
    else
        prog.setUniform("PointLightColor", glm::vec3(0.0f));

    bool relicLightEnabled = sword.isPulled();
    prog.setUniform("SpotLightEnabled", relicLightEnabled);

    float relicPulse = 0.85f + 0.15f * sin(glfwGetTime() * 3.0f);
    float activationBoost = sword.isPulled() ? 2.2f : 1.0f;

    prog.setUniform("SpotLightPos", glm::vec3(0.0f, 5.2f, 0.8f));
    prog.setUniform("SpotLightDirection", glm::vec3(0.0f, -1.0f, -0.18f));
    prog.setUniform("SpotLightColor",glm::vec3(1.2f, 1.3f, 1.8f) * relicPulse * activationBoost);
    prog.setUniform("SpotLightCutOff", glm::cos(glm::radians(18.0f)));
    prog.setUniform("SpotLightOuterCutOff", glm::cos(glm::radians(26.0f)));

    glBindVertexArray(vaoHandle);

    // Large forest ground
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.95f, 0.0f));
    model = glm::scale(model, glm::vec3(180.0f, 0.25f, 180.0f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 10.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Ground
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.65f, 0.0f));
    model = glm::scale(model, glm::vec3(7.5f, 0.35f, 7.5f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 5.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Raised shrine platform
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.1f, 0.0f));
    model = glm::scale(model, glm::vec3(4.2f, 0.25f, 4.2f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 3.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Altar
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.4f, 0.6f, 1.4f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ---------- Left pillar ----------

    // Base
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.95f, 0.35f, 0.95f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 1.8f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Shaft
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 0.45f, 0.0f));
    model = glm::scale(model, glm::vec3(0.55f, 3.7f, 0.55f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Cap
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 3.95f, 0.0f));
    model = glm::scale(model, glm::vec3(0.9f, 0.3f, 0.9f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 1.8f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ---------- Right pillar ----------

    // Base
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, -0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.95f, 0.35f, 0.95f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 1.8f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Shaft
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, 0.45f, 0.0f));
    model = glm::scale(model, glm::vec3(0.55f, 3.7f, 0.55f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Cap
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, 3.95f, 0.0f));
    model = glm::scale(model, glm::vec3(0.9f, 0.3f, 0.9f));
    prog.setUniform("Model", model);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    prog.setUniform("TileFactor", 1.8f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

 

    prog.setUniform("TileFactor", 1.0f);

    // -------- Forest trees --------
    prog.setUniform("EmissiveEnabled", false);
    prog.setUniform("EmissiveColor", glm::vec3(0.0f));
    prog.setUniform("NormalMapEnabled", false);
    prog.setUniform("UseFlatColor", false);
    prog.setUniform("UseTreeTexture", true);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, treeTex);

    // Dense forest ring around shrine clearing
    const int treeCount = 50;
    const float innerRadius = 12.5f;
    const float outerRadius = 14.5f;
    const float groundY = -1.65f;

    for (int i = 0; i < treeCount; i++)
    {
        float angleDeg = (360.0f / treeCount) * i;
        float angleRad = glm::radians(angleDeg);

        // Alternate between inner and outer ring for density
        float radius = (i % 2 == 0) ? innerRadius : outerRadius;

        // Add slight natural variation
        radius += sin(i * 2.13f) * 0.9f;

        float x = cos(angleRad) * radius;
        float z = sin(angleRad) * radius;

        // Small random-looking offsets
        x += sin(i * 1.73f) * 0.8f;
        z += cos(i * 2.41f) * 0.8f;

        glm::mat4 treeModel = glm::mat4(1.0f);
        treeModel = glm::translate(treeModel, glm::vec3(x, groundY, z));

        float rotation = glm::radians((37.0f * i) + 15.0f * sin(i));
        treeModel = glm::rotate(treeModel, rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        float scale = 1.45f + 0.35f * (i % 4);
        treeModel = glm::scale(treeModel, glm::vec3(scale));

        prog.setUniform("Model", treeModel);
        prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(treeModel))));

        tree.render();
    }

    prog.setUniform("UseTreeTexture", false);
    prog.setUniform("UseFlatColor", false);
    prog.setUniform("NormalMapEnabled", normalMapEnabled);

    // -------- Collectible orbs --------
    renderOrbs();

    glBindVertexArray(vaoHandle);

    if (sword.isPulled())
    {
        float glowPulse = 0.75f + 0.25f * sin(glfwGetTime() * 4.0f);
        prog.setUniform("EmissiveEnabled", true);
        prog.setUniform("EmissiveColor", glm::vec3(0.55f, 0.65f, 1.2f) * glowPulse);
    }
    else
    {
        prog.setUniform("EmissiveEnabled", false);
        prog.setUniform("EmissiveColor", glm::vec3(0.0f));
    }

    sword.render(prog, angle);

    prog.setUniform("EmissiveEnabled", false);
    prog.setUniform("EmissiveColor", glm::vec3(0.0f));

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderBloomBlur();
    renderFinalPass();
}

void SceneBasic_Uniform::renderOrbs()
{
    const std::vector<Orb>& orbs = gameplay.getOrbs();

    prog.setUniform("UseTreeTexture", false);
    prog.setUniform("UseKeyTexture", false);
    prog.setUniform("UseFlatColor", true);
    prog.setUniform("NormalMapEnabled", false);
    prog.setUniform("TileFactor", 1.0f);

    for (size_t i = 0; i < orbs.size(); i++)
    {
        if (orbs[i].collected)
            continue;

        float time = static_cast<float>(glfwGetTime());
        float bob = 0.25f * sin(time * 2.5f + static_cast<float>(i));
        float pulse = 0.85f + 0.35f * sin(time * 4.0f + static_cast<float>(i));
        float spin = time * 1.8f + static_cast<float>(i);
        float wobble = glm::radians(15.0f + 8.0f * sin(time * 2.0f + static_cast<float>(i)));

        glm::vec3 glowColor;
        switch (i % 4)
        {
        case 0: glowColor = glm::vec3(1.0f, 0.45f, 0.12f); break;
        case 1: glowColor = glm::vec3(1.0f, 0.65f, 0.18f); break;
        case 2: glowColor = glm::vec3(1.0f, 0.35f, 0.08f); break;
        default: glowColor = glm::vec3(1.0f, 0.55f, 0.22f); break;
        }

        glm::vec3 orbPos = orbs[i].position + glm::vec3(0.0f, 0.75f + bob, 0.0f);

        // -----------------------------
        // Transparent flame aura
        // -----------------------------
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        // Outer flame aura
        glm::mat4 auraModel = glm::mat4(1.0f);
        auraModel = glm::translate(auraModel, orbPos + glm::vec3(0.0f, 0.08f * pulse, 0.0f));
        auraModel = glm::rotate(auraModel, spin * 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
        auraModel = glm::scale(auraModel, glm::vec3(
            0.45f + 0.08f * pulse,
            1.05f + 0.20f * pulse,
            0.45f + 0.08f * pulse));

        prog.setUniform("Model", auraModel);
        prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(auraModel))));
        prog.setUniform("UseFlatColor", true);
        prog.setUniform("UseKeyTexture", false);
        prog.setUniform("FlatColor", glowColor * 0.55f);
        prog.setUniform("EmissiveEnabled", true);
        prog.setUniform("EmissiveColor", glowColor * (1.4f * pulse));
        prog.setUniform("UseAlpha", true);
        prog.setUniform("Alpha", 0.18f + 0.08f * pulse);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Inner flame core
        glm::mat4 innerAuraModel = glm::mat4(1.0f);
        innerAuraModel = glm::translate(innerAuraModel, orbPos + glm::vec3(0.0f, 0.05f * pulse, 0.0f));
        innerAuraModel = glm::rotate(innerAuraModel, -spin * 0.9f, glm::vec3(0.0f, 1.0f, 0.0f));
        innerAuraModel = glm::scale(innerAuraModel, glm::vec3(
            0.22f + 0.04f * pulse,
            0.55f + 0.08f * pulse,
            0.22f + 0.04f * pulse));

        prog.setUniform("Model", innerAuraModel);
        prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(innerAuraModel))));
        prog.setUniform("FlatColor", glm::vec3(1.0f, 0.82f, 0.35f));
        prog.setUniform("EmissiveColor", glm::vec3(1.0f, 0.75f, 0.25f) * (2.2f * pulse));
        prog.setUniform("UseAlpha", true);
        prog.setUniform("Alpha", 0.32f + 0.10f * pulse);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -----------------------------
        // Solid key model
        // -----------------------------
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glm::mat4 keyMat = glm::mat4(1.0f);
        keyMat = glm::translate(keyMat, orbPos);
        keyMat = glm::rotate(keyMat, spin, glm::vec3(0.0f, 1.0f, 0.0f));
        keyMat = glm::rotate(keyMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        keyMat = glm::rotate(keyMat, wobble, glm::vec3(0.0f, 0.0f, 1.0f));
        keyMat = glm::scale(keyMat, glm::vec3(0.35f));

        prog.setUniform("Model", keyMat);
        prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(keyMat))));
        prog.setUniform("EmissiveEnabled", true);
        prog.setUniform("EmissiveColor", glowColor * (1.0f * pulse));
        prog.setUniform("UseAlpha", false);
        prog.setUniform("Alpha", 1.0f);
        prog.setUniform("UseFlatColor", false);
        prog.setUniform("UseKeyTexture", true);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, keyTex);

        keyModel.render();
    }

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    prog.setUniform("EmissiveEnabled", false);
    prog.setUniform("EmissiveColor", glm::vec3(0.0f));
    prog.setUniform("UseFlatColor", false);
    prog.setUniform("UseKeyTexture", false);
    prog.setUniform("UseAlpha", false);
    prog.setUniform("Alpha", 1.0f);
    prog.setUniform("NormalMapEnabled", normalMapEnabled);
}

void SceneBasic_Uniform::drawScreenQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SceneBasic_Uniform::renderBloomBlur()
{
    if (!bloomEnabled)
        return;

    bool horizontal = true;
    bool firstIteration = true;
    const unsigned int blurAmount = 10;

    blurProg.use();
    blurProg.setUniform("image", 0);

    for (unsigned int i = 0; i < blurAmount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal ? 0 : 1]);
        glClear(GL_COLOR_BUFFER_BIT);

        blurProg.setUniform("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D,
            firstIteration ? brightColorBufferTex : pingpongColorbuffers[horizontal ? 1 : 0]
        );

        drawScreenQuad();

        horizontal = !horizontal;

        if (firstIteration)
            firstIteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    std::cout << "[Resize] Resizing scene to " << w << "x" << h << std::endl;

    width = w;
    height = h;
    glViewport(0, 0, w, h);

    if (colorBufferTex != 0)
    {
        glBindTexture(GL_TEXTURE_2D, colorBufferTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    if (rboDepth != 0)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    }
    if (brightColorBufferTex != 0)
    {
        glBindTexture(GL_TEXTURE_2D, brightColorBufferTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    for (int i = 0; i < 2; i++)
    {
        if (pingpongColorbuffers[i] != 0)
        {
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }
    }
    logGLCheck("resize");
}

void SceneBasic_Uniform::renderFinalPass()
{
    static bool overlayLogged = false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0.04f, 0.04f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bloomFinalProg.use();
    bloomFinalProg.setUniform("scene", 0);
    bloomFinalProg.setUniform("bloomBlur", 1);
    bloomFinalProg.setUniform("bloomEnabled", bloomEnabled);
    bloomFinalProg.setUniform("bloomStrength", bloomStrength);

    float pickupPulseStrength = orbPickupPulseTimer / 0.35f;
    bloomFinalProg.setUniform("PickupPulseStrength", pickupPulseStrength);
    bloomFinalProg.setUniform("WinOverlayAmount", winOverlayAmount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBufferTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);

    drawScreenQuad();

    if (gameplay.hasWon())
    {
        if (!overlayLogged)
        {
            std::cout << "[Render] Win overlay rendered." << std::endl;
            overlayLogged = true;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        screenProg.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, winScreenTex);

        drawScreenQuad();

        glDisable(GL_BLEND);
    }
    else
    {
        overlayLogged = false;
    }
}
