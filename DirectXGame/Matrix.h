#pragma once

#include <DirectXMath.h>

class Matrix
{
private:
    DirectX::XMFLOAT3 &position;         // ポジション
    DirectX::XMFLOAT3 &scale;            // スケーリング倍率
    DirectX::XMFLOAT3 &rotation;         // 回転角
    DirectX::XMMATRIX &matView;          // ビュー変換行列
    DirectX::XMMATRIX *matWorldParent;   // 親のワールド行列

    DirectX::XMMATRIX matProjection;     // 射影変換行列
    DirectX::XMMATRIX matWorld;          // ワールド変換行列
    DirectX::XMMATRIX matScale;          // スケーリング行列
    DirectX::XMMATRIX matRotate;         // 回転行列
    DirectX::XMMATRIX matTrans;          // 平行移動行列

    // 射影変換行列の計算を行う
    void CalcMatProjection(bool isPerspective); // 透視投影の場合true

    // スケーリング行列の計算を行う
    void CalcMatScale();

    // 回転行列の計算を行う
    void CalcMatRotate();

    // 平行移動行列の計算を行う
    void CalcMatTrans();

    // ワールド変換行列の計算を行う
    void CalcMatWorld();

    // 射影変換以外すべての行列の計算を行う
    void CalcAllMatrix();

public:
    Matrix(DirectX::XMFLOAT3 &position,
           DirectX::XMFLOAT3 &scale,
           DirectX::XMFLOAT3 &rotation,
           DirectX::XMMATRIX &matView,
           DirectX::XMMATRIX *matWorldParent,
           bool isPerspective);
    ~Matrix();

    // 更新処理
    void Update();

    // 行列の合成
    const DirectX::XMMATRIX GetMat() const
    {
        return matWorld * matView * matProjection;
    }

    DirectX::XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};