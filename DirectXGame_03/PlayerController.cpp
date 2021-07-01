#include "PlayerController.h"
#include "InputKey.h"
#include "Object3D.h"
#include "InputKey.h"
#include "Camera.h"

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")

PlayerController::PlayerController(Object3D &object3D,
                                   InputKey &ik,
                                   Camera &camera):
    object3D(object3D), ik(ik), camera(camera)
{
    eyeAngle = 0.0f;
}

PlayerController::~PlayerController()
{
}

// �ړ�
void PlayerController::Move()
{
    // ��
    if ( ik.IsKeyPressState(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // �E
    if ( ik.IsKeyPressState(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // ��
    if ( ik.IsKeyPressState(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // ��
    if ( ik.IsKeyPressState(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // ��
    if ( ik.IsKeyPressState(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // ��O
    if ( ik.IsKeyPressState(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// �J�����̉�]
void PlayerController::RotateCamera()
{
    // ����]
    if ( ik.IsKeyPressState(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // �E��]
    if ( ik.IsKeyPressState(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // Y���܂��ɉ�]
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // ���]
    if ( ik.IsKeyPressState(DIK_UP) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // ����]
    if ( ik.IsKeyPressState(DIK_DOWN) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }
}

// �X�V����
void PlayerController::Update()
{
    Move();
    RotateCamera();

    // �|���S���̉�]
    // ��
    if ( ik.IsKeyPressState(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // �E
    if ( ik.IsKeyPressState(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // ��
    if ( ik.IsKeyPressState(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // ��
    if ( ik.IsKeyPressState(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}