#pragma once

#include "Matrix.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <d3dx12.h>

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
        DirectX::XMFLOAT4 color; // 色(RGBA)
        DirectX::XMMATRIX mat;   // 3D変換行列
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Matrix *matrix;

    int instanceNum;        // 自身のクラスのインスタンス数

    DirectX::XMFLOAT3 position;                         // ポジション
    DirectX::XMFLOAT3 scale;                            // スケーリング倍率
    DirectX::XMFLOAT3 rotation;                         // 回転角
    DirectX::XMFLOAT4 color;                            // 色
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // 定数バッファ
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV{};
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV{};

    // 定数バッファの生成
    HRESULT CreateConstBuffer();

    // 定数バッファへのデータ転送
    HRESULT TransferConstBuffer();

public:
    Object3D(DirectX::XMFLOAT3 position,
             DirectX::XMFLOAT3 scale,
             DirectX::XMFLOAT3 rotation,
             DirectX::XMFLOAT4 color,
             DirectX::XMMATRIX &matView,
             DirectX::XMMATRIX *matWorldParent,
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
    void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &gpuDescHandleSRV,
              const D3D12_VERTEX_BUFFER_VIEW &vbView,
              const D3D12_INDEX_BUFFER_VIEW &ibView,
              int indicesNum);

    // 移動
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // 回転
    void RotateX(float rotation);
    void RotateY(float rotation);
    void RotateZ(float rotation);

    DirectX::XMMATRIX *GetMatWorld()
    {
        return matrix->GetMatWorld();
    }
};