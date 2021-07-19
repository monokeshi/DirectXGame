#include "Object3D.h"
#include "DirectX12Wrapper.h"
#include "Render.h"
#include "Define.h"

#include <d3dcompiler.h>

#include <string>
#include <assert.h>

using namespace DirectX;

Object3D::Object3D(XMFLOAT3 position,
                   XMFLOAT3 scale,
                   XMFLOAT3 rotation,
                   DirectX::XMFLOAT4 color,
                   XMMATRIX &matView,
                   XMMATRIX *matWorldParent,
                   DirectX12Wrapper &dx12,
                   Render &render,
                   int index,
                   bool isPerspective):
    position(position), scale(scale), rotation(rotation), color(color), dx12(dx12), render(render), instanceNum(index)
{
    matrix = new Matrix(this->position, this->scale, this->rotation, matView, matWorldParent, isPerspective);
    ++instanceNum;
}

Object3D::~Object3D()
{
    delete matrix;
}

// 定数バッファの生成
HRESULT Object3D::CreateConstBuffer()
{
    UINT sizeCB = static_cast<UINT>((sizeof(ConstBufferData) + 0xff) & ~0xff);

    // 定数バッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeCB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&constBuffer));

    // デスクリプタヒープ1つ分のサイズ
    UINT descHandleIncrementSize = dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // CBVのCPUデスクリプタハンドルを計算
    cpuDescHandleCBV = render.GetBasicDescHeap()->GetCPUDescriptorHandleForHeapStart();
    cpuDescHandleCBV.ptr += (instanceNum - 1) * descHandleIncrementSize;

    // CBVのGPUデスクリプタハンドルを計算
    gpuDescHandleCBV = render.GetBasicDescHeap()->GetGPUDescriptorHandleForHeapStart();
    gpuDescHandleCBV.ptr += (instanceNum - 1) * descHandleIncrementSize;

    // 定数バッファビューの生成
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = constBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = static_cast<UINT>(constBuffer->GetDesc().Width);
    dx12.GetDevice()->CreateConstantBufferView(&cbvDesc,
                                               cpuDescHandleCBV);    // デスクリプタヒープの先頭に定数バッファビューの生成

    return result;
}

// 定数バッファへのデータ転送
HRESULT Object3D::TransferConstBuffer()
{
    // マッピング
    ConstBufferData *constMap = nullptr;
    auto result = constBuffer->Map(0, nullptr, (void **)&constMap);
    assert(SUCCEEDED(result));

    constMap->color = color;             // 色を指定
    constMap->mat = matrix->GetMat();

    // マッピング解除
    constBuffer->Unmap(0, nullptr);

    return result;
}

// 初期化
HRESULT Object3D::Initialize()
{
    // 定数バッファ
    auto result = CreateConstBuffer();
    assert(SUCCEEDED(result));

    // 定数バッファの転送
    result = TransferConstBuffer();
    assert(SUCCEEDED(result));

    return result;
}

// 毎フレーム更新処理
void Object3D::Update()
{
    // 行列の更新処理
    matrix->Update();

    // 定数バッファへの転送
    if ( FAILED(TransferConstBuffer()) )
    {
        assert(0);
        return;
    }
}

// 毎フレーム描画処理
void Object3D::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &gpuDescHandleSRV,
                    const D3D12_VERTEX_BUFFER_VIEW &vbView,
                    const D3D12_INDEX_BUFFER_VIEW &ibView,
                    int indicesNum)
{
    // プリミティブ形状の設定
    dx12.GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 頂点バッファビューをセット
    dx12.GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

    // インデックスバッファビューをセット
    dx12.GetCmdList()->IASetIndexBuffer(&ibView);

    // デスクリプタヒープをセット
    ID3D12DescriptorHeap *ppHeaps[] = { render.GetBasicDescHeap() };
    dx12.GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // 定数バッファビューをセット
    dx12.GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());

    // シェーダーリソースビューをセット
    dx12.GetCmdList()->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

    // 描画
    dx12.GetCmdList()->DrawIndexedInstanced(static_cast<UINT>(indicesNum), 1, 0, 0, 0);
}

// X軸移動
void Object3D::MoveX(float move)
{
    position.x += move;
}

// Y軸移動
void Object3D::MoveY(float move)
{
    position.y += move;
}

// Z軸移動
void Object3D::MoveZ(float move)
{
    position.z += move;
}

// X軸回転
void Object3D::RotateX(float rotation)
{
    this->rotation.x += rotation;
}

// Y軸回転
void Object3D::RotateY(float rotation)
{
    this->rotation.y += rotation;
}

// Z軸回転
void Object3D::RotateZ(float rotation)
{
    this->rotation.z += rotation;
}