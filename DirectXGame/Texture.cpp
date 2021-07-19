#include "Texture.h"
#include "DirectX12Wrapper.h"
#include "Define.h"
#include "Render.h"

#include <assert.h>

using namespace DirectX;

Texture::Texture(DirectX12Wrapper &dx12, Render &render, ID3D12DescriptorHeap *basicDescHeap):
    dx12(dx12), render(render), basicDescHeap(basicDescHeap)
{
    obj3DTexturNum = 0;
    spriteTextureNum = 0;
}

Texture::~Texture()
{
}


// オブジェクト3D用テクスチャバッファの生成
HRESULT Texture::CreateTextureBufferObj3D()
{
    HRESULT result = S_FALSE;
    CD3DX12_RESOURCE_DESC texResDesc;

    if ( texInit == TextureInitialize::e_CREATE )
    {
        // リソース設定
        texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT,
                                                  static_cast<UINT>(TEX_WIDTH),
                                                  static_cast<UINT>(TEX_WIDTH),
                                                  1,
                                                  1);

        // テクスチャバッファの生成
        result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
                                                                                    D3D12_MEMORY_POOL_L0),
                                                           D3D12_HEAP_FLAG_NONE,
                                                           &texResDesc,
                                                           D3D12_RESOURCE_STATE_GENERIC_READ,  // テクスチャ用指定
                                                           nullptr,
                                                           IID_PPV_ARGS(&texBuffObj3D));
        assert(SUCCEEDED(result));

        // テクスチャバッファにデータ転送
        result = texBuffObj3D->WriteToSubresource(0,
                                                  nullptr,                               // 全領域へコピー
                                                  imageData,                             // 元データアドレス
                                                  sizeof(XMFLOAT4) * TEX_WIDTH,          // 1ラインサイズ
                                                  sizeof(XMFLOAT4) * IMAGE_DATA_COUNT);  // 全サイズ
    }
    else if ( texInit == TextureInitialize::e_LOAD )
    {
        const Image *img = scratchImg.GetImage(0, 0, 0);    // 生データ抽出

        // リソース設定
        texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format,
                                                  metadata.width,
                                                  static_cast<UINT>(metadata.height),
                                                  static_cast<UINT16>(metadata.arraySize),
                                                  static_cast<UINT16>(metadata.mipLevels));

        // テクスチャバッファの生成
        result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
                                                                                    D3D12_MEMORY_POOL_L0),
                                                           D3D12_HEAP_FLAG_NONE,
                                                           &texResDesc,
                                                           D3D12_RESOURCE_STATE_GENERIC_READ,  // テクスチャ用指定
                                                           nullptr,
                                                           IID_PPV_ARGS(&texBuffObj3D));
        assert(SUCCEEDED(result));

        // テクスチャバッファにデータ転送
        result = texBuffObj3D->WriteToSubresource(0,
                                                  nullptr,                              // 全領域へコピー
                                                  img->pixels,                          // 元データアドレス
                                                  static_cast<UINT>(img->rowPitch),     // 1ラインサイズ
                                                  static_cast<UINT>(img->slicePitch));  // 全サイズ
    }

    texResDescObj3D.push_back(texResDesc);

    return result;
}

// スプライト用テクスチャバッファの生成
HRESULT Texture::CreateTextureBufferSprite()
{
    const Image *img = scratchImg.GetImage(0, 0, 0);    // 生データ抽出

    // リソース設定
    CD3DX12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format,
                                                                    metadata.width,
                                                                    static_cast<UINT>(metadata.height),
                                                                    static_cast<UINT16>(metadata.arraySize),
                                                                    static_cast<UINT16>(metadata.mipLevels));

    ID3D12Resource *tempBuff = nullptr;

    // テクスチャバッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
                                                                                     D3D12_MEMORY_POOL_L0),
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &texResDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,  // テクスチャ用指定
                                                            nullptr,
                                                            IID_PPV_ARGS(&tempBuff));
    assert(SUCCEEDED(result));

    // テクスチャバッファにデータ転送
    result = tempBuff->WriteToSubresource(0,
                                          nullptr,                              // 全領域へコピー
                                          img->pixels,                          // 元データアドレス
                                          static_cast<UINT>(img->rowPitch),     // 1ラインサイズ
                                          static_cast<UINT>(img->slicePitch));  // 全サイズ

    texResDescSprite.push_back(texResDesc);
    texBuffSprite.push_back(tempBuff);

    return result;
}

// シェーダーリソースビューの生成
void Texture::CreateShaderResouceViewObject3D()
{
    // デスクリプタヒープの先頭ハンドルを取得
    cpuDescHandleSRV = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
    gpuDescHandleSRV = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

    // ハンドルのアドレスを進める
    UINT descHandleIncrementSize = dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + obj3DTexturNum) * descHandleIncrementSize;
    gpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + obj3DTexturNum) * descHandleIncrementSize;

    ++obj3DTexturNum;
    // テクスチャ数がデスクリプタヒープで設定したテクスチャバッファ数を上回らない
    if ( obj3DTexturNum > TEXTURE_BUFFER_NUM )
    {
        assert(0);
        return;
    }

    // シェーダーリソースビュー設定
    if ( texInit == TextureInitialize::e_CREATE )
    {
        srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;    // RGBA
    }
    else if ( texInit == TextureInitialize::e_LOAD )
    {
        srvDesc.Format = metadata.format;                   // RGBA
    }

    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // ヒープにシェーダーリソースビュー作成
    dx12.GetDevice()->CreateShaderResourceView(texBuffObj3D,    // ビューと関連付けるバッファ
                                               &srvDesc,        //テクスチャ設定情報
                                               cpuDescHandleSRV);
}

// スプライト用シェーダーリソースビューの生成
void Texture::CreateShaderResouceViewSprite()
{
    // シェーダーリソースビュー設定
    srvDesc.Format = metadata.format;                       // RGBA
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // ヒープにシェーダーリソースビュー作成
    dx12.GetDevice()->CreateShaderResourceView(texBuffSprite[spriteTextureNum], // ビューと関連付けるバッファ
                                               &srvDesc,                        //テクスチャ設定情報
                                               CD3DX12_CPU_DESCRIPTOR_HANDLE(render.GetSpriteDescHeap()->GetCPUDescriptorHandleForHeapStart(),
                                                                             spriteTextureNum,
                                                                             dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

// テクスチャの生成
D3D12_GPU_DESCRIPTOR_HANDLE Texture::CreateTexture(XMFLOAT4 color)
{
    texInit = TextureInitialize::e_CREATE;
    imageData = new XMFLOAT4[IMAGE_DATA_COUNT];

    // 全ピクセルの色を初期化
    for ( int i = 0; i < IMAGE_DATA_COUNT; ++i )
    {
        imageData[i] = color;
    }

    // テクスチャバッファ
    auto result = CreateTextureBufferObj3D();
    assert(SUCCEEDED(result));

    // シェーダーリソースビュー
    CreateShaderResouceViewObject3D();

    delete[] imageData;

    return gpuDescHandleSRV;
}

// テクスチャの読み込み
D3D12_GPU_DESCRIPTOR_HANDLE Texture::LoadTexture(const wchar_t *fileName)
{
    texInit = TextureInitialize::e_LOAD;

    // 読み込み
    auto result = LoadFromWICFile(fileName, // 画像ファイル
                                  WIC_FLAGS_NONE,
                                  &metadata,
                                  scratchImg);
    assert(SUCCEEDED(result));

    // テクスチャバッファの生成
    result = CreateTextureBufferObj3D();
    assert(SUCCEEDED(result));

    // シェーダーリソースビュー
    CreateShaderResouceViewObject3D();

    return gpuDescHandleSRV;
}

// スプライト用テクスチャの読み込み
int Texture::LoadSpriteTexture(const wchar_t *fileName)
{
    texInit = TextureInitialize::e_LOAD;

    // 読み込み
    auto result = LoadFromWICFile(fileName, // 画像ファイル
                                  WIC_FLAGS_NONE,
                                  &metadata,
                                  scratchImg);
    assert(SUCCEEDED(result));

    // テクスチャバッファの生成
    result = CreateTextureBufferSprite();
    assert(SUCCEEDED(result));

    // シェーダーリソースビュー
    CreateShaderResouceViewSprite();

    ++spriteTextureNum;

    return spriteTextureNum - 1;
}