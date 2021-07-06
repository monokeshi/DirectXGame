#include "Render.h"
#include "DirectX12Wrapper.h"
#include "Define.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <string>
#include <assert.h>

using namespace Microsoft::WRL;

Render::Render(DirectX12Wrapper &dx12):
    dx12(dx12)
{
    // �f�X�N���v�^�q�[�v
    auto result = CreateDescriptorHeap();
    assert(SUCCEEDED(result));

    // ���[�g�p�����[�^�̐ݒ�
    SettingRootParameter();

    // �e�N�X�`���T���v���[
    SettingTextureSampler();

    // ���[�g�V�O�l�`��
    result = CreateRootSignature();
    assert(SUCCEEDED(result));

    // �O���t�B�b�N�X�p�C�v���C��
    result = CreateGraphicsPipeline();
    assert(SUCCEEDED(result));
}

Render::~Render()
{
}

// �f�X�N���v�^�q�[�v�̐���
HRESULT Render::CreateDescriptorHeap()
{
    // �ݒ�\����
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // �V�F�[�_�[���猩����
    descHeapDesc.NumDescriptors = CONSTANT_BUFFER_NUM + TEXTURE_BUFFER_NUM;

    // �f�X�N���v�^�q�[�v�̐���
    auto result = dx12.GetDevice()->CreateDescriptorHeap(&descHeapDesc,
                                                         IID_PPV_ARGS(&basicDescHeap));

    return result;
}

// ���[�g�p�����[�^�̐ݒ�
void Render::SettingRootParameter()
{
    // �f�X�N���v�^�q�[�v�e�[�u��(�萔�p)�̐ݒ�
    descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);   // b0���W�X�^

    // �f�X�N���v�^�q�[�v�e�[�u��(�e�N�X�`���p)�̐ݒ�
    descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);   // t0���W�X�^

    // ���[�g�p�����[�^(�萔�p)�̐ݒ�
    rootParams[0].InitAsDescriptorTable(1, &descRangeCBV);

    // ���[�g�p�����[�^(�e�N�X�`���p)�̐ݒ�
    rootParams[1].InitAsDescriptorTable(1, &descRangeSRV);
}

// �e�N�X�`���T���v���[�̐ݒ�
void Render::SettingTextureSampler()
{
    samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
}

// ���[�g�V�O�l�`������
HRESULT Render::CreateRootSignature()
{
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> rootSigBlob;
    auto result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
                                              D3D_ROOT_SIGNATURE_VERSION_1_0,
                                              &rootSigBlob,
                                              &errorBlob);
    assert(SUCCEEDED(result));

    // ���[�g�V�O�l�`���̐���
    result = dx12.GetDevice()->CreateRootSignature(0,
                                                   rootSigBlob->GetBufferPointer(),
                                                   rootSigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(result));

    return result;
}

// �p�C�v���C������
HRESULT Render::CreateGraphicsPipeline()
{
    // ���_�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
    auto result = D3DCompileFromFile(L"BasicVS.hlsl",                                 // �V�F�[�_�t�@�C����
                                     nullptr,
                                     D3D_COMPILE_STANDARD_FILE_INCLUDE,               // �C���N���[�h�\�ɂ���
                                     "main",                                          // �G���g���[�|�C���g��
                                     "vs_5_0",                                        // �V�F�[�_�[���f���w��
                                     D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
                                     0,
                                     &vsBlob,
                                     &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlob����G���[���e��string�^�ɃR�s�[
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // �G���[���e���o�̓E�B���h�E�ɕ\��
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // �s�N�Z���V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
    result = D3DCompileFromFile(L"BasicPS.hlsl",                                 // �V�F�[�_�t�@�C����
                                nullptr,
                                D3D_COMPILE_STANDARD_FILE_INCLUDE,               // �C���N���[�h�\�ɂ���
                                "main",                                          // �G���g���[�|�C���g��
                                "ps_5_0",                                        // �V�F�[�_�[���f���w��
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
                                0,
                                &psBlob,
                                &errorBlob);
    assert(SUCCEEDED(result));
    if ( FAILED(result) )
    {
        // errorBlob����G���[���e��string�^�ɃR�s�[
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n(static_cast<char *>(errorBlob->GetBufferPointer()),
                    errorBlob->GetBufferSize(),
                    errstr.begin());
        errstr += "\n";
        // �G���[���e���o�̓E�B���h�E�ɕ\��
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // ���_���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {   // xyz���W
            "POSITION",                                 // �Z�}���e�B�b�N��
            0,                                          // �����Z�}���e�B�b�N�����������鎞�Ɏg���C���f�b�N�X
            DXGI_FORMAT_R32G32B32_FLOAT,                // �v�f���ƃr�b�g����\��
            0,                                          // ���̓X���b�g�C���f�b�N�X
            D3D12_APPEND_ALIGNED_ELEMENT,               // �f�[�^�̃I�t�Z�b�g�l
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // ���̓f�[�^���
            0                                           // �����x�ɕ`�悷��C���X�^���X��
        },
        {
            // �@���x�N�g��
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {   // uv���W
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    // �O���t�B�b�N�X�p�C�v���C���ݒ�
    // ���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    // �T���v���}�X�N�ƃ��X�^���C�Y�X�e�[�g�ݒ�
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;           // �W���ݒ�
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
    D3D12_RENDER_TARGET_BLEND_DESC &blendDesc = gpipeline.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // �W���ݒ�
    blendDesc.BlendEnable = true;                                   // �u�����h��L��
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;                    // ���Z
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;                      // �\�[�X�̒l��100%�g��
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;                    // �f�X�g�̒l��0%�g��
    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;                         // ���Z
    blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;                     // �\�[�X�̃A���t�@�l
    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;                // 1.0f - �\�[�X�̃A���t�@�l

    // ���_���C�A�E�g�̐ݒ�
    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);

    // �}�`�̌`���ݒ�
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // ���̑��̐ݒ�
    gpipeline.NumRenderTargets = 1;                       //�`��Ώۂ�1��
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255�w���RGBA
    gpipeline.SampleDesc.Count = 1;                       // 1�s�N�Z���ɂ�1��T���v�����O

    // �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
    gpipeline.pRootSignature = rootSignature.Get();

    // �f�v�X�X�e���V���X�e�[�g�̐ݒ�
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                // �[�x�l�t�H�[�}�b�g

    // �p�C�v���C���X�e�[�g�̐���
    result = dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
    assert(SUCCEEDED(result));

    return result;
}

// �`�揈��
void Render::BeginDraw()
{
    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�
    assert(pipelineState != nullptr);
    dx12.GetCmdList()->SetPipelineState(pipelineState.Get());
    dx12.GetCmdList()->SetGraphicsRootSignature(rootSignature.Get());
}