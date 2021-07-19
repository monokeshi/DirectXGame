#include "PlayerController.h"
#include "InputKey.h"
#include "Object3D.h"
#include "InputKey.h"
#include "Camera.h"

#include <dinput.h>

using namespace DirectX;

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

// 移動
void PlayerController::Move()
{
    // 左
    if ( ik.IsKeyPressState(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // 右
    if ( ik.IsKeyPressState(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // 上
    if ( ik.IsKeyPressState(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // 下
    if ( ik.IsKeyPressState(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // 奥
    if ( ik.IsKeyPressState(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // 手前
    if ( ik.IsKeyPressState(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// カメラの回転
void PlayerController::RotateCamera()
{
    // 左回転
    if ( ik.IsKeyPressState(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // Y軸まわりに回転
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // 右回転
    if ( ik.IsKeyPressState(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // Y軸まわりに回転
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // 上回転
    if ( ik.IsKeyPressState(DIK_UP) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // 下回転
    if ( ik.IsKeyPressState(DIK_DOWN) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }
}

// 更新処理
void PlayerController::Update()
{
    Move();
    RotateCamera();

    // ポリゴンの回転
    // 左
    if ( ik.IsKeyPressState(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // 右
    if ( ik.IsKeyPressState(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // 上
    if ( ik.IsKeyPressState(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // 下
    if ( ik.IsKeyPressState(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}