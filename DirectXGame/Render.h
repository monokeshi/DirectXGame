#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include <wrl.h>

class DirectX12Wrapper;

class Render
{
private:
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    DirectX12Wrapper &dx12;

    ComPtr<ID3D12DescriptorHeap> basicDescHeap;
    CD3DX12_DESCRIPTOR_RANGE descRangeCBV{};                    // デスクリプタレンジ 定数用
    CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};                    // デスクリプタレンジ テクスチャ用
    CD3DX12_ROOT_PARAMETER rootParams[2]{};                     // ルートパラメータ
    ComPtr<ID3D12RootSignature> rootSignature;                  // ルートシグネチャ
    ComPtr<ID3D12PipelineState> pipelineState;                  // パイプライン
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};                    // テクスチャサンプラー

    // シェーダーまわり
    ComPtr<ID3DBlob> vsBlob = nullptr;    // 頂点シェーダオブジェクト
    ComPtr<ID3DBlob> psBlob = nullptr;    // ピクセルシェーダオブジェクト
    ComPtr<ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

    // デスクリプタヒープの生成
    HRESULT CreateDescriptorHeap();

    // ルートパラメータの設定
    void SettingRootParameter();

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