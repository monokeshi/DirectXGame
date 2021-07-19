#include "Mesh.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

using namespace DirectX;

Mesh::Mesh(MeshList shapeList, DirectX12Wrapper &dx12):
    dx12(dx12)
{
    // �}�`�ɉ��������_�A�C���f�b�N�X�̐���
    CreateMesh(shapeList);

    // ���_�o�b�t�@�̐���
    if ( FAILED(CreateVertBuffer()) )
    {
        assert(0);
        return;
    }

    // �C���f�b�N�X�o�b�t�@�̐���
    if ( FAILED(CreateIndexBuffer()) )
    {
        assert(0);
        return;
    }

    // �@���̌v�Z
    CalcNormal();

    // ���_�o�b�t�@�ւ̃f�[�^�]��
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}

Mesh::~Mesh()
{
}


// �l�p�`(����)�𐶐�
void Mesh::CreateRectangle()
{
    // ���_�f�[�^�z��
    vertices =
    {
        /*     X      Y    Z     �@��       U     V     */
        {{ -5.0f, -5.0f, 0.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{ -5.0f,  5.0f, 0.0f }, { }, { 0.0f, 0.0f }},  // ����
        {{  5.0f, -5.0f, 0.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  5.0f,  5.0f, 0.0f }, { }, { 1.0f, 0.0f }},  // �E��
    };

    // �C���f�b�N�X�f�[�^�z��
    indices =
    {
         0,  1,  2, // �O�p�`1��
         2,  1,  3, // �O�p�`2��
    };
}

// �����̂𐶐�
void Mesh::CreateCube()
{
    // ���_�f�[�^�z��
    vertices =
    {
        /*     X      Y     Z     �@��       U     V     */
        // �O
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{ -5.0f,  5.0f, -5.0f }, { }, { 0.0f, 0.0f }},  // ����
        {{  5.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 0.0f }},  // �E��

        // ��(�O�ʂ�Z���W�̕������t)
        {{  5.0f,  5.0f, 5.0f }, { }, { 1.0f, 0.0f }},   // �E��
        {{  5.0f, -5.0f, 5.0f }, { }, { 1.0f, 1.0f }},   // �E��
        {{ -5.0f, -5.0f, 5.0f }, { }, { 0.0f, 1.0f }},   // ����
        {{ -5.0f,  5.0f, 5.0f }, { }, { 0.0f, 0.0f }},   // ����

        // ��
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // ����
        {{ -5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{ -5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // �E��

        // �E(���ʂ�X���W�̕������t)
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // �E��
        {{  5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{  5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // ����

        // ��
        {{ -5.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // ����
        {{  5.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // �E��

        // ��(���ʂ�Y���W�̕������t)
        {{  5.0f,  5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  5.0f,  5.0f,  5.0f }, { }, { 1.0f, 0.0f }},  // �E��
        {{ -5.0f,  5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{ -5.0f,  5.0f,  5.0f }, { }, { 0.0f, 0.0f }},  // ����
    };

    // �C���f�b�N�X�f�[�^�z��
    indices =
    {
        // �O
         0,  1,  2, // �O�p�`1��
         2,  1,  3, // �O�p�`2��

        // ��
         7,  6,  4, // �O�p�`3��
         4,  6,  5, // �O�p�`4��

        // ��
         8,  9, 10, // �O�p�`5��
        10,  9, 11, // �O�p�`6��

        // �E
        12, 13, 14, // �O�p�`7��
        14, 13, 15, // �O�p�`8��

        // ��
        16, 18, 17, // �O�p�`9��
        17, 18, 19, // �O�p�`10��

        // ��
        20, 22, 21, // �O�p�`11��
        21, 22, 23, // �O�p�`12��
    };
}

// �O�p���𐶐�
void Mesh::CreateTriangularPyramid()
{
    const float TOP_HEIGHT = 10.0f; // �V��̍���
    const int DIV = 3;
    const float ASPECT = static_cast<float>(WINDOW_WIDTH) /
        static_cast<float>(WINDOW_HEIGHT);   // �A�X�y�N�g��
    float radius = 5.0f;
    const float PI2DIV = 2.0f * XM_2PI / DIV;

    // �����g�p
    // ���_�f�[�^�z��
    //vertices.resize(DIV + 5);
    //for ( int i = 0; i < DIV; i++ )
    //{
    //    vertices[i].pos.x = radius * sinf(PI2DIV * i) * ASPECT;
    //    vertices[i].pos.y = radius * cosf(PI2DIV * i) * ASPECT;
    //    vertices[i].pos.z = 0.0f;
    //}
    //vertices[DIV] = { XMFLOAT3(0.0f, 0.0f, 0.0f) };
    //vertices[DIV + 1] = { XMFLOAT3(0.0f, 0.0f, -TOP_HEIGHT * ASPECT) };

    //// �C���f�b�N�X�f�[�^�z��
    //indices =
    //{
    //    // ���
    //     0,  3,  1,
    //     1,  3,  2,
    //     2,  3,  0,

    //     // ����
    //     0,  1,  4,
    //     1,  2,  4,
    //     2,  0,  4,
    //};

    // �������g�p
    // ���_�f�[�^�z��
    vertices =
    {
        /*     X      Y      Z    �@��       U     V     */
        // ���
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // �����
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // �E���
        {{  0.0f, -5.0f, -5.0f }, { }, { 0.0f, 0.0f }},  // �O

        // �O������
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{  0.0f, -5.0f, -5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  0.0f,  5.0f,  0.0f }, { }, { 1.0f, 0.0f }},  // ��

        // �O�E����
        {{  0.0f, -5.0f, -5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  0.0f,  5.0f,  0.0f }, { }, { 0.0f, 0.0f }},  // ��

        // ��둤��
        {{ -5.0f, -5.0f,  5.0f }, { }, { 0.0f, 1.0f }},  // ����
        {{  5.0f, -5.0f,  5.0f }, { }, { 1.0f, 1.0f }},  // �E��
        {{  0.0f,  5.0f,  0.0f }, { }, { 0.0f, 0.0f }},  // ��
    };

    // �C���f�b�N�X�f�[�^�z��
    indices =
    {
        // ���
        0, 2, 1,

        // �O������
        3, 5, 4,

        // �O�E����
        6, 8, 7,

        // ��둤��
        9, 10, 11
    };
}

// �}�`�𐶐�
void Mesh::CreateMesh(MeshList shapeList)
{
    switch ( shapeList )
    {
        case MeshList::e_RECTANGLE:            // �l�p�`
            CreateRectangle();
            break;

        case MeshList::e_CUBE:                 // ������
            CreateCube();
            break;

        case MeshList::e_TRIANGULAR_PYRAMID:   // �O�p��
            CreateTriangularPyramid();
            break;

        default:
            break;
    }
}

// ���_�o�b�t�@�̐���
HRESULT Mesh::CreateVertBuffer()
{
    // ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
    UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

    // ���_�o�b�t�@�̐���
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&vertBuffer));
    assert(SUCCEEDED(result));

    // ���_�o�b�t�@�r���[�̍쐬
    vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeVB;
    vbView.StrideInBytes = sizeof(Vertex);

    return result;
}

// �C���f�b�N�X�o�b�t�@�̐���
HRESULT Mesh::CreateIndexBuffer()
{
    // �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
    UINT sizeIB = static_cast<UINT>(indices.size() * sizeof(short));

    // �C���f�b�N�X�o�b�t�@�̐���
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&indexBuffer));
    assert(SUCCEEDED(result));

    // GPU��̃o�b�t�@�ɑΉ��������z���������擾
    unsigned short *indexMap = nullptr;
    result = indexBuffer->Map(0, nullptr, (void **)&indexMap);
    assert(SUCCEEDED(result));

    // �S�C���f�b�N�X�ɑ΂���
    for ( int i = 0; i < indices.size(); ++i )
    {
        indexMap[i] = indices[i]; // �C���f�b�N�X���R�s�[
    }

    // �}�b�v������
    indexBuffer->Unmap(0, nullptr);

    // �C���f�b�N�X�o�b�t�@�r���[�̍쐬
    ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    ibView.Format = DXGI_FORMAT_R16_UINT;
    ibView.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(short));

    return result;
}

// �@�����v�Z����
void Mesh::CalcNormal()
{
    for ( int i = 0; i < indices.size() / 3; ++i )
    {
        // �O�p�`1���ƂɌv�Z���Ă���
        unsigned short index0 = indices[i * 3 + 0];
        unsigned short index1 = indices[i * 3 + 1];
        unsigned short index2 = indices[i * 3 + 2];

        // �O�p�`���\���邷�钸�_���W���x�N�g���ɑ��
        XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
        XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
        XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

        // p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z(�x�N�g���̌��Z)
        XMVECTOR v1 = XMVectorSubtract(p1, p0);
        XMVECTOR v2 = XMVectorSubtract(p2, p0);

        // �O�ς͗������琂���ȃx�N�g��
        XMVECTOR normal = XMVector3Cross(v1, v2);

        // ���K��(������1�ɂ���)
        normal = XMVector3Normalize(normal);

        // ���߂��@���𒸓_�f�[�^�ɑ��
        XMStoreFloat3(&vertices[index0].normal, normal);
        XMStoreFloat3(&vertices[index1].normal, normal);
        XMStoreFloat3(&vertices[index2].normal, normal);
    }
}

// ���_�o�b�t�@�ւ̓]��
HRESULT Mesh::TransferVertBuffer()
{
    // GPU��̃o�b�t�@�ɑΉ��������z���������擾
    Vertex *vertMap = nullptr;
    auto result = vertBuffer->Map(0, nullptr, (void **)&vertMap);
    assert(SUCCEEDED(result));

    // �S���_�ɑ΂���
    for ( int i = 0; i < vertices.size(); ++i )
    {
        vertMap[i] = vertices[i]; // ���W���R�s�[
    }

    // �}�b�v������
    vertBuffer->Unmap(0, nullptr);

    return result;
}

void Mesh::Update()
{
    // ���_�o�b�t�@�ւ̃f�[�^�]��
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}