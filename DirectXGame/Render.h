#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include <wrl.h>

class DirectX12Wrapper;

class Render
{
private:
    DirectX12Wrapper &dx12;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap = nullptr;
    D3D12_DESCRIPTOR_RANGE descRangeCBV{};              // デスクリプタレンジ 定数用
    D3D12_DESCRIPTOR_RANGE descRangeSRV{};              // デスクリプタレンジ テクスチャ用
    D3D12_ROOT_PARAMETER rootParams[2]{};               // ルートパラメータ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;       // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;       // パイプライン
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};            // テクスチャサンプラー

    // シェーダーまわり
    ID3DBlob *vsBlob = nullptr;    // 頂点シェーダオブジェクト
    ID3DBlob *psBlob = nullptr;    // ピクセルシェーダオブジェクト
    ID3DBlob *errorBlob = nullptr; //エラーオブジェクト

    // デスクリプタヒープの生成
    HRESULT CreateDescriptorHeap();

    // テクスチャサンプラーの設定
    void SettingTextureSampler();

    // ルートシグネチャ生成
    HRESULT CreateRootSignature();

    // パイプライン生成
    HRESULT CreateGraphicsPipeline();

public:
    Render(DirectX12Wrapper &dx12);
    ~Render();

    // 描画処理
    void BeginDraw();

    ID3D12DescriptorHeap *GetBasicDescHeap()
    {
        return basicDescHeap.Get();
    }
};