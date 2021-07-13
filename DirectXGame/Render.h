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

    struct PipelineSet
    {
        ComPtr<ID3D12RootSignature> rootSignature;  // ルートシグネチャ
        ComPtr<ID3D12PipelineState> pipelineState;  // パイプライン
    };

    PipelineSet obj3DPipeline;
    PipelineSet spritePipeline;

    DirectX12Wrapper &dx12;

    ComPtr<ID3D12DescriptorHeap> basicDescHeap;
    CD3DX12_DESCRIPTOR_RANGE descRangeCBV{};                    // デスクリプタレンジ 定数用
    CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};                    // デスクリプタレンジ テクスチャ用
    CD3DX12_ROOT_PARAMETER rootParams[2]{};                     // ルートパラメータ
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};                    // テクスチャサンプラー

    // シェーダーまわり
    ComPtr<ID3DBlob> vsBlob = nullptr;    // 頂点シェーダオブジェクト
    ComPtr<ID3DBlob> psBlob = nullptr;    // ピクセルシェーダオブジェクト
    ComPtr<ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> spriteDescHeap;    // スプライト用デスクリプタヒープ

    // デスクリプタヒープの生成
    HRESULT CreateDescriptorHeapObject3D(); // オブジェクト3D用
    HRESULT CreateDescriptorHeapSprite();   // スプライト用

    // ルートパラメータの設定
    void SettingRootParameter();

    // テクスチャサンプラーの設定
    void SettingTextureSampler();

    // ルートシグネチャ生成
    HRESULT CreateRootSignatureObject3D();  // オブジェクト3D用
    HRESULT CreateRootSignatureSprite();    // スプライト用

    // パイプライン生成
    HRESULT CreateGraphicsPipelineObject3D();   // オブジェクト3D用
    HRESULT CreateGraphicsPipelineSprite();     // スプライト用

public:
    Render(DirectX12Wrapper &dx12);
    ~Render();

    // オブジェクト3Dのパイプラインセット
    void BeginDrawObj3D();

    // スプライトのパイプラインセット
    void BeginDrawSprite();

    ID3D12DescriptorHeap *GetBasicDescHeap()
    {
        return basicDescHeap.Get();
    }
    
    ID3D12DescriptorHeap *GetSpriteDescHeap()
    {
        return spriteDescHeap.Get();
    }
};