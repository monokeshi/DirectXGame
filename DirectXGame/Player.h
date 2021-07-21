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

    // �ړ�
    void Move();

    // �J�����̉�]
    void RotateCamera();

public:
    Player(Object3D &object3D,
                     InputKey &ik,
                     Camera &camera);
    ~Player();

    // �X�V����
    void Update();
};