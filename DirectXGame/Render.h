#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include <wrl.h>

class DirectX12Wrapper;

class Render
{
private:
    DirectX12Wrapper &dx12;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap = nullptr;
    D3D12_DESCRIPTOR_RANGE descRangeCBV{};              // �f�X�N���v�^�����W �萔�p
    D3D12_DESCRIPTOR_RANGE descRangeSRV{};              // �f�X�N���v�^�����W �e�N�X�`���p
    D3D12_ROOT_PARAMETER rootParams[2]{};               // ���[�g�p�����[�^
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;       // ���[�g�V�O�l�`��
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;       // �p�C�v���C��
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};            // �e�N�X�`���T���v���[

    // �V�F�[�_�[�܂��
    ID3DBlob *vsBlob = nullptr;    // ���_�V�F�[�_�I�u�W�F�N�g
    ID3DBlob *psBlob = nullptr;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
    ID3DBlob *errorBlob = nullptr; //�G���[�I�u�W�F�N�g

    // �f�X�N���v�^�q�[�v�̐���
    HRESULT CreateDescriptorHeap();

    // �e�N�X�`���T���v���[�̐ݒ�
    void SettingTextureSampler();

    // ���[�g�V�O�l�`������
    HRESULT CreateRootSignature();

    // �p�C�v���C������
    HRESULT CreateGraphicsPipeline();

public:
    Render(DirectX12Wrapper &dx12);
    ~Render();

    // �`�揈��
    void BeginDraw();

    ID3D12DescriptorHeap *GetBasicDescHeap()
    {
        return basicDescHeap.Get();
    }
};