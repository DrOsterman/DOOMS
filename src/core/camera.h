//
// Created by Ryan Matimu on 20/08/2025.
//
# pragma once

#include <bx/math.h>

class Camera {
public:
    /**
     *  Constructs a new Camera object
     * @param position Inital camera position
     * @param target Point the camera is looking at
     * @param up Up vector
     */
    Camera(const bx::Vec3& position = {0.0f, 0.0f, -5.0f},
           const bx::Vec3& target = {0.0f, 0.0f, 0.0f},
           const bx::Vec3& up = {0.0f, 1.0f, 0.0f});

    /**
     * Get the view matrix
     * returns bx::Vec3 The current view matrix
     */
    void getViewMatrix(float* viewMatrix) const;

    // movement methods
    void moveForward(float distance);
    void moveBackward(float distance);
    void moveLeft(float distance);
    void moveRight(float distance);

    // rotation methods
    void rotateYaw(float angle);
    void rotatePitch(float angle);
    void zoom(float distance);

    // getters
    const bx::Vec3& getPosition() const { return m_position; }
    const bx::Vec3& getTarget() const { return m_target; }
    const bx::Vec3& getYaw() const{ return m_yaw; }
    const bx::Vec3& getPitch() const{ return m_pitch; }

    // setters
    void setPosition(const bx::Vec3& position) { m_position = position; }
    void setTarget(const bx::Vec3& target) { m_target = target; }
    void setUp(const bx::Vec3& up) { m_up = up; }
    void setYaw(const bx::Vec3& yaw) { m_yaw = yaw; }
    void setPitch(const bx::Vec3& pitch) { m_pitch = pitch; }
    void setMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }
    void setRotationSpeed(float rotationSpeed) { m_rotationSpeed = rotationSpeed; }
    void setZoomSpeed(float zoomSpeed) { m_zoomSpeed = zoomSpeed; }

private:
    //camera properties
    bx::Vec3 m_position;
    bx::Vec3 m_target;
    bx::Vec3 m_up;
    bx::Vec3 m_yaw;
    bx::Vec3 m_pitch;
    float m_moveSpeed;
    float m_rotationSpeed;
    float m_zoomSpeed;

    float m_minPitch;
    float m_maxPitch;
    float m_minDistance;
    float m_maxDistance;

    // Recalculate chamera position based on target, distace, yaw and pitch
    void updatePosition();
};

