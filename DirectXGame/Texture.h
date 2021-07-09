#pragma once

#include <d3d12.h>
#include <DirectXTex.h>

#include <wrl.h>

class DirectX12Wrapper;

class Texture
{
private:
    // �e�N�X�`���̏��������@
    enum class TextureInitialize
    {
        e_CREATE,   // ����
        e_LOAD      // �ǂݍ���
    };

    TextureInitialize texInit;

    DirectX12Wrapper &dx12;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    // �e�N�X�`�������p
    const int TEX_WIDTH = 256;                          // ���s�N�Z����
    const int IMAGE_DATA_COUNT = TEX_WIDTH * TEX_WIDTH; // �z��v�f��
    DirectX::XMFLOAT4 *imageData;

    // �e�N�X�`���ǂݍ��ݗp
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    ID3D12Resource *texBuff = nullptr;                  // �e�N�X�`���o�b�t�@
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};          // �ݒ�\����

    int textureNum; // �e�N�X�`����

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // �e�N�X�`���o�b�t�@�̐���
    HRESULT CreateTextureBuffer();

    // �V�F�[�_�[���\�[�X�r���[�̐���
    void CreateShaderResouceView();

public:
    Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // �e�N�X�`���̐���
    D3D12_GPU_DESCRIPTOR_HANDLE CreateTexture(DirectX::XMFLOAT4 color);

    // �e�N�X�`���̓ǂݍ���
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);
};