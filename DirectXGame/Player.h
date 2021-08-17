#pragma once

#include "InputKey.h"
#include "InputMouse.h"

class Object3D;
class Camera;

class Player
{
private:
    Object3D &object3D;
    Camera &camera;

    InputKey *inputKey;
    InputMouse *inputMouse;

    float eyeAngle;

    // ˆÚ“®
    void Move();

    // ƒJƒƒ‰‚Ì‰ñ“]
    void RotateCamera();

public:
    Player(Object3D &object3D,
           Camera &camera);
    ~Player();

    // XVˆ—
    void Update();
};