#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <vector>

class DirectX12Wrapper;
class Render;

class Sprite
{
private:
    struct  Vertex
    {
        DirectX::XMFLOAT3 pos;  // xyz座標
        DirectX::XMFLOAT2 uv;   // uv座標
    };

    struct ConstBufferData
    {
        DirectX::XMFLOAT4 color; // 色(RGBA)
        DirectX::XMMATRIX mat;   // 3D変換行列
    };

    DirectX12Wrapper &dx12;
    Render &render;

    DirectX::XMFLOAT3 position; // 座標
    float rotation;             // 回転
    DirectX::XMMATRIX matWorld; // ワールド行列
    DirectX::XMFLOAT4 color;    // 色

    std::vector<Vertex> vertices;                       // 頂点データ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffer;  // 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vbView{};                  // 頂点バッファビュー
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // 定数バッファ

    // 行列の更新処理
    void UpdateMatrix();

    // 頂点生成
    void CreateVertex();

    // 頂点バッファの生成
    HRESULT CreateVertBuffer();

    // 頂点バッファへの転送
    HRESULT TransferVertBuffer();

    // 定数バッファの生成
    HRESULT CreateConstBuffer();

    // 定数バッファへのデータ転送
    HRESULT TransferConstBuffer();

public:
    Sprite(DirectX::XMFLOAT3 position,
           float rotation,
           DirectX::XMFLOAT4 color,
           DirectX12Wrapper &dx12,
           Render &render);
    ~Sprite();

    // 初期化処理
    HRESULT Initialize();

    // 更新処理
    void Update();

    // 描画処理
    void Draw(const int &textureNum);
};