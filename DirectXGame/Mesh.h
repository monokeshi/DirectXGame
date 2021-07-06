#pragma once

#include <d3dx12.h>
#include <DirectXMath.h>

#include <vector>

class DirectX12Wrapper;

enum class  MeshList
{
    e_RECTANGLE,            // �l�p�`
    e_CUBE,                 // ������
    e_TRIANGULAR_PYRAMID,   // �O�p��
};

class Mesh
{
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;       // xyz���W
        DirectX::XMFLOAT3 normal;    // �@���x�N�g��
        DirectX::XMFLOAT2 uv;        // uv���W
    };

    DirectX12Wrapper &dx12;

    MeshList shapeList;

    std::vector<Vertex> vertices;               // ���_�f�[�^
    ID3D12Resource *vertBuffer = nullptr;       // ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW vbView{};          // ���_�o�b�t�@�r���[
    std::vector<unsigned short> indices;        // �C���f�b�N�X�f�[�^
    ID3D12Resource *indexBuffer = nullptr;      // �C���f�b�N�X�o�b�t�@
    D3D12_INDEX_BUFFER_VIEW ibView{};           // �C���f�b�N�X�o�b�t�@�r���[

    // �l�p�`(����)�𐶐�
    void CreateRectangle();

    // �����̂𐶐�
    void CreateCube();

    // �O�p���𐶐�
    void CreateTriangularPyramid();

    // �}�`�𐶐�
    void CreateMesh(MeshList shapeList);

    // ���_�o�b�t�@�̐���
    HRESULT CreateVertBuffer();

    // �C���f�b�N�X�o�b�t�@�̐���
    HRESULT CreateIndexBuffer();

    // �@�����v�Z����
    void CalcNormal();

    // ���_�o�b�t�@�̓]��
    HRESULT TransferVertBuffer();

public:
    Mesh(MeshList shapeList, DirectX12Wrapper &dx12);
    ~Mesh();

    void Update();

    D3D12_VERTEX_BUFFER_VIEW GetVbView()
    {
        return vbView;
    }

    D3D12_INDEX_BUFFER_VIEW GetIbView()
    {
        return ibView;
    }

    int GetIndicesNum()
    {
        return static_cast<int>(indices.size());
    }
};