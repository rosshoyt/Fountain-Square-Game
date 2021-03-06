#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "AABB.h"
#include "SphereCollider.h"
#include "../GameData.h"

/**
 * Character movement states
 */
enum CharacterMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    JUMPING,
    RUNNING_START,
    RUNNING_STOP,
};

// Default character camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED_WALKING = 3.0f, SPEED_RUNNING = 6.0f;
const float SENSITIVITY = 0.10f;
const float ZOOM = 45.0f;


/**
 * A camera class that encapsulates the game user's character, processes keyboard input and calculates the corresponding 
 * Euler Angles, Vectors and Matrices for use in OpenGL
 * Also provides collision detection capabilities between the character and other SphereColliders.
 * source: https://learnopengl.com/Getting-started/Camera
 */
class CharacterCamera : public SphereCollider
{
public:
    // CharacterCamera Fields
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;

    // CharacterCamera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool running = false, jumping = false;

    // Constructor with vectors
    CharacterCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED_WALKING), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), SphereCollider(position, 1.0f) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    /**
     * Returns the view matrix calculated using Euler Angles and the LookAt Matrix
     */
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /**
     *  Processes input received from any keyboard-like input system. Accepts input parameter in the form of CharacterMovement Enum
     */
    void processKeyboard(CharacterMovement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            glm::vec3 temp = Front * velocity;
            Position.x += temp.x;
            Position.z += temp.z;
        }
        else if (direction == BACKWARD) {
            glm::vec3 temp = Front * velocity;
            Position.x -= temp.x;
            Position.z -= temp.z;
        }
        else if (direction == LEFT)
            Position -= Right * velocity;
        else if (direction == RIGHT)
            Position += Right * velocity;
        else if (direction == RUNNING_START)
            MovementSpeed = SPEED_RUNNING;
        else if (direction == RUNNING_STOP)
            MovementSpeed = SPEED_WALKING;
        
        // update AABB with new location
        updateSphereColliderPosition(Position);
    }

    /**
     *  Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
     */
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
       
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        
        Yaw += xoffset;
        Pitch += yoffset;
        
        //std::cout << "xoffset: " << xoffset << "  yoffset: " << yoffset << "  Yaw: "<< Yaw <<"  Pitch: " << Pitch <<" \n";
        
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors(); // TODO only update if movement occured
    }

    /**
     * Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
     */
    void processMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:
    /**
     *  Calculates the front vector from the CharacterCamera's (updated) Euler Angles
     */
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};