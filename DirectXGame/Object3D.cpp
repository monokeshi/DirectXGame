#include "Object3D.h"
#include "DirectX12Wrapper.h"
#include "Render.h"
#include "Define.h"

#include <d3dcompiler.h>

#include <string>
#include <assert.h>

using namespace DirectX;

Object3D::Object3D(XMFLOAT3 position,
                   XMFLOAT3 scale,
                   XMFLOAT3 rotation,
                   DirectX::XMFLOAT4 color,
                   XMMATRIX &matView,
                   XMMATRIX *matWorldParent,
                   DirectX12Wrapper &dx12,
                   Render &render,
                   int index,
                   bool isPerspective):
    position(position), scale(scale), rotation(rotation), color(color), dx12(dx12), render(render), instanceNum(index)
{
    matrix = new Matrix(this->position, this->scale, this->rotation, matView, matWorldParent, isPerspective);
    ++instanceNum;
}

Object3D::~Object3D()
{
    delete matrix;
}

// �萔�o�b�t�@�̐���
HRESULT Object3D::CreateConstBuffer()
{
    UINT sizeCB = static_cast<UINT>((sizeof(ConstBufferData) + 0xff) & ~0xff);

    // �萔�o�b�t�@�̐���
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &CD3DX12_RESOURCE_DESC::Buffer(sizeCB),
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(&constBuffer));

    // �f�X�N���v�^�q�[�v1���̃T�C�Y
    UINT descHandleIncrementSize = dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // CBV��CPU�f�X�N���v�^�n���h�����v�Z
    cpuDescHandleCBV = render.GetBasicDescHeap()->GetCPUDescriptorHandleForHeapStart();
    cpuDescHandleCBV.ptr += (instanceNum - 1) * descHandleIncrementSize;

    // CBV��GPU�f�X�N���v�^�n���h�����v�Z
    gpuDescHandleCBV = render.GetBasicDescHeap()->GetGPUDescriptorHandleForHeapStart();
    gpuDescHandleCBV.ptr += (instanceNum - 1) * descHandleIncrementSize;

    // �萔�o�b�t�@�r���[�̐���
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = constBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = static_cast<UINT>(constBuffer->GetDesc().Width);
    dx12.GetDevice()->CreateConstantBufferView(&cbvDesc,
                                               cpuDescHandleCBV);    // �f�X�N���v�^�q�[�v�̐擪�ɒ萔�o�b�t�@�r���[�̐���

    return result;
}

// �萔�o�b�t�@�ւ̃f�[�^�]��
HRESULT Object3D::TransferConstBuffer()
{
    // �}�b�s���O
    ConstBufferData *constMap = nullptr;
    auto result = constBuffer->Map(0, nullptr, (void **)&constMap);
    assert(SUCCEEDED(result));

    constMap->color = color;             // �F���w��
    constMap->mat = matrix->GetMat();

    // �}�b�s���O����
    constBuffer->Unmap(0, nullptr);

    return result;
}

// ������
HRESULT Object3D::Initialize()
{
    // �萔�o�b�t�@
    auto result = CreateConstBuffer();
    assert(SUCCEEDED(result));

    // �萔�o�b�t�@�̓]��
    result = TransferConstBuffer();
    assert(SUCCEEDED(result));

    return result;
}

// ���t���[���X�V����
void Object3D::Update()
{
    // �s��̍X�V����
    matrix->Update();

    // �萔�o�b�t�@�ւ̓]��
    if ( FAILED(TransferConstBuffer()) )
    {
        assert(0);
        return;
    }
}

// ���t���[���`�揈��
void Object3D::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &gpuDescHandleSRV,
                    const D3D12_VERTEX_BUFFER_VIEW &vbView,
                    const D3D12_INDEX_BUFFER_VIEW &ibView,
                    int indicesNum)
{
    // �v���~�e�B�u�`��̐ݒ�
    dx12.GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ���_�o�b�t�@�r���[���Z�b�g
    dx12.GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

    // �C���f�b�N�X�o�b�t�@�r���[���Z�b�g
    dx12.GetCmdList()->IASetIndexBuffer(&ibView);

    // �f�X�N���v�^�q�[�v���Z�b�g
    ID3D12DescriptorHeap *ppHeaps[] = { render.GetBasicDescHeap() };
    dx12.GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // �萔�o�b�t�@�r���[���Z�b�g
    dx12.GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());

    // �V�F�[�_�[���\�[�X�r���[���Z�b�g
    dx12.GetCmdList()->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

    // �`��
    dx12.GetCmdList()->DrawIndexedInstanced(static_cast<UINT>(indicesNum), 1, 0, 0, 0);
}

// X���ړ�
void Object3D::MoveX(float move)
{
    position.x += move;
}

// Y���ړ�
void Object3D::MoveY(float move)
{
    position.y += move;
}

// Z���ړ�
void Object3D::MoveZ(float move)
{
    position.z += move;
}

// X����]
void Object3D::RotateX(float rotation)
{
    this->rotation.x += rotation;
}

// Y����]
void Object3D::RotateY(float rotation)
{
    this->rotation.y += rotation;
}

// Z����]
void Object3D::RotateZ(float rotation)
{
    this->rotation.z += rotation;
}