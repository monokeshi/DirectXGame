#include "Sprite.h"
#include "DirectX12Wrapper.h"
#include "Define.h"
#include "Render.h"
#include "Texture.h"

#include <d3dx12.h>

using namespace DirectX;

Sprite::Sprite(
    DirectX::XMFLOAT3 position,
    float rotation,
    DirectX::XMFLOAT4 color,
    DirectX::XMFLOAT2 size,
    DirectX::XMMATRIX *matWorldParent,
    DirectX12Wrapper &dx12,
    Render &render,
    Texture &texture):
    position(position), rotation(rotation), color(color), size(size), matWorld(matWorld), dx12(dx12), render(render), texture(texture)
{
    anchorPoint = { 0.5f,0.5f };
    isFlipX = isFlipY = false;
    isSetTexResolutionOnlyOnce = true;
    //dirtyFlag = false;

    UpdateMatrix();
}

Sprite::~Sprite()
{
}

// 行列の更新処理
void Sprite::UpdateMatrix()
{
    matWorld = XMMatrixIdentity();                                          // 単位行列
    matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));            // Z軸回転
    matWorld *= XMMatrixTranslation(position.x, position.y, position.z);    // 平行移動

    // 親ワールド行列がnullではない このオブジェクトは子となる
    if ( matWorldParent != nullptr )
    {
        // 親オブジェクトのワールド行列をかける
        matWorld *= (*matWorldParent);
    }
}

// 頂点生成
void Sprite::CreateVertex()
{
    vertices =
    {
        /*      X       Y     Z         U     V      */
        {{   0.0f, 100.0f, 0.0f }, { 0.0f, 1.0f }},  // 左下
        {{   0.0f,   0.0f, 0.0f }, { 0.0f, 0.0f }},  // 左上
        {{ 100.0f, 100.0f, 0.0f }, { 1.0f, 1.0f }},  // 右下
        {{ 100.0f,   0.0f, 0.0f }, { 1.0f, 0.0f }},  // 右上
    };

    //enum
    //{
    //    LB, // 左下
    //    LT, // 左上
    //    RB, // 右下
    //    RT  // 右上
    //};

    //vertices[LB].pos = { 0.0f, size.y, 0.0f };      // 左下
    //vertices[LT].pos = { 0.0f,   0.0f, 0.0f };      // 左上
    //vertices[RB].pos = { size.x, size.y, 0.0f };    // 右下
    //vertices[RT].pos = { size.x,   0.0f, 0.0f };    // 右上
}

// 頂点バッファの生成
HRESULT Sprite::CreateVertBuffer()
{
    //// 頂点バッファの生成
    //auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
    //                                                        D3D12_HEAP_FLAG_NONE,
    //                                                        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
    //                                                        D3D12_RESOURCE_STATE_GENERIC_READ,
    //                                                        nullptr,
    //                                                        IID_PPV_ARGS(&vertBuffer));
    //assert(SUCCEEDED(result));

    //// 頂点バッファビューの作成
    //vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
    //vbView.SizeInBytes = sizeof(vertices);
    //vbView.StrideInBytes = sizeof(vertices[0]);

    // 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
    UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

    // 頂点バッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&vertBuffer));
    assert(SUCCEEDED(result));

    // 頂点バッファビューの作成
    vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeVB;
    vbView.StrideInBytes = sizeof(Vertex);

    return result;
}

// 頂点バッファの転送
HRESULT Sprite::TransferVertBuffer()
{
    // GPU上のバッファに対応した仮想メモリを取得
    Vertex *vertMap = nullptr;
    auto result = vertBuffer->Map(0, nullptr, (void **)&vertMap);
    assert(SUCCEEDED(result));

    // 全頂点に対して
    for ( int i = 0; i < vertices.size(); ++i )
    {
        vertMap[i] = vertices[i]; // コピー
    }

    // マップを解除
    vertBuffer->Unmap(0, nullptr);

    return result;
}

// 定数バッファの生成
HRESULT Sprite::CreateConstBuffer()
{
    UINT sizeCB = static_cast<UINT>((sizeof(ConstBufferData) + 0xff) & ~0xff);

    // 定数バッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeCB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&constBuffer));

    return result;
}

// 定数バッファへのデータ転送
HRESULT Sprite::TransferConstBuffer()
{
    // マッピング
    ConstBufferData *constMap = nullptr;
    auto result = constBuffer->Map(0, nullptr, (void **)&constMap);
    assert(SUCCEEDED(result));

    constMap->color = color; // 色を指定
    // 平行投影
    constMap->mat = matWorld * XMMatrixOrthographicOffCenterLH(0.0f,
                                                               static_cast<float>(WINDOW_WIDTH),
                                                               static_cast<float>(WINDOW_HEIGHT),
                                                               0.0f,
                                                               0.0f,
                                                               1.0f);

    // マッピング解除
    constBuffer->Unmap(0, nullptr);

    return result;
}

// 頂点座標を計算
void Sprite::CalcVertPos()
{
    float left = (0.0f - anchorPoint.x) * size.x;;
    float right = (1.0f - anchorPoint.x) * size.x;
    float top = (0.0f - anchorPoint.y) * size.y;
    float bottom = (1.0f - anchorPoint.y) * size.y;

    if ( isFlipX )
    {
        left *= -1;
        right *= -1;
    }
    if ( isFlipY )
    {
        top *= -1;
        bottom *= -1;
    }

    vertices[LB].pos = { left, bottom, 0.0f };  // 左下
    vertices[LT].pos = { left,    top, 0.0f };  // 左上
    vertices[RB].pos = { right, bottom, 0.0f }; // 右下
    vertices[RT].pos = { right,   top, 0.0f };  // 右上
}

// 頂点UV座標を計算
void Sprite::CalcClippingVertUV(XMFLOAT2 texLeftTop, XMFLOAT2 texSize, const int &texIndex)
{
    XMFLOAT2 size = texture.GetSpriteTextureSize(texIndex);
    float texLeft = texLeftTop.x / size.x;
    float texRight = (texLeftTop.x + texSize.x) / size.x;
    float texTop = texLeftTop.y / size.y;
    float texBottom = (texLeftTop.y + texSize.y) / size.y;

    vertices[LB].uv = { texLeft,  texBottom };   // 左下
    vertices[LT].uv = { texLeft,     texTop };   // 左上
    vertices[RB].uv = { texRight, texBottom };   // 右下
    vertices[RT].uv = { texRight,    texTop };   // 右上
}

// 初期化処理
HRESULT Sprite::Initialize()
{
    // 頂点生成
    CreateVertex();

    // 頂点バッファの生成
    auto result = CreateVertBuffer();
    assert(SUCCEEDED(result));

    // 頂点バッファへのデータ転送
    result = TransferVertBuffer();
    assert(SUCCEEDED(result));

    // 定数バッファ
    result = CreateConstBuffer();
    assert(SUCCEEDED(result));

    // 定数バッファへの転送
    result = TransferConstBuffer();
    assert(SUCCEEDED(result));

    return result;
}

// テクスチャを切り出す
void Sprite::ClippingTexture(XMFLOAT2 texLeftTop, XMFLOAT2 texSize, const int &texIndex)
{
    CalcClippingVertUV(texLeftTop, texSize, texIndex);

    // 頂点バッファへのデータ転送
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}

// 更新処理
void Sprite::Update()
{
    /*if ( !dirtyFlag )
    {
        return;
    }*/

    // 行列の更新
    UpdateMatrix();

    // 定数バッファへの転送
    if ( FAILED(TransferConstBuffer()) )
    {
        assert(0);
        return;
    }
}

// 描画処理
void Sprite::Draw(const int &texIndex, bool isFlipX, bool isFlipY, bool isInvisible, bool isSetTexResolution)
{
    if ( isInvisible )
    {
        return;
    }

    // ポリゴンサイズをテクスチャ画像の解像度に設定(一度だけ)
    if ( isSetTexResolution &&
        isSetTexResolutionOnlyOnce )
    {
        SetSize(texture.GetSpriteTextureSize(texIndex));
        isSetTexResolutionOnlyOnce = false;
    }

    // 反転
    if ( this->isFlipX != isFlipX || this->isFlipY != isFlipY )
    {
        this->isFlipX = isFlipX;
        this->isFlipY = isFlipY;
        CalcVertPos();

        // 定数バッファへの転送
        if ( FAILED(TransferVertBuffer()) )
        {
            assert(0);
            return;
        }
    }

    // 頂点バッファビューをセット
    dx12.GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

    // 定数バッファをセット
    dx12.GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());

    // シェーダーリソースビューをセット
    dx12.GetCmdList()->SetGraphicsRootDescriptorTable(1,
                                                      CD3DX12_GPU_DESCRIPTOR_HANDLE(render.GetSpriteDescHeap()->GetGPUDescriptorHandleForHeapStart(),
                                                                                    texIndex,
                                                                                    dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    // ポリゴンの描画(4頂点で四角形)
    dx12.GetCmdList()->DrawInstanced(4, 1, 0, 0);
}

//// サイズを設定
//void Sprite::SetSize(XMFLOAT2 size)
//{
//    // ポリゴンサイズをテクスチャ画像の解像度に設定フラグをfalse
//    isSetTexResolutionOnlyOnce = false;
//
//    float left = (0.0f - anchorPoint.x) * size.x;
//    float right = (1.0f - anchorPoint.x) * size.x;
//    float top = (0.0f - anchorPoint.y) * size.y;
//    float bottom = (1.0f - anchorPoint.y) * size.y;
//
//    vertices[LB].pos = { left, bottom, 0.0f };  // 左下
//    vertices[LT].pos = { left,    top, 0.0f };  // 左上
//    vertices[RB].pos = { right, bottom, 0.0f }; // 右下
//    vertices[RT].pos = { right,   top, 0.0f };  // 右上
//
//    // 頂点バッファへのデータ転送
//    if ( FAILED(TransferVertBuffer()) )
//    {
//        assert(0);
//        return;
//    }
//}
