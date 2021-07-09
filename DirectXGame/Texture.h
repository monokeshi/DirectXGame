#pragma once

#include <d3d12.h>
#include <DirectXTex.h>

#include <wrl.h>

class DirectX12Wrapper;

class Texture
{
private:
    DirectX12Wrapper &dx12;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};
    ID3D12Resource *texBuff = nullptr;                  // �e�N�X�`���o�b�t�@
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};          // �ݒ�\����

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // �e�N�X�`���o�b�t�@�̐���
    HRESULT CreateTextureBuffer();

    // �V�F�[�_�[���\�[�X�r���[�̐���
    void CreateShaderResouceView();

public:
    Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // �e�N�X�`���̓ǂݍ���
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);
};