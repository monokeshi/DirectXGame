#include "Mesh.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

using namespace DirectX;

Mesh::Mesh(MeshList shapeList, DirectX12Wrapper &dx12):
    dx12(dx12)
{
    // 図形に応じた頂点、インデックスの生成
    CreateMesh(shapeList);

    // 頂点バッファの生成
    if ( FAILED(CreateVertBuffer()) )
    {
        assert(0);
        return;
    }

    // インデックスバッファの生成
    if ( FAILED(CreateIndexBuffer()) )
    {
        assert(0);
        return;
    }

    // 法線の計算
    CalcNormal();

    // 頂点バッファへのデータ転送
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}

Mesh::~Mesh()
{
}


// 四角形(平面)を生成
void Mesh::CreateRectangle()
{
    // 頂点データ配列
    vertices =
    {
        /*     X      Y    Z     法線       U     V     */
        {{ -5.0f, -5.0f, 0.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{ -5.0f,  5.0f, 0.0f }, { }, { 0.0f, 0.0f }},  // 左上
        {{  5.0f, -5.0f, 0.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  5.0f,  5.0f, 0.0f }, { }, { 1.0f, 0.0f }},  // 右上
    };

    // インデックスデータ配列
    indices =
    {
         0,  1,  2, // 三角形1つ目
         2,  1,  3, // 三角形2つ目
    };
}

// 立方体を生成
void Mesh::CreateCube()
{
    // 頂点データ配列
    vertices =
    {
        /*     X      Y     Z     法線       U     V     */
        // 前
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{ -5.0f,  5.0f, -5.0f }, { }, { 0.0f, 0.0f }},  // 左上
        {{  5.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 0.0f }},  // 右上

        // 後(前面とZ座標の符号が逆)
        {{  5.0f,  5.0f, 5.0f }, { }, { 1.0f, 0.0f }},   // 右上
        {{  5.0f, -5.0f, 5.0f }, { }, { 1.0f, 1.0f }},   // 右下
        {{ -5.0f, -5.0f, 5.0f }, { }, { 0.0f, 1.0f }},   // 左下
        {{ -5.0f,  5.0f, 5.0f }, { }, { 0.0f, 0.0f }},   // 左上

        // 左
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // 左上
        {{ -5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{ -5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // 右上

        // 右(左面とX座標の符号が逆)
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // 右上
        {{  5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{  5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // 左上

        // 下
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // 左上
        {{  5.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // 右上

        // 上(下面とY座標の符号が逆)
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // 右上
        {{ -5.0f,  5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{ -5.0f,  5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // 左上
    };

    // インデックスデータ配列
    indices =
    {
        // 前
         0,  1,  2, // 三角形1つ目
         2,  1,  3, // 三角形2つ目

        // 後
         7,  6,  4, // 三角形3つ目
         4,  6,  5, // 三角形4つ目

        // 左
         8,  9, 10, // 三角形5つ目
        10,  9, 11, // 三角形6つ目

        // 右
        12, 13, 14, // 三角形7つ目
        14, 13, 15, // 三角形8つ目

        // 下
        16, 18, 17, // 三角形9つ目
        17, 18, 19, // 三角形10つ目

        // 上
        20, 22, 21, // 三角形11つ目
        21, 22, 23, // 三角形12つ目
    };
}

// 三角錐を生成
void Mesh::CreateTriangularPyramid()
{
    const float TOP_HEIGHT = 10.0f; // 天井の高さ
    const int DIV = 3;
    const float ASPECT = static_cast<float>(WINDOW_WIDTH) /
        static_cast<float>(WINDOW_HEIGHT);   // アスペクト比
    float radius = 5.0f;
    const float PI2DIV = 2.0f * XM_2PI / DIV;

    // 数式使用
    // 頂点データ配列
    //vertices.resize(DIV + 5);
    //for ( int i = 0; i < DIV; i++ )
    //{
    //    vertices[i].pos.x = radius * sinf(PI2DIV * i) * ASPECT;
    //    vertices[i].pos.y = radius * cosf(PI2DIV * i) * ASPECT;
    //    vertices[i].pos.z = 0.0f;
    //}
    //vertices[DIV] = { XMFLOAT3(0.0f, 0.0f, 0.0f) };
    //vertices[DIV + 1] = { XMFLOAT3(0.0f, 0.0f, -TOP_HEIGHT * ASPECT) };

    //// インデックスデータ配列
    //indices =
    //{
    //    // 底面
    //     0,  3,  1,
    //     1,  3,  2,
    //     2,  3,  0,

    //     // 側面
    //     0,  1,  4,
    //     1,  2,  4,
    //     2,  0,  4,
    //};

    // 数式未使用
    // 頂点データ配列
    vertices =
    {
        /*     X      Y      Z    法線       U     V     */
        // 底面
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // 左後ろ
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // 右後ろ
        {{  0.0f, -5.0f, -5.0f }, { }, { 0.0f, 0.0f }},  // 前

        // 前左側面
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{  0.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  0.0f,  5.0f,  0.0f }, { }, { 1.0f, 0.0f }},  // 上

        // 前右側面
        {{  0.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  0.0f,  5.0f,  0.0f }, { }, { 0.0f, 0.0f }},  // 上

        // 後ろ側面
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // 左下
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // 右下
        {{  0.0f,  5.0f,  0.0f }, { }, { 0.0f, 0.0f }},  // 上
    };

    // インデックスデータ配列
    indices =
    {
        // 底面
        0, 2, 1,

        // 前左側面
        3, 5, 4,

        // 前右側面
        6, 8, 7,

        // 後ろ側面
        9, 10, 11
    };
}

// 図形を生成
void Mesh::CreateMesh(MeshList shapeList)
{
    switch ( shapeList )
    {
        case MeshList::e_RECTANGLE:            // 四角形
            CreateRectangle();
            break;

        case MeshList::e_CUBE:                 // 立方体
            CreateCube();
            break;

        case MeshList::e_TRIANGULAR_PYRAMID:   // 三角錐
            CreateTriangularPyramid();
            break;

        default:
            break;
    }
}

// 頂点バッファの生成
HRESULT Mesh::CreateVertBuffer()
{
    // 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
    UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

    // 頂点バッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&vertBuffer));
    assert(SUCCEEDED(result));

    // 頂点バッファビューの作成
    vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeVB;
    vbView.StrideInBytes = sizeof(Vertex);

    return result;
}

// インデックスバッファの生成
HRESULT Mesh::CreateIndexBuffer()
{
    // インデックスデータ全体のサイズ
    UINT sizeIB = static_cast<UINT>(indices.size() * sizeof(short));

    // インデックスバッファの生成
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&indexBuffer));
    assert(SUCCEEDED(result));

    // GPU上のバッファに対応した仮想メモリを取得
    unsigned short *indexMap = nullptr;
    result = indexBuffer->Map(0, nullptr, (void **)&indexMap);
    assert(SUCCEEDED(result));

    // 全インデックスに対して
    for ( int i = 0; i < indices.size(); ++i )
    {
        indexMap[i] = indices[i]; // インデックスをコピー
    }

    // マップを解除
    indexBuffer->Unmap(0, nullptr);

    // インデックスバッファビューの作成
    ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    ibView.Format = DXGI_FORMAT_R16_UINT;
    ibView.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(short));

    return result;
}

// 法線を計算する
void Mesh::CalcNormal()
{
    for ( int i = 0; i < indices.size() / 3; ++i )
    {
        // 三角形1つごとに計算していく
        unsigned short index0 = indices[i * 3 + 0];
        unsigned short index1 = indices[i * 3 + 1];
        unsigned short index2 = indices[i * 3 + 2];

        // 三角形を構成るする頂点座標をベクトルに代入
        XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
        XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
        XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

        // p0→p1ベクトル、p0→p2ベクトルを計算(ベクトルの減算)
        XMVECTOR v1 = XMVectorSubtract(p1, p0);
        XMVECTOR v2 = XMVectorSubtract(p2, p0);

        // 外積は両方から垂直なベクトル
        XMVECTOR normal = XMVector3Cross(v1, v2);

        // 正規化(長さを1にする)
        normal = XMVector3Normalize(normal);

        // 求めた法線を頂点データに代入
        XMStoreFloat3(&vertices[index0].normal, normal);
        XMStoreFloat3(&vertices[index1].normal, normal);
        XMStoreFloat3(&vertices[index2].normal, normal);
    }
}

// 頂点バッファへの転送
HRESULT Mesh::TransferVertBuffer()
{
    // GPU上のバッファに対応した仮想メモリを取得
    Vertex *vertMap = nullptr;
    auto result = vertBuffer->Map(0, nullptr, (void **)&vertMap);
    assert(SUCCEEDED(result));

    // 全頂点に対して
    for ( int i = 0; i < vertices.size(); ++i )
    {
        vertMap[i] = vertices[i]; // 座標をコピー
    }

    // マップを解除
    vertBuffer->Unmap(0, nullptr);

    return result;
}

void Mesh::Update()
{
    // 頂点バッファへのデータ転送
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}