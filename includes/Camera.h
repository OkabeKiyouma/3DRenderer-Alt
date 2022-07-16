#ifndef _CAMERA
#define _CAMERA
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Kmath.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.05f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
 public:
  // camera Attributes
  Vec3 Position;
  Vec3 Front;
  Vec3 Up;
  Vec3 Right;
  Vec3 WorldUp;
  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // constructor with vectors
  Camera(Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
         Vec3 up = Vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
      : Front(Vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch)
      : Front(Vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM) {
    Position = Vec3(posX, posY, posZ);
    WorldUp = Vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  Mat4 GetViewMatrix() { return LookAt(Position, Position + Front, Up); }

  // processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void ProcessKeyboard(Camera_Movement direction, Uint64 deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD) Position += Front * velocity;
    if (direction == BACKWARD) Position -= Front * velocity;
    if (direction == LEFT) Position -= Right * velocity;
    if (direction == RIGHT) Position += Right * velocity;
  }

  // processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset,
                            bool constrainPitch = true) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
      if (Pitch > 89.0f) Pitch = 89.0f;
      if (Pitch < -89.0f) Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
  }

  // processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 45.0f) Zoom = 45.0f;
  }

 public:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors() {
    // calculate the new Front vector
    Vec3 front;
    front.x = cos(DegreesToRadians(Yaw)) * cos(DegreesToRadians(Pitch));
    front.y = sin(DegreesToRadians(Pitch));
    front.z = sin(DegreesToRadians(Yaw)) * cos(DegreesToRadians(Pitch));
    Front = NormalizeChecked(front);
    // also re-calculate the Right and Up vector
    Right = NormalizeChecked(CrossProduct(
        Front, WorldUp));  // normalize the vectors, because their length gets
                           // closer to 0 the more you look up or down which
                           // results in slower movement.
    Up = NormalizeChecked(CrossProduct(Right, Front));
  }
};
#endif
