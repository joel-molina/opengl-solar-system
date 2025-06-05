#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"
#include "Light.h"

#define numVBOs 2;
#define numIBs 2;


class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void Render();

    Camera* GetCamera() { return m_camera; }

    void UpdateAsteroidBelt(std::vector<glm::mat4>& belt_transform, glm::mat4 localTransform, float dt, int numAsteroids, float radius);

  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_positionAttrib;
    GLint m_instanceTransform;
    GLint m_isInstanced;
    //GLint m_colorAttrib;
    GLint m_normalAttrib;

    GLint m_tcAttrib;
    GLint m_hasTexture;
    GLint m_hasNormalMap;

    GLint m_normalMatrix;


    Light* m_light;

    GLuint m_globalAmbLoc;
    GLuint m_lightALoc;
    GLuint m_lightDLoc;
    GLuint m_lightSLoc;
    GLuint m_lightPosLoc;

    GLuint m_mAmbLoc;
    GLuint m_mDiffLoc;
    GLuint m_mSpecLoc;
    GLuint m_mShineLoc;

    GLuint m_isEmissive;
    GLuint m_emissiveColor;
    GLuint m_emissiveStrength;

    //galaxy
    Sphere* m_galaxy;

    //comet
    Mesh* m_halley_comet;

    //asteroid
    Mesh* m_inner_asteroid;
    Mesh* m_outer_asteroid;
    std::vector<glm::mat4> m_inner_asteroid_transforms;
    std::vector<glm::mat4> m_outer_asteroid_transforms;

    GLuint m_numInnerAsteroids = 1000;
    GLuint m_numOuterAsteroids = 1000;
    GLuint m_instanceVBO;
    GLuint m_instanceVBO2;

    //main solar system
    Sphere* m_sun;
    Sphere* m_mercury;
    Sphere* m_venus;
    Sphere* m_earth;
    Sphere* m_moon;
    Sphere* m_mars;
    Sphere* m_mars_moon1;
    Sphere* m_mars_moon2;
    Sphere* m_jupiter;
    Sphere* m_jupiter_moon1;
    Sphere* m_jupiter_moon2;
    Sphere* m_jupiter_moon3;
    Sphere* m_saturn;
    Sphere* m_saturn_moon1;
    Sphere* m_saturn_moon2;
    Sphere* m_saturn_moon3;
    Sphere* m_saturn_ring;
    Sphere* m_uranus;
    Sphere* m_neptune;

    //dwarf planets
    Sphere* m_ceres;
    Sphere* m_eris;
    Sphere* m_haumea;

    //spaceship
    Mesh* m_starship;
    Mesh* m_starship_fire;



};

#endif /* GRAPHICS_H */
