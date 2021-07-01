#pragma once

class Object3D;
class InputKey;
class Camera;

class PlayerController
{
private:
    Object3D &object3D;
    InputKey &ik;
    Camera &camera;

    float eyeAngle;

    // ˆÚ“®
    void Move();

    // ƒJƒƒ‰‚Ì‰ñ“]
    void RotateCamera();
public:
    PlayerController(Object3D &object3D,
                     InputKey &ik,
                     Camera &camera);
    ~PlayerController();

    // XVˆ—
    void Update();
};