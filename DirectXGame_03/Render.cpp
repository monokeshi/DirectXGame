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
    auto result = CreateDescriptorHeap();
    assert(SUCCEEDED(result));

    // テクスチャサンプラー
    SettingTextureSampler();

    // ルートシグネチャ
    result = CreateRootSignature();
    assert(SUCCEEDED(result));

    // グラフィックスパイプライン
    result = CreateGraphicsPipeline();
    assert(SUCCEEDED(result));
}

Render::~Render()
{
}

// デスクリプタヒープの生成
HRESULT Render::CreateDescriptorHeap()
{
    // 設定構造体
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーから見える
    descHeapDesc.NumDescriptors = CONSTANT_BUFFER_NUM + TEXTURE_BUFFER_NUM;

    // デスクリプタヒープの生成
    auto result = dx12.GetDevice()->CreateDescriptorHeap(&descHeapDesc,
                                                         IID_PPV_ARGS(&basicDescHeap));
    assert(SUCCEEDED(result));

    // デスクリプタヒープテーブル(定数用)の設定
    descRangeCBV.NumDescriptors = 1;                            // 定数一つ
    descRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;   // 種別は定数
    descRangeCBV.BaseShaderRegister = 0;                        // 0番スロットから
    descRangeCBV.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;                   // 標準

    // デスクリプタヒープテーブル(テクスチャ用)の設定
    descRangeSRV.NumDescriptors = 1;                            // テクスチャ一つ
    descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;   // 種別はテクスチャ
    descRangeSRV.BaseShaderRegister = 0;                        // 0番スロットから
    descRangeSRV.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;                   // 標準

    // ルートパラメータ(定数用)の設定
    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;   // 種別
    rootParams[0].DescriptorTable.pDescriptorRanges = &descRangeCBV;            // デスクリプタレンジ
    rootParams[0].DescriptorTable.NumDescriptorRanges = 1;                      // デスクリプタレンジ数
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;               // 全てのシェーダーから見える

    // ルートパラメータ(テクスチャ用)の設定
    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;   // 種別
    rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;            // デスクリプタレンジ
    rootParams[1].DescriptorTable.NumDescriptorRanges = 1;                      // デスクリプタレンジ数
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;               // 全てのシェーダーから見える

    return result;
}

// テクスチャサンプラーの設定
void Render::SettingTextureSampler()
{
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 // 横繰り返し(タイリング)
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 // 縦繰り返し(タイリング)
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 // 奥行繰り返し(タイリング)
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;  // ボーダーの時は黒
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;                    // 補間しない(ニアレストネイバー)
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                 // ミニマップ最大値
    samplerDesc.MinLOD = 0.0f;                                              // ミニマップ最小値
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           // ピクセルシェーダーからのみ可視
}

// ルートシグネチャ生成
HRESULT Render::CreateRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParams;                 // ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = _countof(rootParams);     // ルートパラメータ数
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob> rootSigBlob = nullptr;
    auto result = D3D12SerializeRootSignature(&rootSignatureDesc,
                                              D3D_ROOT_SIGNATURE_VERSION_1_0,
                                              &rootSigBlob,
                                              &errorBlob);

    // ルートシグネチャの生成
    assert(SUCCEEDED(result));
    result = dx12.GetDevice()->CreateRootSignature(0,
                                                   rootSigBlob->GetBufferPointer(),
                                                   rootSigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(result));

    return result;
}

// パイプライン生成
HRESULT Render::CreateGraphicsPipeline()
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
    gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
    gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
    gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
    gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

    // サンプルマスクとラスタライズステート設定
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;           // 標準設定
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;  // 背面をカリング
    gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    gpipeline.RasterizerState.DepthClipEnable = true;           // 深度クリッピングを有効に

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
    gpipeline.pRootSignature = rootSignature.Get();

    // デプスステンシルステートの設定
    gpipeline.DepthStencilState.DepthEnable = true;                             // 深度テストを行う
    gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;    // 書き込み可能
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;         // 小さければ合格
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                // 深度値フォーマット

    // パイプラインステートの生成
    result = dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
    assert(SUCCEEDED(result));

    return result;
}

// 描画処理
void Render::BeginDraw()
{
    // パイプラインステートとルートシグネチャの設定
    assert(pipelineState != nullptr);
    dx12.GetCmdList()->SetPipelineState(pipelineState.Get());
    dx12.GetCmdList()->SetGraphicsRootSignature(rootSignature.Get());
}