#include "Player.h"
#include "Object3D.h"
#include "Camera.h"

#include <dinput.h>

using namespace DirectX;

Player::Player(Object3D &object3D,
               Camera &camera):
    object3D(object3D), camera(camera)
{
    inputKey = InputKey::GetInstance();
    inputMouse = InputMouse::GetInstance();

    eyeAngle = 0.0f;
}

Player::~Player()
{
    inputKey = nullptr;
    inputMouse = nullptr;
    delete inputKey;
    delete inputMouse;
}

// �ړ�
void Player::Move()
{
    // ��
    if ( inputKey->IsKeyPress(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // �E
    if ( inputKey->IsKeyPress(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // ��
    if ( inputKey->IsKeyPress(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // ��
    if ( inputKey->IsKeyPress(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // ��
    if ( inputKey->IsKeyPress(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // ��O
    if ( inputKey->IsKeyPress(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// �J�����̉�]
void Player::RotateCamera()
{
    // ����]
    if ( inputKey->IsKeyPress(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateBaseY(eyeAngle);
    }

    // �E��]
    if ( inputKey->IsKeyPress(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateBaseY(eyeAngle);
    }

    // ���]
    if ( inputMouse->IsMousePress(DIM_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateBaseX(eyeAngle);
    }

    // ����]
    if ( inputMouse->IsMousePress(DIM_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);
        camera.RotateBaseX(eyeAngle);
    }
}

// �X�V����
void Player::Update()
{
    Move();
    RotateCamera();

    // �|���S���̉�]
    // ��
    if ( inputKey->IsKeyPress(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // �E
    if ( inputKey->IsKeyPress(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // ��
    if ( inputKey->IsKeyPress(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // ��
    if ( inputKey->IsKeyPress(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}