#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H
#include "camera.h"
#include "helper/scene.h"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "sword.h"
#include "objmodel.h"
#include "gameplay.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLuint vaoHandle;
    GLuint texHandle;
    GLuint mossTex;
    GLuint normalMapTex;
    GLuint treeTex;
    ObjModel keyModel;
    GLuint keyTex;
    GLSLProgram prog;
    float angle;
    Camera camera;
    Gameplay gameplay;
    bool swordAutoPulled;
    bool normalMapEnabled;
    GLuint framebuffer;
    GLuint colorBufferTex;
    GLuint rboDepth;
    int previousCollectedCount;
    float orbPickupPulseTimer;
    float winOverlayAmount;
    GLuint winScreenTex;
    bool rPressedLastFrame;

    void resetScene();
    GLuint quadVAO;
    GLuint quadVBO;

    GLSLProgram screenProg;
    float lastTime;
    float deltaTime;
    Sword sword;
    ObjModel tree;
    bool firstMouse;
    float lastX;
    float lastY;
    GLuint skyboxVAO;
    GLuint skyboxVBO;
    GLuint skyboxTexture;

    GLSLProgram skyProg;
    bool gammaEnabled;
    bool fogEnabled;
    bool moonlightEnabled;
    bool altarLightEnabled;
    bool animationEnabled;
    GLuint brightColorBufferTex;
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];

    GLSLProgram blurProg;
    GLSLProgram bloomFinalProg;

    bool bloomEnabled;
    float bloomStrength;

    void renderOrbs();
    void setupMainFramebuffer();
    void setupBloomBuffers();
    void renderSceneGeometry();
    void renderBloomBlur();
    void renderFinalPass();
    void drawScreenQuad();

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H