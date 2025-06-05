#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
    //initalize ship position and direction vectors, for both modes
    shipPos = glm::vec3(0.0f, 0.0f, 10.0f);
    shipFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    shipUp = glm::vec3(0.0f, 1.0f, 0.0f);
    shipRight = glm::normalize(glm::cross(shipFront, shipUp));

    //initialize camera position and direction vectors
    cameraPos = shipPos - shipFront * 10.0f + shipUp * 10.0f;
    cameraFront = glm::normalize(shipPos + cameraPos);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //Positive Y is up

    //initialize view and projection matrices
    view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
    projection = glm::perspective( glm::radians(40.f), //fov
                                 float(w)/float(h), //aspect ratio
                                 0.01f, //near plane
                                 300.0f); //far plane

    //initialize cached variables 
    //exploration mode
    cached_shipPos = shipPos;
    cached_shipFront = shipFront;
    cached_shipUp = shipUp;
    cached_velocity = 0.0f;
    cached_roll = 0.0f;
    cached_fov = 40.0f; 
    cached_angleX = 0.0f;
    cached_angleY = 0.0f;
    
    //observation mode
    glm::vec3 observationStartPos = shipPos + shipFront * 2.0f; //2, arbitrary distance value ahead of the ship.
    cached_obs_shipPos = observationStartPos;
    cached_obs_shipFront = shipFront;
    cached_obs_shipUp = shipUp;
    cached_obs_observationAngleX = 0.0f;
    cached_obs_observationAngleY = 0.0f;
    cached_obs_fov = 40.0f;
    return true;
}

void Camera::Update(double mX, double mY, char key, double yScroll)
{
    //0 [exploration] 1 [observation]
    static bool tPress = false; //prevent debounce
    if (key == 't')
    {
        if (!tPress)
        {
            gameMode = !gameMode;
            tPress = true;
        }
    }
    else
    {
        tPress = false;
    }

    // save/load cached values
    static int prevMode = -1;
    static bool justSwitched = 0;
    if (gameMode != prevMode) //onChange
    {
        if (gameMode == 1) 
        {
            //save exploration state
            cached_shipPos = shipPos;
            cached_shipFront = shipFront;
            cached_shipUp = shipUp;
            cached_velocity = velocity;
            cached_roll = roll;
            cached_angleX = angleX;
            cached_angleY = angleY;
            cached_fov = fov;
            cached_cameraPos = cameraPos;
            cached_cameraFront = cameraFront;
            cached_cameraUp = cameraUp;

            //load observation state
            shipPos = cached_obs_shipPos;
            shipFront = cached_obs_shipFront;
            shipUp = cached_obs_shipUp;
            observationAngleX = cached_obs_observationAngleX;
            observationAngleY = cached_obs_observationAngleY;
            fov = cached_obs_fov;
            cameraPos = cached_obs_cameraPos;
            cameraFront = cached_obs_cameraFront;
            cameraUp = cached_obs_cameraUp;
        }
        else if (gameMode == 0) 
        {
            //save observation state
            cached_obs_shipPos = shipPos;
            cached_obs_shipFront= shipFront;
            cached_obs_shipUp = shipUp;
            cached_obs_observationAngleX = observationAngleX;
            cached_obs_observationAngleY = observationAngleY;
            cached_obs_fov = fov;
            cached_obs_cameraPos = cameraPos;
            cached_obs_cameraFront = cameraFront;
            cached_obs_cameraUp = cameraUp;

            //restore exploration state
            shipPos = cached_shipPos;
            shipFront = cached_shipFront;
            shipUp = cached_shipUp;
            velocity = cached_velocity;
            roll = cached_roll;
            angleX = cached_angleX; 
            angleY = cached_angleY;  
            fov = cached_fov;
            cameraPos = cached_cameraPos;
            cameraFront = cached_cameraFront;
            cameraUp = cached_cameraUp;
        }
        prevMode = gameMode;
        justSwitched = true;
        first = true;
    }

    //if just switched, don't update to let cache update for this frame.
    if (justSwitched)
    {
        justSwitched = false;

        //update view and projection matrices
        int w = 800;
        int h = 600;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), float(w) / float(h), 0.01f, 300.0f);
        return;
    }

    //Exploration mode [move ship]
    if (gameMode == 0)
    {
        //accelerate or brake on 'w' and 's'
        if (key == 'w')
        {
            velocity += acceleration;
        }
        else if (key == 's')
        {
            velocity -= acceleration;
        }
        //roll left and right on 'a' and 'd'
        else if (key == 'a')
        {
            roll -= 1.f;
        }
        else if (key == 'd')
        {
            roll += 1.f;
        }

        //brake
        else if (key == '~') {
            velocity = 0;
        }

        //clamp starship speed
        if (velocity > speedLimit)
        {
            velocity = speedLimit;
        }

        //mouse control (camera rotation)
        //initialize last positions.
        static double lastX = 400, lastY = 300;

        //store last known locations
        if (first)
        {
            lastX = mX;
            lastY = mY;
            first = false;
        }

        //calculate offsets from position
        float xOffset = float(mX - lastX) * 0.3f; //0.3 = sensitivity
        float yOffset = float(lastY - mY) * 0.3f;

        //set last known positions
        lastX = mX;
        lastY = mY;

        //relative angles based on offset
        angleX += xOffset;
        angleY += yOffset;

        //clamp angleY to stop flipping
        if (angleY > 89.0f)
        {
            angleY = 89.0f;
        }
        if (angleY < -89.0f)
        {
            angleY = -89.0f;
        }

        //convert angleX and angleY rotations into a direction vector. Apply onto ship.
        glm::vec3 front;
        front.x = cos(glm::radians(angleX)) * cos(glm::radians(angleY));
        front.y = sin(glm::radians(angleY));
        front.z = sin(glm::radians(angleX)) * cos(glm::radians(angleY));
        shipFront = glm::normalize(front);

        //apply 'a'/'d' roll
        //calculate local ship's right and up relative to world up before rolling
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        shipRight = glm::normalize(glm::cross(shipFront, worldUp));
        shipUp = glm::normalize(glm::cross(shipRight, shipFront));
        //rotation matrix around shipfront, by roll degrees
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), shipFront);
        //update right and up after rolling
        shipRight = glm::normalize(glm::vec3(rollMat * glm::vec4(shipRight, 0.0)));
        shipUp = glm::normalize(glm::cross(shipRight, shipFront));

        //apply 'w'/'s' thrust
        shipPos += shipFront * velocity;

        //set camera position (behind the ship, 3rd person)
        cameraPos = shipPos - shipFront * 4.0f + shipUp * 2.0f;
        cameraFront = glm::normalize(shipPos - cameraPos);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        ////adjust FoV based on mouse scroll
        if (yScroll != 0.0)
        {
            fov -= yScroll;

            //restrict FoV
            if (fov < 10.0f)
            {
                fov = 10.0f;
            }
            if (fov > 90.0f)
            {
                fov = 90.0f;
            }
        }

        //update view and projection.
        int w = 800;
        int h = 600;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), float(w) / float(h), 0.01f, 300.0f); 
    }
    //planetary observation mode
    else
    {
        //move ship forward or backward on 'w' and 's'
        if (key == 'w')
        {
            shipPos += shipFront * speed;
        }
        else if (key == 's')
        {
            shipPos -= shipFront * speed;
        }
        //move ship left or right on 'a' and 'd'
        else if (key == 'a')
        {
            shipPos -= glm::normalize(glm::cross(shipFront, cameraUp)) * speed;
        }
        else if (key == 'd')
        {
            shipPos += glm::normalize(glm::cross(shipFront, cameraUp)) * speed;
        }
        //reset to initialized view
        else if (key == 'r')
        {
            shipPos = glm::vec3(0.0f, 0.0f, 10.0f);
            shipFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
            shipUp = glm::vec3(0.0f, 1.0f, 0.0f);
            observationAngleX = 0.0f;
            observationAngleY = 0.0f;
            fov = 40.0f;
            cameraPos = shipPos + shipFront * 2.0f;
            cameraFront = shipFront;
            cameraUp = shipUp;
            first = true;
        }

        //mouse control (camera rotation)
        //initialize last positions.
        static double lastX = 400, lastY = 300;

        //store last known locations
        if (first)
        {
            lastX = mX;
            lastY = mY;
            first = false;
        }

        //calculate offsets from position
        float xOffset = float(mX - lastX) * 0.3f; //0.3 = sensitivity
        float yOffset = float(lastY - mY) * 0.3f;

        //set last known positions
        lastX = mX;
        lastY = mY;

        //relative angles based on offset
        observationAngleX += xOffset;
        observationAngleY += yOffset;

        //clamp angleY to stop flipping
        if (observationAngleY > 89.0f)
        {
            observationAngleY = 89.0f;
        }
        if (observationAngleY < -89.0f)
        {
            observationAngleY = -89.0f;
        }

        //convert angleX and angleY rotations into a direction vector
        glm::vec3 front;
        front.x = cos(glm::radians(observationAngleX)) * cos(glm::radians(observationAngleY));
        front.y = sin(glm::radians(observationAngleY));
        front.z = sin(glm::radians(observationAngleX)) * cos(glm::radians(observationAngleY));

        //move ship
        shipFront = glm::normalize(front);
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        shipRight = glm::normalize(glm::cross(shipFront, worldUp));
        shipUp = glm::normalize(glm::cross(shipRight, shipFront));

        //keep camera in front of ship (doesn't move)
        cameraPos = shipPos + shipFront * 2.0f;
        cameraFront = shipFront;
        cameraUp = shipUp;

        //adjust FoV based on mouse scroll
        if (yScroll != 0.0)
        {
            fov -= yScroll;

            //restrict FoV
            if (fov < 10.0f)
            {
                fov = 10.0f;
            }
            if (fov > 90.0f)
            {
                fov = 90.0f;
            }
        }

        //update view and projection.
        int w = 800;
        int h = 600;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), 
            float(w) / float(h), 
            0.01f, 
            300.0f); 
    }
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}
