#pragma once

#include "Matrix.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

#include <vector>
#include <wrl.h>

class DirectX12Wrapper;
class Render;

class Object3D
{
private:
    // 定数バッファ用データ構造体
    struct ConstBufferData
    {
        XMFLOAT4 color; // 色(RGBA)
        XMMATRIX mat;   // 3D変換行列
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Matrix *matrix;

    int instanceNum;        // 自身のクラスのインスタンス数

    XMFLOAT3 position;                          // ポジション
    XMFLOAT3 scale;                             // スケーリング倍率
    XMFLOAT3 rotation;                          // 回転角
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer = nullptr;      // 定数バッファ
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;

    // 定数バッファの生成
    HRESULT CreateConstBuffer();

    // 定数バッファへのデータ転送
    HRESULT TransferConstBuffer();
public:
    Object3D(XMFLOAT3 position,
             XMFLOAT3 scale,
             XMFLOAT3 rotation,
             XMMATRIX &matView,
             XMMATRIX *matWorldParent,
             DirectX12Wrapper &dx12,
             Render &render,
             int index,
             bool isPerspective = true);
    ~Object3D();

    // 初期化
    HRESULT Initialize();

    // 毎フレーム更新処理
    void Update();

    // 毎フレーム描画処理
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
              D3D12_VERTEX_BUFFER_VIEW vbView,
              D3D12_INDEX_BUFFER_VIEW ibView,
              int indicesNum);

    // 移動
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // 回転
    void RotateX(float rotation);
    void RotateY(float rotation);
    void RotateZ(float rotation);

    XMMATRIX *GetMatWorld()
    {
        return matrix->GetMatWorld();
    }
};