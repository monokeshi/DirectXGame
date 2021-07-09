#include "Matrix.h"
#include "Define.h"

using namespace DirectX;

Matrix::Matrix(XMFLOAT3 &position,
               XMFLOAT3 &scale,
               XMFLOAT3 &rotation,
               XMMATRIX &matView,
               XMMATRIX *matWorldParent,
               bool isPerspective):
    position(position), scale(scale), rotation(rotation), matView(matView), matWorldParent(matWorldParent)
{
    // 射影変換行列の生成
    CalcMatProjection(isPerspective);

    // 射影変換以外すべての行列の計算を行う
    CalcAllMatrix();
}

Matrix::~Matrix()
{
}

// 射影変換の計算を行う
void Matrix::CalcMatProjection(bool isPerspective)
{
    if ( !isPerspective )
    {
        // 平行投影
        matProjection = XMMatrixIdentity();
        matProjection *= XMMatrixOrthographicOffCenterLH(0.0f,
                                                         static_cast<float>(WINDOW_WIDTH),
                                                         static_cast<float>(WINDOW_HEIGHT),
                                                         0.0f,
                                                         0.1f,
                                                         1000.0f);
    }
    else
    {
        // 透視投影
        matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f),                         // 上下画角60度
                                                 static_cast<float>(WINDOW_WIDTH) /
                                                 static_cast<float>(WINDOW_HEIGHT), // アスペクト比
                                                 0.1f,                                              // 前端
                                                 1000.0f);                                          // 奥端
    }
}

// スケーリング行列の計算を行う
void Matrix::CalcMatScale()
{
    matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
}

// 回転行列の計算を行う
void Matrix::CalcMatRotate()
{
    // 2Dの場合はZ軸のみでよい
    matRotate = XMMatrixIdentity();
    matRotate *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
    matRotate *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
    matRotate *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
}

// 平行移動行列の計算を行う
void Matrix::CalcMatTrans()
{
    matTrans = XMMatrixTranslation(position.x, position.y, position.z); // 平行移動行列を再計算
}

// ワールド変換の計算を行う
void Matrix::CalcMatWorld()
{
    matWorld = XMMatrixIdentity();  // ワールド行列は毎フレームリセット
    matWorld *= matScale;           // ワールド行列にスケーリングを反映
    matWorld *= matRotate;          // ワールド行列に回転を反映
    matWorld *= matTrans;           // ワールド行列に平行移動を反映

    // 親ワールド行列がnullではない このオブジェクトは子となる
    if ( matWorldParent != nullptr )
    {
        // 親オブジェクトのワールド行列をかける
        matWorld *= (*matWorldParent);
    }
}

// 射影変換以外すべての行列の計算を行う
void Matrix::CalcAllMatrix()
{
    CalcMatScale();
    CalcMatRotate();
    CalcMatTrans();
    CalcMatWorld();
}

// 更新処理
void Matrix::Update()
{
    // 射影変換以外全ての行列変換
    CalcAllMatrix();
}