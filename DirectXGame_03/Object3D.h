#pragma once

#include "Matrix.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

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
        XMFLOAT4 color; // �F(RGBA)
        XMMATRIX mat;   // 3D�ϊ��s��
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Matrix *matrix;

    int instanceNum;        // ���g�̃N���X�̃C���X�^���X��

    XMFLOAT3 position;                          // �|�W�V����
    XMFLOAT3 scale;                             // �X�P�[�����O�{��
    XMFLOAT3 rotation;                          // ��]�p
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer = nullptr;      // �萔�o�b�t�@
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;

    // �萔�o�b�t�@�̐���
    HRESULT CreateConstBuffer();

    // �萔�o�b�t�@�ւ̃f�[�^�]��
    HRESULT TransferConstBuffer();
public:
    Object3D(XMFLOAT3 position,
             XMFLOAT3 scale,
             XMFLOAT3 rotation,
             XMMATRIX &matView,
             XMMATRIX *matWorldParent,
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
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
              D3D12_VERTEX_BUFFER_VIEW vbView,
              D3D12_INDEX_BUFFER_VIEW ibView,
              int indicesNum);

    // �ړ�
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // ��]
    void RotateX(float rotation);
    void RotateY(float rotation);
    void RotateZ(float rotation);

    XMMATRIX *GetMatWorld()
    {
        return matrix->GetMatWorld();
    }
};