#include "Texture.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

#include <d3dx12.h>
#pragma comment(lib, "d3d12.lib")

#include <assert.h>

using namespace DirectX;

Texture::Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap):
    dx12(dx12), basicDescHeap(basicDescHeap)
{
}

Texture::~Texture()
{
}


// テクスチャバッファの生成
HRESULT Texture::CreateTextureBuffer()
{
    const Image *img = scratchImg.GetImage(0, 0, 0);    // 生データ抽出

    // リソース設定
    CD3DX12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format,
                                                                    metadata.width,
                                                                    static_cast<UINT>(metadata.height),
                                                                    static_cast<UINT16>(metadata.arraySize),
                                                                    static_cast<UINT16>(metadata.mipLevels));

    // テクスチャバッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
                                                                                     D3D12_MEMORY_POOL_L0),
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &texResDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,  // テクスチャ用指定
                                                            nullptr,
                                                            IID_PPV_ARGS(&texBuff));
    assert(SUCCEEDED(result));

    // テクスチャバッファにデータ転送
    result = texBuff->WriteToSubresource(0,
                                         nullptr,                              // 全領域へコピー
                                         img->pixels,                          // 元データアドレス
                                         static_cast<UINT>(img->rowPitch),     // 1ラインサイズ
                                         static_cast<UINT>(img->slicePitch));  // 全サイズ

    return result;
}

// シェーダーリソースビューの生成
void Texture::CreateShaderResouceView()
{
    // デスクリプタヒープの先頭ハンドルを取得
    cpuDescHandleSRV = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
    gpuDescHandleSRV = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

    // ハンドルのアドレスを進める
    static int textureNum = 0;
    UINT descHandleIncrementSize = dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + textureNum) * descHandleIncrementSize;
    gpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + textureNum) * descHandleIncrementSize;

    ++textureNum;
    // テクスチャ数がデスクリプタヒープで設定したテクスチャバッファ数を上回らない
    if ( textureNum > TEXTURE_BUFFER_NUM )
    {
        assert(0);
        return;
    }

    // シェーダーリソースビュー設定
    srvDesc.Format = metadata.format;                       // RGBA
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // ヒープの2番目にシェーダーリソースビュー作成
    dx12.GetDevice()->CreateShaderResourceView(texBuff,     // ビューと関連付けるバッファ
                                               &srvDesc,    //テクスチャ設定情報
                                               cpuDescHandleSRV);
}

// テクスチャの読み込み
D3D12_GPU_DESCRIPTOR_HANDLE Texture::LoadTexture(const wchar_t *fileName)
{
    // 画像データ
    // 読み込み
    auto result = LoadFromWICFile(fileName, // 画像ファイル
                                  WIC_FLAGS_NONE,
                                  &metadata,
                                  scratchImg);
    assert(SUCCEEDED(result));

    // テクスチャバッファ
    result = CreateTextureBuffer();
    assert(SUCCEEDED(result));

    // シェーダーリソースビュー
    CreateShaderResouceView();

    return gpuDescHandleSRV;
}