#include "Render.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <string>
#include <assert.h>

using namespace Microsoft::WRL;

Render::Render(DirectX12Wrapper &dx12):
    dx12(dx12)
{
    // デスクリプタヒープ
    auto result = CreateDescriptorHeapObject3D();
    assert(SUCCEEDED(result));
    result = CreateDescriptorHeapSprite();
    assert(SUCCEEDED(result));

    // ルートパラメータの設定
    SettingRootParameter();

    // テクスチャサンプラー
    SettingTextureSampler();

    // ルートシグネチャ
    result = CreateRootSignatureObject3D();
    assert(SUCCEEDED(result));
    result = CreateRootSignatureSprite();
    assert(SUCCEEDED(result));

    // グラフィックスパイプライン
    result = CreateGraphicsPipelineObject3D();
    assert(SUCCEEDED(result));
    result = CreateGraphicsPipelineSprite();
    assert(SUCCEEDED(result));
}

Render::~Render()
{
}

// デスクリプタヒープの生成
HRESULT Render::CreateDescriptorHeapObject3D()
{
    // 設定構造体
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーから見える
    descHeapDesc.NumDescriptors = CONSTANT_BUFFER_NUM + TEXTURE_BUFFER_NUM;

    // デスクリプタヒープの生成
    auto result = dx12.GetDevice()->CreateDescriptorHeap(&descHeapDesc,
                                                         IID_PPV_ARGS(&basicDescHeap));

    return result;
}

// スプライト用デスクリプタヒープの生成
HRESULT Render::CreateDescriptorHeapSprite()
{
    // 設定構造体
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーから見える
    descHeapDesc.NumDescriptors = SPRITE_TEXTURE_NUM;

    // デスクリプタヒープの生成
    auto result = dx12.GetDevice()->CreateDescriptorHeap(&descHeapDesc,
                                                         IID_PPV_ARGS(&spriteDescHeap));

    return result;
}

// ルートパラメータの設定
void Render::SettingRootParameter()
{
    // デスクリプタヒープテーブル(定数用)の設定
    descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);   // b0レジスタ

    // デスクリプタヒープテーブル(テクスチャ用)の設定
    descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);   // t0レジスタ

    // ルートパラメータ(定数用)の設定
    rootParams[0].InitAsConstantBufferView(0);  // 定数バッファビューとして初期化(b0レジスタ)

    // ルートパラメータ(テクスチャ用)の設定
    rootParams[1].InitAsDescriptorTable(1, &descRangeSRV);
}

// テクスチャサンプラーの設定
void Render::SettingTextureSampler()
{
    samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
}

// ルートシグネチャ生成
HRESULT Render::CreateRootSignatureObject3D()
{
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> rootSigBlob;
    auto result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
                                                        D3D_ROOT_SIGNATURE_VERSION_1_0,
                                                        &rootSigBlob,
                                                        &errorBlob);
    assert(SUCCEEDED(result));

    // ルートシグネチャの生成
    result = dx12.GetDevice()->CreateRootSignature(0,
                                                   rootSigBlob->GetBufferPointer(),
                                                   rootSigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&obj3DPipeline.rootSignature));
    assert(SUCCEEDED(result));

    return result;
}

// スプライト用ルートシグネチャ生成
HRESULT Render::CreateRootSignatureSprite()
{
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> rootSigBlob;
    auto result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
                                                        D3D_ROOT_SIGNATURE_VERSION_1_0,
                                                        &rootSigBlob,
                                                        &errorBlob);
    assert(SUCCEEDED(result));

    // ルートシグネチャの生成
    result = dx12.GetDevice()->CreateRootSignature(0,
                                                   rootSigBlob->GetBufferPointer(),
                                                   rootSigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&spritePipeline.rootSignature));
    assert(SUCCEEDED(result));

    return result;
}

// パイプライン生成
// オブジェクト3D用
HRESULT Render::CreateGraphicsPipelineObject3D()
{
    // 頂点シェーダファイルの読み込みとコンパイル
    auto result = D3DCompileFromFile(L"BasicVS.hlsl",                                 // シェーダファイル名
                                     nullptr,
                                     D3D_COMPILE_STANDARD_FILE_INCLUDE,               // インクルード可能にする
                                     "main",                                          // エントリーポイント名
                                     "vs_5_0",                                        // シェーダーモデル指定
                                     D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
                                     0,
                                     &vsBlob,
                                     &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // ピクセルシェーダファイルの読み込みとコンパイル
    result = D3DCompileFromFile(L"BasicPS.hlsl",                                 // シェーダファイル名
                                nullptr,
                                D3D_COMPILE_STANDARD_FILE_INCLUDE,               // インクルード可能にする
                                "main",                                          // エントリーポイント名
                                "ps_5_0",                                        // シェーダーモデル指定
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
                                0,
                                &psBlob,
                                &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {   // xyz座標
            "POSITION",                                 // セマンティック名
            0,                                          // 同じセマンティック名が複数ある時に使うインデックス
            DXGI_FORMAT_R32G32B32_FLOAT,                // 要素数とビット数を表す
            0,                                          // 入力スロットインデックス
            D3D12_APPEND_ALIGNED_ELEMENT,               // データのオフセット値
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // 入力データ種別
            0                                           // いい度に描画するインスタンス数
        },
        {
            // 法線ベクトル
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {   // uv座標
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    // グラフィックスパイプライン設定
    // 頂点シェーダ、ピクセルシェーダをパイプラインに設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    // サンプルマスクとラスタライズステート設定
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;           // 標準設定
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // レンダーターゲットのブレンド設定
    D3D12_RENDER_TARGET_BLEND_DESC &blendDesc = gpipeline.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定
    blendDesc.BlendEnable = true;                                   // ブレンドを有効
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;                    // 加算
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;                      // ソースの値を100%使う
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;                    // デストの値を0%使う
    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;                         // 加算
    blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;                     // ソースのアルファ値
    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;                // 1.0f - ソースのアルファ値

    // 頂点レイアウトの設定
    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);

    // 図形の形状を設定
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // その他の設定
    gpipeline.NumRenderTargets = 1;                       //描画対象は1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255指定のRGBA
    gpipeline.SampleDesc.Count = 1;                       // 1ピクセルにつき1回サンプリング

    // パイプラインにルートシグネチャをセット
    gpipeline.pRootSignature = obj3DPipeline.rootSignature.Get();

    // デプスステンシルステートの設定
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                // 深度値フォーマット

    // パイプラインステートの生成
    result = dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&obj3DPipeline.pipelineState));
    assert(SUCCEEDED(result));

    return result;
}

// スプライト用
HRESULT Render::CreateGraphicsPipelineSprite()
{
    // 頂点シェーダファイルの読み込みとコンパイル
    auto result = D3DCompileFromFile(L"SpriteVS.hlsl",                                  // シェーダファイル名
                                     nullptr,
                                     D3D_COMPILE_STANDARD_FILE_INCLUDE,                 // インクルード可能にする
                                     "main",                                            // エントリーポイント名
                                     "vs_5_0",                                          // シェーダーモデル指定
                                     D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,   // デバッグ用設定
                                     0,
                                     &vsBlob,
                                     &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // ピクセルシェーダファイルの読み込みとコンパイル
    result = D3DCompileFromFile(L"SpritePS.hlsl",                                   // シェーダファイル名
                                nullptr,
                                D3D_COMPILE_STANDARD_FILE_INCLUDE,                  // インクルード可能にする
                                "main",                                             // エントリーポイント名
                                "ps_5_0",                                           // シェーダーモデル指定
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,    // デバッグ用設定
                                0,
                                &psBlob,
                                &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {   // xyz座標
            "POSITION",                                 // セマンティック名
            0,                                          // 同じセマンティック名が複数ある時に使うインデックス
            DXGI_FORMAT_R32G32B32_FLOAT,                // 要素数とビット数を表す
            0,                                          // 入力スロットインデックス
            D3D12_APPEND_ALIGNED_ELEMENT,               // データのオフセット値
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // 入力データ種別
            0                                           // いい度に描画するインスタンス数
        },
        {   // uv座標
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    // グラフィックスパイプライン設定
    // 頂点シェーダ、ピクセルシェーダをパイプラインに設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    // サンプルマスクとラスタライズステート設定
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;                   // 標準設定
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    // レンダーターゲットのブレンド設定
    D3D12_RENDER_TARGET_BLEND_DESC &blendDesc = gpipeline.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定
    blendDesc.BlendEnable = true;                                   // ブレンドを有効
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;                    // 加算
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;                      // ソースの値を100%使う
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;                    // デストの値を0%使う
    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;                         // 加算
    blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;                     // ソースのアルファ値
    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;                // 1.0f - ソースのアルファ値

    // 頂点レイアウトの設定
    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);

    // 図形の形状を設定
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // その他の設定
    gpipeline.NumRenderTargets = 1;                       //描画対象は1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255指定のRGBA
    gpipeline.SampleDesc.Count = 1;                       // 1ピクセルにつき1回サンプリング

    // パイプラインにルートシグネチャをセット
    gpipeline.pRootSignature = spritePipeline.rootSignature.Get();

    // デプスステンシルステートの設定
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;   // 常に上書き
    gpipeline.DepthStencilState.DepthEnable = false;                        // 深度テストをしない
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                            // 深度値フォーマット

    // パイプラインステートの生成
    result = dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&spritePipeline.pipelineState));
    assert(SUCCEEDED(result));

    return result;
}

// オブジェクト3Dのパイプラインセット
void Render::BeginDrawObj3D()
{
    dx12.GetCmdList()->SetPipelineState(obj3DPipeline.pipelineState.Get());
    dx12.GetCmdList()->SetGraphicsRootSignature(obj3DPipeline.rootSignature.Get());
}

// スプライトのパイプラインセット
void Render::BeginDrawSprite()
{
    // パイプラインステートの設定
    dx12.GetCmdList()->SetPipelineState(spritePipeline.pipelineState.Get());

    // ルートシグネスチャの設定
    dx12.GetCmdList()->SetGraphicsRootSignature(spritePipeline.rootSignature.Get());

    // プリミティブ形状の設定
    dx12.GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // テクスチャ用デスクリプタヒープの設定
    ID3D12DescriptorHeap *ppHeaps[] = { spriteDescHeap.Get() };
    dx12.GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}