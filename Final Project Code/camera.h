#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
  public:
    Camera();
    ~Camera();
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();

    void Update(double mX, double mY, char key, double yScroll);
    float getSpeed() { return speed; }

    glm::vec3 GetShipPos()   const { return shipPos; }
    glm::vec3 GetShipFront() const { return shipFront; }
    glm::vec3 GetShipUp()    const { return shipUp; }
    float GetVelocity()    const { return velocity; }

  
  private:

    double x = 0.0;
    double y = 10.0;
    double z = -16.0;

    glm::mat4 projection;
    glm::mat4 view;

    float speed = 0.5f;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float fov = 40.f;

    //Exploration mode (3rd person view on the ship)
    glm::vec3 shipPos;
    glm::vec3 shipFront;
    glm::vec3 shipUp;
    glm::vec3 shipRight;
    //movement
    float roll = 0.0f;
    float pitch = 0.0f; 
    float yaw = 0.0f;
    float velocity = 0.0f; 
    float acceleration = 0.01f; //speeding up and slowing down ship
    float speedLimit = 0.3f;
    float angleX = 0.0f;
    float angleY = 0.0f;
    

    //observation mode (1st person 'freecam' mode)
    float observationAngleX = 0.0f;
    float observationAngleY = 0.0f;

    //gamemode
    bool gameMode = 0;
    bool first = true;

    //caching mechanism
    //exploration
    glm::vec3 cached_shipPos;
    glm::vec3 cached_shipFront;
    glm::vec3 cached_shipUp;
    float cached_velocity;
    float cached_roll;
    float cached_angleX;
    float cached_angleY;
    float cached_fov;
    glm::vec3 cached_cameraPos;
    glm::vec3 cached_cameraFront;
    glm::vec3 cached_cameraUp;
    //observation
    glm::vec3 cached_obs_shipPos;
    glm::vec3 cached_obs_shipFront;
    glm::vec3 cached_obs_shipUp;
    float cached_obs_observationAngleX;
    float cached_obs_observationAngleY;
    float cached_obs_fov;
    glm::vec3 cached_obs_cameraPos;
    glm::vec3 cached_obs_cameraFront;
    glm::vec3 cached_obs_cameraUp;
};

#endif /* CAMERA_H */
