#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dinput.h>

#include <vector>
#include <wrl.h>

class DirectX12Wrapper
{
private:
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    // DXGIまわり
    ComPtr<IDXGIFactory6> dxgiFactory;
    ComPtr<IDXGISwapChain4> swapChain;

    // DirectX12まわり
    ComPtr<ID3D12Device >dev;
    ComPtr<ID3D12CommandAllocator> cmdAllocator;
    ComPtr<ID3D12GraphicsCommandList> cmdList;
    ComPtr<ID3D12CommandQueue> cmdQueue;

    // バッファまわり
    std::vector<ComPtr<ID3D12Resource>> backBuffers;    // バックバッファ
    ComPtr<ID3D12DescriptorHeap> rtvHeaps;              // レンダーターゲット用デスクリプタヒープ
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};              // デスクリプタヒープ

    ComPtr<ID3D12DescriptorHeap> dsvHeap;

    ComPtr<ID3D12Resource> depthBuffer;

    // フェンス
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceVal = 0;

    // 入力デバイス
    IDirectInputDevice8 *devkeyboard = nullptr;

    // DXGIまわり初期化
    HRESULT InitDXGIDevice();

    // コマンドまわり初期化
    HRESULT InitCommand();

    // スワップチェインの生成
    HRESULT CreateSwapChain(const HWND &hwnd);

    // 最終的なレンダーターゲットの生成
    HRESULT CreateFinalRenderTarget();

    // 深度バッファの生成
    HRESULT CreateDepthBuffer();

    // 入力デバイスの生成
    HRESULT CreateInputDevice(WNDCLASSEX w, HWND hwnd);

public:
    DirectX12Wrapper(WNDCLASSEX w, HWND hwnd);
    ~DirectX12Wrapper();

    // 更新処理
    void Update();

    // 描画処理
    void FlushGPU();    // GPUの処理が終わるのを待つ
    void BeginDraw();   // 描画準備
    void EndDraw();     // 描画後処理

    ID3D12Device *GetDevice()
    {
        return dev.Get();
    }

    ID3D12GraphicsCommandList *GetCmdList()
    {
        return cmdList.Get();
    }

    IDirectInputDevice8 *GetDevkeyboard()
    {
        return devkeyboard;
    }
};