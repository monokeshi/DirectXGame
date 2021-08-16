#pragma once

#include <DirectXMath.h>

class Camera
{
private:
    DirectX::XMFLOAT3 eye;           // 視点座標
    DirectX::XMFLOAT3 target;        // 注視点座標
    DirectX::XMFLOAT3 up;            // 上方向ベクトル
   
    DirectX::XMMATRIX matView;       // ビュー変換行列

    float length;           // カメラと図形の距離

    // ビュー変換行列の生成
    void CreateMatrixView(DirectX::XMFLOAT3 eyePos);

    // カメラ位置から注視点までの距離を求める
    void CalcLength();

    // ビュー変換の計算を行う
    void CalcMatView();

public:
    Camera(DirectX::XMFLOAT3 eyePos);
    ~Camera();

    // 更新処理
    void Update();

    // 移動
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // 回転
    void RotateBaseX(float angle);  // X軸基準
    void RotateBaseZ(float angle);  // Y軸基準
    void RotateBaseY(float angle);  // Z軸基準

    DirectX::XMMATRIX *GetMatView()
    {
        return &matView;
    }
};