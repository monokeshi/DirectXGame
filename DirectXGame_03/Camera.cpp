#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMFLOAT3 eyePos)
{
    // ビュー変換行列の生成
    CreateMatrixView(eyePos);

    // カメラ位置から注視点までの距離を求める
    CalcLength();
}

Camera::~Camera()
{
}

// ビュー変換行列の生成
void Camera::CreateMatrixView(XMFLOAT3 eyePos)
{
    eye = XMFLOAT3(eyePos.x, eyePos.y, eyePos.z);   // 視点座標
    target = XMFLOAT3(0.0f, 0.0f, 0.0f);            // 注視点座標
    up = XMFLOAT3(0.0f, 1.0f, 0.0f);                // 上方向ベクトル
}

// カメラ位置から注視点までの距離を求める
void Camera::CalcLength()
{
    float lengthX = fabsf(fabsf(eye.x) - fabsf(target.x));
    float lengthY = fabsf(fabsf(eye.y) - fabsf(target.y));
    float lengthZ = fabsf(fabsf(eye.z) - fabsf(target.z));
    length = sqrtf(lengthX * lengthX + lengthY * lengthY + lengthZ * lengthZ);
}

// ビュー変換の計算を行う
void Camera::CalcMatView()
{
    matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

// 更新処理
void Camera::Update()
{
    CalcMatView();
}

// 移動
void Camera::MoveX(float move)
{
    eye.x += move;
}

void Camera::MoveY(float move)
{
    eye.y += move;
}

void Camera::MoveZ(float move)
{
    eye.z += move;
}

// 回転
void Camera::RotateX(float angle)
{
    eye.x = -length * sinf(angle);
}

void Camera::RotateY(float angle)
{
    eye.y = -length * sinf(angle);
}

void Camera::RotateZ(float angle)
{
    eye.z = -length * cosf(angle);
}