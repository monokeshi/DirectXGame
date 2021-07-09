#pragma once

#include <d3d12.h>
#include <DirectXTex.h>

#include <wrl.h>

class DirectX12Wrapper;

class Texture
{
private:
    // テクスチャの初期化方法
    enum class TextureInitialize
    {
        e_CREATE,   // 生成
        e_LOAD      // 読み込み
    };

    TextureInitialize texInit;

    DirectX12Wrapper &dx12;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    // テクスチャ生成用
    const int TEX_WIDTH = 256;                          // 横ピクセル数
    const int IMAGE_DATA_COUNT = TEX_WIDTH * TEX_WIDTH; // 配列要素数
    DirectX::XMFLOAT4 *imageData;

    // テクスチャ読み込み用
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    ID3D12Resource *texBuff = nullptr;                  // テクスチャバッファ
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};          // 設定構造体

    int textureNum; // テクスチャ数

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // テクスチャバッファの生成
    HRESULT CreateTextureBuffer();

    // シェーダーリソースビューの生成
    void CreateShaderResouceView();

public:
    Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // テクスチャの生成
    D3D12_GPU_DESCRIPTOR_HANDLE CreateTexture(DirectX::XMFLOAT4 color);

    // テクスチャの読み込み
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);
};