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

    // �ړ�
    void Move();

    // �J�����̉�]
    void RotateCamera();
public:
    PlayerController(Object3D &object3D,
                     InputKey &ik,
                     Camera &camera);
    ~PlayerController();

    // �X�V����
    void Update();
};