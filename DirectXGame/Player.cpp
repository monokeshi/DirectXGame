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

// ˆÚ“®
void Player::Move()
{
    // ¶
    if ( InputKey::GetInstance()->IsKeyPress(DIK_A) )
    {
        object3D.MoveX(-1.0f);
    }

    // ‰E
    if ( InputKey::GetInstance()->IsKeyPress(DIK_D) )
    {
        object3D.MoveX(1.0f);
    }

    // ã
    if ( InputKey::GetInstance()->IsKeyPress(DIK_W) )
    {
        object3D.MoveY(1.0f);
    }

    // ‰º
    if ( InputKey::GetInstance()->IsKeyPress(DIK_S) )
    {
        object3D.MoveY(-1.0f);
    }

    // ‰œ
    if ( InputKey::GetInstance()->IsKeyPress(DIK_Q) )
    {
        object3D.MoveZ(1.0f);
    }

    // Žè‘O
    if ( InputKey::GetInstance()->IsKeyPress(DIK_E) )
    {
        object3D.MoveZ(-1.0f);
    }
}

// ƒJƒƒ‰‚Ì‰ñ“]
void Player::RotateCamera()
{
    // ¶‰ñ“]
    if ( InputKey::GetInstance()->IsKeyPress(DIK_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);

        // YŽ²‚Ü‚í‚è‚É‰ñ“]
        camera.RotateBaseY(eyeAngle);
    }

    // ‰E‰ñ“]
    if ( InputKey::GetInstance()->IsKeyPress(DIK_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);

        // YŽ²‚Ü‚í‚è‚É‰ñ“]
        camera.RotateBaseY(eyeAngle);
    }

    // ã‰ñ“]
    if ( InputMouse::GetInstance()->IsMousePress(DIM_LEFT) )
    {
        eyeAngle += XMConvertToRadians(1.0f);
        camera.RotateBaseX(eyeAngle);
    }

    // ‰º‰ñ“]
    if ( InputMouse::GetInstance()->IsMousePress(DIM_RIGHT) )
    {
        eyeAngle -= XMConvertToRadians(1.0f);
        camera.RotateBaseX(eyeAngle);
    }
}

// XVˆ—
void Player::Update()
{
    Move();
    RotateCamera();

    // ƒ|ƒŠƒSƒ“‚Ì‰ñ“]
    // ¶
    if ( InputKey::GetInstance()->IsKeyPress(DIK_Z) )
    {
        object3D.RotateY(0.5f);
    }

    // ‰E
    if ( InputKey::GetInstance()->IsKeyPress(DIK_X) )
    {
        object3D.RotateY(-0.5f);
    }

    // ã
    if ( InputKey::GetInstance()->IsKeyPress(DIK_C) )
    {
        object3D.RotateX(0.5f);
    }

    // ‰º
    if ( InputKey::GetInstance()->IsKeyPress(DIK_V) )
    {
        object3D.RotateX(-0.5f);
    }
}