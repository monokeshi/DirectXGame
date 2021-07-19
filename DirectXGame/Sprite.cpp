#include "Sprite.h"
#include "DirectX12Wrapper.h"
#include "Define.h"
#include "Render.h"
#include "Texture.h"

#include <d3dx12.h>

using namespace DirectX;

Sprite::Sprite(
    DirectX::XMFLOAT3 position,
    float rotation,
    DirectX::XMFLOAT4 color,
    DirectX::XMFLOAT2 size,
    DirectX::XMMATRIX *matWorldParent,
    DirectX12Wrapper &dx12,
    Render &render,
    Texture &texture):
    position(position), rotation(rotation), color(color), size(size), matWorld(matWorld), dx12(dx12), render(render), texture(texture)
{
    anchorPoint = { 0.5f,0.5f };
    isFlipX = isFlipY = false;
    isSetTexResolutionOnlyOnce = true;
    //dirtyFlag = false;

    UpdateMatrix();
}

Sprite::~Sprite()
{
}

// �s��̍X�V����
void Sprite::UpdateMatrix()
{
    matWorld = XMMatrixIdentity();                                          // �P�ʍs��
    matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));            // Z����]
    matWorld *= XMMatrixTranslation(position.x, position.y, position.z);    // ���s�ړ�

    // �e���[���h�s��null�ł͂Ȃ� ���̃I�u�W�F�N�g�͎q�ƂȂ�
    if ( matWorldParent != nullptr )
    {
        // �e�I�u�W�F�N�g�̃��[���h�s���������
        matWorld *= (*matWorldParent);
    }
}

// ���_����
void Sprite::CreateVertex()
{
    vertices =
    {
        /*      X       Y     Z         U     V      */
        {{   0.0f, 100.0f, 0.0f }, { 0.0f, 1.0f }},  // ����
        {{   0.0f,   0.0f, 0.0f }, { 0.0f, 0.0f }},  // ����
        {{ 100.0f, 100.0f, 0.0f }, { 1.0f, 1.0f }},  // �E��
        {{ 100.0f,   0.0f, 0.0f }, { 1.0f, 0.0f }},  // �E��
    };

    //enum
    //{
    //    LB, // ����
    //    LT, // ����
    //    RB, // �E��
    //    RT  // �E��
    //};

    //vertices[LB].pos = { 0.0f, size.y, 0.0f };      // ����
    //vertices[LT].pos = { 0.0f,   0.0f, 0.0f };      // ����
    //vertices[RB].pos = { size.x, size.y, 0.0f };    // �E��
    //vertices[RT].pos = { size.x,   0.0f, 0.0f };    // �E��
}

// ���_�o�b�t�@�̐���
HRESULT Sprite::CreateVertBuffer()
{
    //// ���_�o�b�t�@�̐���
    //auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
    //                                                        D3D12_HEAP_FLAG_NONE,
    //                                                        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
    //                                                        D3D12_RESOURCE_STATE_GENERIC_READ,
    //                                                        nullptr,
    //                                                        IID_PPV_ARGS(&vertBuffer));
    //assert(SUCCEEDED(result));

    //// ���_�o�b�t�@�r���[�̍쐬
    //vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
    //vbView.SizeInBytes = sizeof(vertices);
    //vbView.StrideInBytes = sizeof(vertices[0]);

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

// ���_�o�b�t�@�̓]��
HRESULT Sprite::TransferVertBuffer()
{
    // GPU��̃o�b�t�@�ɑΉ��������z���������擾
    Vertex *vertMap = nullptr;
    auto result = vertBuffer->Map(0, nullptr, (void **)&vertMap);
    assert(SUCCEEDED(result));

    // �S���_�ɑ΂���
    for ( int i = 0; i < vertices.size(); ++i )
    {
        vertMap[i] = vertices[i]; // �R�s�[
    }

    // �}�b�v������
    vertBuffer->Unmap(0, nullptr);

    return result;
}

// �萔�o�b�t�@�̐���
HRESULT Sprite::CreateConstBuffer()
{
    UINT sizeCB = static_cast<UINT>((sizeof(ConstBufferData) + 0xff) & ~0xff);

    // �萔�o�b�t�@�̐���
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeCB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&constBuffer));

    return result;
}

// �萔�o�b�t�@�ւ̃f�[�^�]��
HRESULT Sprite::TransferConstBuffer()
{
    // �}�b�s���O
    ConstBufferData *constMap = nullptr;
    auto result = constBuffer->Map(0, nullptr, (void **)&constMap);
    assert(SUCCEEDED(result));

    constMap->color = color; // �F���w��
    // ���s���e
    constMap->mat = matWorld * XMMatrixOrthographicOffCenterLH(0.0f,
                                                               static_cast<float>(WINDOW_WIDTH),
                                                               static_cast<float>(WINDOW_HEIGHT),
                                                               0.0f,
                                                               0.0f,
                                                               1.0f);

    // �}�b�s���O����
    constBuffer->Unmap(0, nullptr);

    return result;
}

// ���_���W���v�Z
void Sprite::CalcVertPos()
{
    float left = (0.0f - anchorPoint.x) * size.x;;
    float right = (1.0f - anchorPoint.x) * size.x;
    float top = (0.0f - anchorPoint.y) * size.y;
    float bottom = (1.0f - anchorPoint.y) * size.y;

    if ( isFlipX )
    {
        left *= -1;
        right *= -1;
    }
    if ( isFlipY )
    {
        top *= -1;
        bottom *= -1;
    }

    vertices[LB].pos = { left, bottom, 0.0f };  // ����
    vertices[LT].pos = { left,    top, 0.0f };  // ����
    vertices[RB].pos = { right, bottom, 0.0f }; // �E��
    vertices[RT].pos = { right,   top, 0.0f };  // �E��
}

// ���_UV���W���v�Z
void Sprite::CalcClippingVertUV(XMFLOAT2 texLeftTop, XMFLOAT2 texSize, const int &texIndex)
{
    XMFLOAT2 size = texture.GetSpriteTextureSize(texIndex);
    float texLeft = texLeftTop.x / size.x;
    float texRight = (texLeftTop.x + texSize.x) / size.x;
    float texTop = texLeftTop.y / size.y;
    float texBottom = (texLeftTop.y + texSize.y) / size.y;

    vertices[LB].uv = { texLeft,  texBottom };   // ����
    vertices[LT].uv = { texLeft,     texTop };   // ����
    vertices[RB].uv = { texRight, texBottom };   // �E��
    vertices[RT].uv = { texRight,    texTop };   // �E��
}

// ����������
HRESULT Sprite::Initialize()
{
    // ���_����
    CreateVertex();

    // ���_�o�b�t�@�̐���
    auto result = CreateVertBuffer();
    assert(SUCCEEDED(result));

    // ���_�o�b�t�@�ւ̃f�[�^�]��
    result = TransferVertBuffer();
    assert(SUCCEEDED(result));

    // �萔�o�b�t�@
    result = CreateConstBuffer();
    assert(SUCCEEDED(result));

    // �萔�o�b�t�@�ւ̓]��
    result = TransferConstBuffer();
    assert(SUCCEEDED(result));

    return result;
}

// �e�N�X�`����؂�o��
void Sprite::ClippingTexture(XMFLOAT2 texLeftTop, XMFLOAT2 texSize, const int &texIndex)
{
    CalcClippingVertUV(texLeftTop, texSize, texIndex);

    // ���_�o�b�t�@�ւ̃f�[�^�]��
    if ( FAILED(TransferVertBuffer()) )
    {
        assert(0);
        return;
    }
}

// �X�V����
void Sprite::Update()
{
    /*if ( !dirtyFlag )
    {
        return;
    }*/

    // �s��̍X�V
    UpdateMatrix();

    // �萔�o�b�t�@�ւ̓]��
    if ( FAILED(TransferConstBuffer()) )
    {
        assert(0);
        return;
    }
}

// �`�揈��
void Sprite::Draw(const int &texIndex, bool isFlipX, bool isFlipY, bool isInvisible, bool isSetTexResolution)
{
    if ( isInvisible )
    {
        return;
    }

    // �|���S���T�C�Y���e�N�X�`���摜�̉𑜓x�ɐݒ�(��x����)
    if ( isSetTexResolution &&
        isSetTexResolutionOnlyOnce )
    {
        SetSize(texture.GetSpriteTextureSize(texIndex));
        isSetTexResolutionOnlyOnce = false;
    }

    // ���]
    if ( this->isFlipX != isFlipX || this->isFlipY != isFlipY )
    {
        this->isFlipX = isFlipX;
        this->isFlipY = isFlipY;
        CalcVertPos();

        // �萔�o�b�t�@�ւ̓]��
        if ( FAILED(TransferVertBuffer()) )
        {
            assert(0);
            return;
        }
    }

    // ���_�o�b�t�@�r���[���Z�b�g
    dx12.GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

    // �萔�o�b�t�@���Z�b�g
    dx12.GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());

    // �V�F�[�_�[���\�[�X�r���[���Z�b�g
    dx12.GetCmdList()->SetGraphicsRootDescriptorTable(1,
                                                      CD3DX12_GPU_DESCRIPTOR_HANDLE(render.GetSpriteDescHeap()->GetGPUDescriptorHandleForHeapStart(),
                                                                                    texIndex,
                                                                                    dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    // �|���S���̕`��(4���_�Ŏl�p�`)
    dx12.GetCmdList()->DrawInstanced(4, 1, 0, 0);
}

//// �T�C�Y��ݒ�
//void Sprite::SetSize(XMFLOAT2 size)
//{
//    // �|���S���T�C�Y���e�N�X�`���摜�̉𑜓x�ɐݒ�t���O��false
//    isSetTexResolutionOnlyOnce = false;
//
//    float left = (0.0f - anchorPoint.x) * size.x;
//    float right = (1.0f - anchorPoint.x) * size.x;
//    float top = (0.0f - anchorPoint.y) * size.y;
//    float bottom = (1.0f - anchorPoint.y) * size.y;
//
//    vertices[LB].pos = { left, bottom, 0.0f };  // ����
//    vertices[LT].pos = { left,    top, 0.0f };  // ����
//    vertices[RB].pos = { right, bottom, 0.0f }; // �E��
//    vertices[RT].pos = { right,   top, 0.0f };  // �E��
//
//    // ���_�o�b�t�@�ւ̃f�[�^�]��
//    if ( FAILED(TransferVertBuffer()) )
//    {
//        assert(0);
//        return;
//    }
//}
