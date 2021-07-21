#pragma once

class Object3D;
class InputKey;
class Camera;

class Player
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
    Player(Object3D &object3D,
                     InputKey &ik,
                     Camera &camera);
    ~Player();

    // XVˆ—
    void Update();
};