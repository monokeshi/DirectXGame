#include "Player.h"
#include "Object3D.h"
#include "Camera.h"
#include "InputKey.h"
#include "InputMouse.h"

#include <dinput.h>

using namespace DirectX;

Player::Player(Object3D &object3D,
               Camera &camera):
    object3D(object3D), camera(camera)
{
    eyeAngle = 0.0f;
}

Player::~Player()
{
}

// �ړ�
void Player::Move()
{
    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // �E
    if ( InputKey::GetInstance()->IsKeyPress(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // ��O
    if ( InputKey::GetInstance()->IsKeyPress(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// �J�����̉�]
void Player::RotateCamera()
{
    // ����]
    if ( InputKey::GetInstance()->IsKeyPress(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateBaseY(eyeAngle);
    }

    // �E��]
    if ( InputKey::GetInstance()->IsKeyPress(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateBaseY(eyeAngle);
    }

    // ���]
    if ( InputMouse::GetInstance()->IsMousePress(DIM_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateBaseX(eyeAngle);
    }

    // ����]
    if ( InputMouse::GetInstance()->IsMousePress(DIM_RIGHT) )
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
    if ( InputKey::GetInstance()->IsKeyPress(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // �E
    if ( InputKey::GetInstance()->IsKeyPress(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // ��
    if ( InputKey::GetInstance()->IsKeyPress(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}