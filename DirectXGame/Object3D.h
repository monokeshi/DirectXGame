#pragma once

#include "Matrix.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <d3dx12.h>

#include <vector>
#include <wrl.h>

class DirectX12Wrapper;
class Render;

class Object3D
{
private:
    // �萔�o�b�t�@�p�f�[�^�\����
    struct ConstBufferData
    {
        DirectX::XMFLOAT4 color; // �F(RGBA)
        DirectX::XMMATRIX mat;   // 3D�ϊ��s��
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Matrix *matrix;

    int instanceNum;        // ���g�̃N���X�̃C���X�^���X��

    DirectX::XMFLOAT3 position;                         // �|�W�V����
    DirectX::XMFLOAT3 scale;                            // �X�P�[�����O�{��
    DirectX::XMFLOAT3 rotation;                         // ��]�p
    DirectX::XMFLOAT4 color;                            // �F
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // �萔�o�b�t�@
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV{};
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV{};

    // �萔�o�b�t�@�̐���
    HRESULT CreateConstBuffer();

    // �萔�o�b�t�@�ւ̃f�[�^�]��
    HRESULT TransferConstBuffer();

public:
    Object3D(DirectX::XMFLOAT3 position,
             DirectX::XMFLOAT3 scale,
             DirectX::XMFLOAT3 rotation,
             DirectX::XMFLOAT4 color,
             DirectX::XMMATRIX &matView,
             DirectX::XMMATRIX *matWorldParent,
             DirectX12Wrapper &dx12,
             Render &render,
             int index,
             bool isPerspective = true);
    ~Object3D();

    // ������
    HRESULT Initialize();

    // ���t���[���X�V����
    void Update();

    // ���t���[���`�揈��
    void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &gpuDescHandleSRV,
              const D3D12_VERTEX_BUFFER_VIEW &vbView,
              const D3D12_INDEX_BUFFER_VIEW &ibView,
              int indicesNum);

    // �ړ�
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // ��]
    void RotateX(float rotation);
    void RotateY(float rotation);
    void RotateZ(float rotation);

    DirectX::XMMATRIX *GetMatWorld()
    {
        return matrix->GetMatWorld();
    }
};