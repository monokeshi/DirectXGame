#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include <wrl.h>

class DirectX12Wrapper;

class Render
{
private:
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct PipelineSet
    {
        ComPtr<ID3D12RootSignature> rootSignature;  // ���[�g�V�O�l�`��
        ComPtr<ID3D12PipelineState> pipelineState;  // �p�C�v���C��
    };

    PipelineSet obj3DPipeline;
    PipelineSet spritePipeline;

    DirectX12Wrapper &dx12;

    ComPtr<ID3D12DescriptorHeap> basicDescHeap;
    CD3DX12_DESCRIPTOR_RANGE descRangeCBV{};                    // �f�X�N���v�^�����W �萔�p
    CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};                    // �f�X�N���v�^�����W �e�N�X�`���p
    CD3DX12_ROOT_PARAMETER rootParams[2]{};                     // ���[�g�p�����[�^
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};                    // �e�N�X�`���T���v���[

    // �V�F�[�_�[�܂��
    ComPtr<ID3DBlob> vsBlob = nullptr;    // ���_�V�F�[�_�I�u�W�F�N�g
    ComPtr<ID3DBlob> psBlob = nullptr;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
    ComPtr<ID3DBlob> errorBlob = nullptr; //�G���[�I�u�W�F�N�g

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> spriteDescHeap;    // �X�v���C�g�p�f�X�N���v�^�q�[�v

    // �f�X�N���v�^�q�[�v�̐���
    HRESULT CreateDescriptorHeapObject3D(); // �I�u�W�F�N�g3D�p
    HRESULT CreateDescriptorHeapSprite();   // �X�v���C�g�p

    // ���[�g�p�����[�^�̐ݒ�
    void SettingRootParameter();

    // �e�N�X�`���T���v���[�̐ݒ�
    void SettingTextureSampler();

    // ���[�g�V�O�l�`������
    HRESULT CreateRootSignatureObject3D();  // �I�u�W�F�N�g3D�p
    HRESULT CreateRootSignatureSprite();    // �X�v���C�g�p

    // �p�C�v���C������
    HRESULT CreateGraphicsPipelineObject3D();   // �I�u�W�F�N�g3D�p
    HRESULT CreateGraphicsPipelineSprite();     // �X�v���C�g�p

public:
    Render(DirectX12Wrapper &dx12);
    ~Render();

    // �I�u�W�F�N�g3D�̃p�C�v���C���Z�b�g
    void BeginDrawObj3D();

    // �X�v���C�g�̃p�C�v���C���Z�b�g
    void BeginDrawSprite();

    ID3D12DescriptorHeap *GetBasicDescHeap()
    {
        return basicDescHeap.Get();
    }
    
    ID3D12DescriptorHeap *GetSpriteDescHeap()
    {
        return spriteDescHeap.Get();
    }
};