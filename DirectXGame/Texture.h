#pragma once

#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>

#include <wrl.h>

class DirectX12Wrapper;
class Render;

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
    Render &render;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;

    // �e�N�X�`�������p
    static const int TEX_WIDTH = 256;                          // ���s�N�Z����
    static const int IMAGE_DATA_COUNT = TEX_WIDTH * TEX_WIDTH; // �z��v�f��
    DirectX::XMFLOAT4 *imageData;

    // �e�N�X�`���ǂݍ��ݗp
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    ID3D12Resource *texBuffObj3D = nullptr;         // �I�u�W�F�N�g�p�e�N�X�`���o�b�t�@
    std::vector<ID3D12Resource *> texBuffSprite;    // �X�v���C�g�p�e�N�X�`���o�b�t�@
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};      // �ݒ�\����

    std::vector<CD3DX12_RESOURCE_DESC> texResDescObj3D;
    std::vector<CD3DX12_RESOURCE_DESC> texResDescSprite;

    int obj3DTexturNum;     // �I�u�W�F�N�g�p�e�N�X�`����
    int spriteTextureNum;   // �X�v���C�g�p�e�N�X�`����

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV{};

    // �e�N�X�`���o�b�t�@�̐���
    HRESULT CreateTextureBufferObj3D();     // �I�u�W�F�N�g3D�p
    HRESULT CreateTextureBufferSprite();    // �X�v���C�g�p

    // �V�F�[�_�[���\�[�X�r���[�̐���
    void CreateShaderResouceViewObject3D(); // �I�u�W�F�N�g3D�p
    void CreateShaderResouceViewSprite();   // �X�v���C�g�p

public:
    Texture(DirectX12Wrapper &dx12, Render &render, ID3D12DescriptorHeap *basicDescHeap);
    ~Texture();

    // �e�N�X�`���̐���
    D3D12_GPU_DESCRIPTOR_HANDLE CreateTexture(DirectX::XMFLOAT4 color);

    // �e�N�X�`���̓ǂݍ���
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const wchar_t *fileName);

    // �X�v���C�g�p�e�N�X�`���̓ǂݍ���
    int LoadSpriteTexture(const wchar_t *fileName);

    const DirectX::XMFLOAT2 &GetObj3DTextureSize(const int &handle) const
    {
        return { static_cast<float>(texResDescObj3D[handle].Width),
                 static_cast<float>(texResDescObj3D[handle].Height) };
    }

    const DirectX::XMFLOAT2 &GetSpriteTextureSize(const int &handle) const
    {
        return { static_cast<float>(texResDescSprite[handle].Width),
                 static_cast<float>(texResDescSprite[handle].Height) };
    }
};