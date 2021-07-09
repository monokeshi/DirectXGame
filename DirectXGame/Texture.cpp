#include "Texture.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

#include <d3dx12.h>
#pragma comment(lib, "d3d12.lib")

#include <assert.h>

using namespace DirectX;

Texture::Texture(DirectX12Wrapper &dx12, ID3D12DescriptorHeap *basicDescHeap):
    dx12(dx12), basicDescHeap(basicDescHeap)
{
}

Texture::~Texture()
{
}


// �e�N�X�`���o�b�t�@�̐���
HRESULT Texture::CreateTextureBuffer()
{
    const Image *img = scratchImg.GetImage(0, 0, 0);    // ���f�[�^���o

    // ���\�[�X�ݒ�
    CD3DX12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format,
                                                                    metadata.width,
                                                                    static_cast<UINT>(metadata.height),
                                                                    static_cast<UINT16>(metadata.arraySize),
                                                                    static_cast<UINT16>(metadata.mipLevels));

    // �e�N�X�`���o�b�t�@�̐���
    auto result = dx12.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
                                                                                     D3D12_MEMORY_POOL_L0),
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &texResDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,  // �e�N�X�`���p�w��
                                                            nullptr,
                                                            IID_PPV_ARGS(&texBuff));
    assert(SUCCEEDED(result));

    // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
    result = texBuff->WriteToSubresource(0,
                                         nullptr,                              // �S�̈�փR�s�[
                                         img->pixels,                          // ���f�[�^�A�h���X
                                         static_cast<UINT>(img->rowPitch),     // 1���C���T�C�Y
                                         static_cast<UINT>(img->slicePitch));  // �S�T�C�Y

    return result;
}

// �V�F�[�_�[���\�[�X�r���[�̐���
void Texture::CreateShaderResouceView()
{
    // �f�X�N���v�^�q�[�v�̐擪�n���h�����擾
    cpuDescHandleSRV = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
    gpuDescHandleSRV = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

    // �n���h���̃A�h���X��i�߂�
    static int textureNum = 0;
    UINT descHandleIncrementSize = dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + textureNum) * descHandleIncrementSize;
    gpuDescHandleSRV.ptr += (CONSTANT_BUFFER_NUM + textureNum) * descHandleIncrementSize;

    ++textureNum;
    // �e�N�X�`�������f�X�N���v�^�q�[�v�Őݒ肵���e�N�X�`���o�b�t�@��������Ȃ�
    if ( textureNum > TEXTURE_BUFFER_NUM )
    {
        assert(0);
        return;
    }

    // �V�F�[�_�[���\�[�X�r���[�ݒ�
    srvDesc.Format = metadata.format;                       // RGBA
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  // 2D�e�N�X�`��
    srvDesc.Texture2D.MipLevels = 1;

    // �q�[�v��2�ԖڂɃV�F�[�_�[���\�[�X�r���[�쐬
    dx12.GetDevice()->CreateShaderResourceView(texBuff,     // �r���[�Ɗ֘A�t����o�b�t�@
                                               &srvDesc,    //�e�N�X�`���ݒ���
                                               cpuDescHandleSRV);
}

// �e�N�X�`���̓ǂݍ���
D3D12_GPU_DESCRIPTOR_HANDLE Texture::LoadTexture(const wchar_t *fileName)
{
    // �摜�f�[�^
    // �ǂݍ���
    auto result = LoadFromWICFile(fileName, // �摜�t�@�C��
                                  WIC_FLAGS_NONE,
                                  &metadata,
                                  scratchImg);
    assert(SUCCEEDED(result));

    // �e�N�X�`���o�b�t�@
    result = CreateTextureBuffer();
    assert(SUCCEEDED(result));

    // �V�F�[�_�[���\�[�X�r���[
    CreateShaderResouceView();

    return gpuDescHandleSRV;
}