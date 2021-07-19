#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <vector>

class DirectX12Wrapper;
class Render;
class Texture;

class Sprite
{
private:
    struct  Vertex
    {
        DirectX::XMFLOAT3 pos;  // xyz座標
        DirectX::XMFLOAT2 uv;   // uv座標
    };

    struct ConstBufferData
    {
        DirectX::XMFLOAT4 color; // 色(RGBA)
        DirectX::XMMATRIX mat;   // 3D変換行列
    };

    enum
    {
        LB, // 左下
        LT, // 左上
        RB, // 右下
        RT  // 右上
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Texture &texture;

    DirectX::XMFLOAT3 position;     // 座標
    float rotation;                 // 回転
    DirectX::XMMATRIX matWorld;     // ワールド行列
    DirectX::XMFLOAT4 color;        // 色
    DirectX::XMFLOAT2 size;         // サイズ
    DirectX::XMFLOAT2 anchorPoint;  // アンカーポイント
    //DirectX::XMFLOAT2 texLeftTop;   // テクスチャ左上座標
    //DirectX::XMFLOAT2 texSize;      // テクスチャ切り出しサイズ
    bool isFlipX;                   // 左右反転
    bool isFlipY;                   // 上下反転

    DirectX::XMMATRIX *matWorldParent;   // 親のワールド行列

    std::vector<Vertex> vertices;                       // 頂点データ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffer;  // 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vbView{};                  // 頂点バッファビュー
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // 定数バッファ

    bool isSetTexResolutionOnlyOnce;    // Draw関数呼び出し時にSetSize関数を一度だけ呼び出すためのフラグ
    //bool dirtyFlag;                     // 定数バッファ用の項目で変更があった場合のみtrue

    // 行列の更新処理
    void UpdateMatrix();

    // 頂点生成
    void CreateVertex();

    // 頂点バッファの生成
    HRESULT CreateVertBuffer();

    // 頂点バッファへの転送
    HRESULT TransferVertBuffer();

    // 定数バッファの生成
    HRESULT CreateConstBuffer();

    // 定数バッファへのデータ転送
    HRESULT TransferConstBuffer();

    // 頂点座標を計算
    void CalcVertPos();

    // 切り抜き後の頂点UV座標を計算
    void CalcClippingVertUV(DirectX::XMFLOAT2 texLeftTop, DirectX::XMFLOAT2 texSize, const int &texIndex);

public:
    Sprite(DirectX::XMFLOAT3 position,
           float rotation,
           DirectX::XMFLOAT4 color,
           DirectX::XMFLOAT2 size,
           DirectX::XMMATRIX *matWorldParent,
           DirectX12Wrapper &dx12,
           Render &render,
           Texture &texture);
    ~Sprite();

    // 初期化処理
    HRESULT Initialize();

    // テクスチャを切り出す
    void ClippingTexture(DirectX::XMFLOAT2 texLeftTop, DirectX::XMFLOAT2 texSize, const int &texIndex);

    // 更新処理
    void Update();

    // 描画処理
    void Draw(const int &texIndex, bool isFlipX = false, bool isFlipY = false, bool isInvisible = false, bool isSetTexResolution = true);

    // 座標を設定
    void SetPosition(DirectX::XMFLOAT3 position)
    {
        this->position = position;
    }

    // サイズを設定
    void SetSize(DirectX::XMFLOAT2 size)
    {
        this->size = size;
        CalcVertPos();

        // 頂点バッファへのデータ転送
        if ( FAILED(TransferVertBuffer()) )
        {
            assert(0);
            return;
        }
    }

    DirectX::XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};