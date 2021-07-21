#include "Player.h"
#include "Object3D.h"
#include "InputKey.h"
#include "Camera.h"

#include <dinput.h>

using namespace DirectX;

Player::Player(Object3D &object3D,
               InputKey &ik,
               Camera &camera):
    object3D(object3D), ik(ik), camera(camera)
{
    eyeAngle = 0.0f;
}

Player::~Player()
{
}

// ˆÚ“®
void Player::Move()
{
    // ¶
    if ( ik.IsKeyPressState(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // ‰E
    if ( ik.IsKeyPressState(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // ã
    if ( ik.IsKeyPressState(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // ‰º
    if ( ik.IsKeyPressState(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // ‰œ
    if ( ik.IsKeyPressState(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // è‘O
    if ( ik.IsKeyPressState(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// ƒJƒƒ‰‚Ì‰ñ“]
void Player::RotateCamera()
{
    // ¶‰ñ“]
    if ( ik.IsKeyPressState(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // Y²‚Ü‚í‚è‚É‰ñ“]
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // ‰E‰ñ“]
    if ( ik.IsKeyPressState(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // Y²‚Ü‚í‚è‚É‰ñ“]
        camera.RotateX(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // ã‰ñ“]
    if ( ik.IsKeyPressState(DIK_UP) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }

    // ‰º‰ñ“]
    if ( ik.IsKeyPressState(DIK_DOWN) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);
        camera.RotateY(eyeAngle);
        camera.RotateZ(eyeAngle);
    }
}

// XVˆ—
void Player::Update()
{
    Move();
    RotateCamera();

    // ƒ|ƒŠƒSƒ“‚Ì‰ñ“]
    // ¶
    if ( ik.IsKeyPressState(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // ‰E
    if ( ik.IsKeyPressState(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // ã
    if ( ik.IsKeyPressState(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // ‰º
    if ( ik.IsKeyPressState(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}