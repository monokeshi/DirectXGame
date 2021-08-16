#pragma once

class Object3D;
class Camera;

class Player
{
private:
    Object3D &object3D;
    Camera &camera;

    float eyeAngle;

    // �ړ�
    void Move();

    // �J�����̉�]
    void RotateCamera();

public:
    Player(Object3D &object3D,
           Camera &camera);
    ~Player();

    // �X�V����
    void Update();
};