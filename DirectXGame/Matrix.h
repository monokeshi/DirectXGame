#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Matrix
{
private:
    XMFLOAT3 &position;         // ポジション
    XMFLOAT3 &scale;            // スケーリング倍率
    XMFLOAT3 &rotation;         // 回転角
    XMMATRIX &matView;          // ビュー変換行列
    XMMATRIX *matWorldParent;   // 親のワールド行列

    XMMATRIX matProjection;     // 射影変換行列
    XMMATRIX matWorld;          // ワールド変換行列
    XMMATRIX matScale;          // スケーリング行列
    XMMATRIX matRotate;         // 回転行列
    XMMATRIX matTrans;          // 平行移動行列

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
    Matrix(XMFLOAT3 &position,
           XMFLOAT3 &scale,
           XMFLOAT3 &rotation,
           XMMATRIX &matView,
           XMMATRIX *matWorldParent,
           bool isPerspective);
    ~Matrix();

    // 更新処理
    void Update();

    // 行列の合成
    XMMATRIX GetMat()
    {
        return matWorld * matView * matProjection;
    }

    XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};