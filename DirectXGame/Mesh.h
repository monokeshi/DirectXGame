#pragma once

#include <d3dx12.h>
#include <DirectXMath.h>

#include <vector>
#include <wrl.h>

class DirectX12Wrapper;

enum class  MeshList
{
    e_RECTANGLE,            // 四角形
    e_CUBE,                 // 立方体
    e_TRIANGULAR_PYRAMID,   // 三角錐
};

class Mesh
{
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;       // xyz座標
        DirectX::XMFLOAT3 normal;    // 法線ベクトル
        DirectX::XMFLOAT2 uv;        // uv座標
    };

    DirectX12Wrapper &dx12;

    MeshList shapeList;

    std::vector<Vertex> vertices;                       // 頂点データ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffer;  // 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vbView{};                  // 頂点バッファビュー
    std::vector<unsigned short> indices;                // インデックスデータ
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer; // インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW ibView{};                   // インデックスバッファビュー

    // 四角形(平面)を生成
    void CreateRectangle();

    // 立方体を生成
    void CreateCube();

    // 三角錐を生成
    void CreateTriangularPyramid();

    // 図形を生成
    void CreateMesh(MeshList shapeList);

    // 頂点バッファの生成
    HRESULT CreateVertBuffer();

    // インデックスバッファの生成
    HRESULT CreateIndexBuffer();

    // 法線を計算する
    void CalcNormal();

    // 頂点バッファへの転送
    HRESULT TransferVertBuffer();

public:
    Mesh(MeshList shapeList, DirectX12Wrapper &dx12);
    ~Mesh();

    void Update();

    const D3D12_VERTEX_BUFFER_VIEW GetVbView() const
    {
        return vbView;
    }

    const D3D12_INDEX_BUFFER_VIEW GetIbView() const
    {
        return ibView;
    }

    const int GetIndicesNum() const
    {
        return static_cast<int>(indices.size());
    }
};