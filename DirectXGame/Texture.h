#pragma once

#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>

#include <wrl.h>

class DirectX12Wrapper;
class Render;

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
    Render &render;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    // テクスチャ生成用
    static const int TEX_WIDTH = 256;                          // 横ピクセル数
    static const int IMAGE_DATA_COUNT = TEX_WIDTH * TEX_WIDTH; // 配列要素数
    DirectX::XMFLOAT4 *imageData;

    // テクスチャ読み込み用
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    ID3D12Resource *texBuffObj3D = nullptr;         // オブジェクト用テクスチャバッファ
    std::vector<ID3D12Resource *> texBuffSprite;    // スプライト用テクスチャバッファ
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};      // 設定構造体

    std::vector<CD3DX12_RESOURCE_DESC> texResDescObj3D;
    std::vector<CD3DX12_RESOURCE_DESC> texResDescSprite;

    int obj3DTexturNum;     // オブジェクト用テクスチャ数
    int spriteTextureNum;   // スプライト用テクスチャ数

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // テクスチャバッファの生成
    HRESULT CreateTextureBufferObj3D();     // オブジェクト3D用
    HRESULT CreateTextureBufferSprite();    // スプライト用

    // シェーダーリソースビューの生成
    void CreateShaderResouceViewObject3D(); // オブジェクト3D用
    void CreateShaderResouceViewSprite();   // スプライト用

public:
    Texture(DirectX12Wrapper &dx12, Render &render, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // テクスチャの生成
    D3D12_GPU_DESCRIPTOR_HANDLE CreateTexture(DirectX::XMFLOAT4 color);

    // テクスチャの読み込み
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);

    // スプライト用テクスチャの読み込み
    int LoadSpriteTexture(const wchar_t *fileName);

    const DirectX::XMFLOAT2 &GetObj3DTextureSize(const int &handle) const
    {
        return { static_cast<float>(texResDescObj3D[handle].Width),
                 static_cast<float>(texResDescObj3D[handle].Height) };
    }

    const DirectX::XMFLOAT2 &GetSpriteTextureSize(const int &handle) const
    {
        return { static_cast<float>(texResDescSprite[handle].Width),
                 static_cast<float>(texResDescSprite[handle].Height) };
    }
};