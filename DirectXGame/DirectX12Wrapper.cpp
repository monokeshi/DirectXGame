#include "DirectX12Wrapper.h"
#include "Application.h"
#include "Define.h"

#include <d3d12.h>
#include <d3dx12.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#include <assert.h>
#include <vector>
#include <string>

using namespace Microsoft::WRL;

// デバッグレイヤーを有効にする
void EnableDebugLayer()
{
    ComPtr<ID3D12Debug> debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
}

DirectX12Wrapper::DirectX12Wrapper(WNDCLASSEX w, HWND hwnd)
{
#ifdef _DEBUG
    //デバッグレイヤーをオン
    EnableDebugLayer();
#endif
    if ( FAILED(InitDXGIDevice()) )
    {
        assert(0);
        return;
    }

    // コマンドまわり初期化
    if ( FAILED(InitCommand()) )
    {
        assert(0);
        return;
    }

    // スワップチェーンの生成
    if ( FAILED(CreateSwapChain(hwnd)) )
    {
        assert(0);
        return;
    }

    // 最終的なレンダーターゲット
    if ( FAILED(CreateFinalRenderTarget()) )
    {
        assert(0);
        return;
    }

    // 深度バッファの生成
    if ( FAILED(CreateDepthBuffer()) )
    {
        assert(0);
        return;
    }

    // フェンスの生成
    if ( FAILED(dev->CreateFence(fenceVal,
                                 D3D12_FENCE_FLAG_NONE,
                                 IID_PPV_ARGS(&fence))) )
    {
        assert(0);
        return;
    }

    // 入力デバイスの生成
    if ( FAILED(CreateInputDevice(w, hwnd)) )
    {
        assert(0);
        return;
    }
}

DirectX12Wrapper::~DirectX12Wrapper()
{
}

// DXGIまわり初期化
HRESULT DirectX12Wrapper::InitDXGIDevice()
{
    // DXGIファクトリーの生成
    auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(result));

    // アダプターの列挙用
    std::vector<ComPtr<IDXGIAdapter1>> adapters;

    // ここに特定の名前を持つアダプターオブジェクトが入る
    ComPtr<IDXGIAdapter1> tmpAdapter = nullptr;
    for ( int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) !=
         DXGI_ERROR_NOT_FOUND; i++ )
    {
        adapters.push_back(tmpAdapter.Get()); // 動的配列に追加する
    }

    for ( unsigned int i = 0; i < adapters.size(); i++ )
    {
        DXGI_ADAPTER_DESC1 adesc;
        adapters[i]->GetDesc1(&adesc); // アダプターの情報を取得

        // ソフトウェアデバイスを回避
        if ( adesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE )
        {
            continue;
        }

        std::wstring strDesc = adesc.Description; // アダプター名
        // Intel UHD Graphics(オンボードグラフィック)を回避
        if ( strDesc.find(L"Intel") == std::wstring::npos )
        {
            tmpAdapter = adapters[i]; // 採用
            break;
        }
    }

    // 対応レベルの配列
    D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;
    for ( int i = 0; i < _countof(levels); levels )
    {
        // 採用したアダプターでデバイスを生成
        result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
        if ( SUCCEEDED(result) )
        {
            // デバイスを生成できた時点でループを抜ける
            featureLevel = levels[i];
            break;
        }
    }

    return result;
}

// コマンドまわり初期化
HRESULT DirectX12Wrapper::InitCommand()
{
    // コマンドアロケーターを生成
    auto result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              IID_PPV_ARGS(&cmdAllocator));
    assert(SUCCEEDED(result));

    // コマンドリストを生成
    result = dev->CreateCommandList(0,
                                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                                    cmdAllocator.Get(),
                                    nullptr,
                                    IID_PPV_ARGS(&cmdList));
    assert(SUCCEEDED(result));

    // 標準設定でコマンドキューを生成
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
    result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
    assert(SUCCEEDED(result));

    return result;
}

// スワップチェインの生成
HRESULT DirectX12Wrapper::CreateSwapChain(const HWND &hwnd)
{
    // 各種設定
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
    swapchainDesc.Width = WINDOW_WIDTH;
    swapchainDesc.Height = WINDOW_HEIGHT;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;        // 色情報の書式
    swapchainDesc.SampleDesc.Count = 1;                       // マルチサンプルしない
    swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;       // バックバッファ用
    swapchainDesc.BufferCount = 2;                            // バッファ数を2つに設定
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    assert(cmdQueue != nullptr);

    //ComPtr<IDXGISwapChain1> swapChain1;

    // スワップチェーン生成
    auto result = dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(),
                                                      hwnd,
                                                      &swapchainDesc,
                                                      nullptr,
                                                      nullptr,
                                                      //&swapChain1);
                                                      (IDXGISwapChain1 **)swapChain.ReleaseAndGetAddressOf());

    // 生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換
    //swapChain1.As(&swapChain);

    return result;
}

// 最終的なレンダーターゲットの生成
HRESULT DirectX12Wrapper::CreateFinalRenderTarget()
{
    // 各種設定をしてデスクリプタヒープを生成
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
    heapDesc.NumDescriptors = 2;                    // 裏表の2つ
    auto result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    if ( FAILED(result) )
    {
        assert(0);
        return result;
    }

    DXGI_SWAP_CHAIN_DESC swcDesc{};
    result = swapChain->GetDesc(&swcDesc);
    backBuffers.resize(swcDesc.BufferCount);

    for ( int i = 0; i < 2; i++ )
    {
        // スワップチェーンからバッファを取得
        result = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
        assert(SUCCEEDED(result));
        dev->CreateRenderTargetView(backBuffers[i].Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
                                                                                                 i,
                                                                                                 dev->GetDescriptorHandleIncrementSize(heapDesc.Type)));
    }

    return result;
}

// 深度バッファの生成
HRESULT DirectX12Wrapper::CreateDepthBuffer()
{
    // 深度バッファリソース設定
    CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                                                                      WINDOW_WIDTH,
                                                                      WINDOW_HEIGHT,
                                                                      1, 0,
                                                                      1, 0,
                                                                      D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    // 深度バッファの生成
    auto result = dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                               D3D12_HEAP_FLAG_NONE,
                                               &depthResDesc,
                                               D3D12_RESOURCE_STATE_DEPTH_WRITE,    // 深度値書き込みに使用
                                               &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
                                               IID_PPV_ARGS(&depthBuffer));
    assert(SUCCEEDED(result));

    // 深度ビュー用デスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;                     // 深度ビューは1つ
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;  // デプスステンシルビュー
    result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
    assert(SUCCEEDED(result));

    // 深度ビュー作成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // 深度値フォーマット
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dev->CreateDepthStencilView(depthBuffer.Get(),
                                &dsvDesc,
                                dsvHeap->GetCPUDescriptorHandleForHeapStart());

    return result;
}

// 入力デバイスの生成
HRESULT DirectX12Wrapper::CreateInputDevice(WNDCLASSEX w, HWND hwnd)
{
    // 入力の生成
    IDirectInput8 *dinput = nullptr;
    auto result = DirectInput8Create(w.hInstance,
                                     DIRECTINPUT_VERSION,
                                     IID_IDirectInput8,
                                     (void **)&dinput,
                                     nullptr);
    assert(SUCCEEDED(result));

    // キーボードデバイスの生成
    result = dinput->CreateDevice(GUID_SysKeyboard,
                                  &devkeyboard,
                                  NULL);
    assert(SUCCEEDED(result));

    // 入力データ形式のセット
    result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
    assert(SUCCEEDED(result));

    // 排他制御レベルのセット
    result = devkeyboard->SetCooperativeLevel(hwnd,
                                              DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(result));

    return result;
}

// 更新処理
void DirectX12Wrapper::Update()
{
}

// GPUの処理が終わるのを待つ
void DirectX12Wrapper::FlushGPU()
{
    cmdQueue->Signal(fence.Get(), ++fenceVal);
    if ( fence->GetCompletedValue() != fenceVal )
    {
        HANDLE event = CreateEvent(nullptr, false, false, nullptr);
        fence->SetEventOnCompletion(fenceVal, event);
        assert(event != nullptr);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

// 描画準備
void DirectX12Wrapper::BeginDraw()
{
    // バックバッファの番号を取得(2つなので0番か1番)
    UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

    // リソースバリアで書き込み可能に変更
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
                                                                      D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // 描画先指定
    // レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
    // 深度ステンシルビュー用のデスクリプタヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

    // 画面クリア
    /*                       R      G     B     A                       */
    float clearColor[] = { 0.9f, 0.5f, 0.5f, 0.0f };
    cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
    cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // 描画コマンドここから
    // ビューポートの設定
    cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<FLOAT>(WINDOW_WIDTH), static_cast<FLOAT>(WINDOW_HEIGHT)));

    // シザー矩形の設定
    cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
}

// 描画後処理
void DirectX12Wrapper::EndDraw()
{
    // バックバッファの番号を取得(2つなので0番か1番)
    UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

    // リソースバリアを戻す
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
                                                                      D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                      D3D12_RESOURCE_STATE_PRESENT));

    // 命令のクローズ
    cmdList->Close();

    // コマンドリストの実行
    ID3D12CommandList *cmdLists[] = { cmdList.Get() };  // コマンドリストの配列
    cmdQueue->ExecuteCommandLists(1, cmdLists);         // コマンドリストの実行完了を待つ
    FlushGPU();

    cmdAllocator->Reset();                          // キューをクリア
    cmdList->Reset(cmdAllocator.Get(), nullptr);    // 再びコマンドリストをためる準備

    // バッファをフリップ(裏表の入れ替え)
    swapChain->Present(1, 0);
}