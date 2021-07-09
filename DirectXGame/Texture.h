#pragma once

#include <d3d12.h>
#include <DirectXTex.h>

#include <wrl.h>

class DirectX12Wrapper;

class Texture
{
private:
    DirectX12Wrapper &dx12;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};
    ID3D12Resource *texBuff = nullptr;                  // テクスチャバッファ
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};          // 設定構造体

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // テクスチャバッファの生成
    HRESULT CreateTextureBuffer();

    // シェーダーリソースビューの生成
    void CreateShaderResouceView();

public:
    Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // テクスチャの読み込み
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);
};