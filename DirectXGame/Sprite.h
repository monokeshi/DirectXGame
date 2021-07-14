#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <vector>

class DirectX12Wrapper;
class Render;
class Texture;

class Sprite
{
private:
    struct  Vertex
    {
        DirectX::XMFLOAT3 pos;  // xyz���W
        DirectX::XMFLOAT2 uv;   // uv���W
    };

    struct ConstBufferData
    {
        DirectX::XMFLOAT4 color; // �F(RGBA)
        DirectX::XMMATRIX mat;   // 3D�ϊ��s��
    };

    DirectX12Wrapper &dx12;
    Render &render;
    Texture &texture;

    DirectX::XMFLOAT3 position;     // ���W
    float rotation;                 // ��]
    DirectX::XMMATRIX matWorld;     // ���[���h�s��
    DirectX::XMFLOAT4 color;        // �F
    DirectX::XMFLOAT2 anchorPoint;  // �A���J�[�|�C���g
    bool isFlipX;                   // ���E���]
    bool isFlipY;                   // �㉺���]

    DirectX::XMMATRIX *matWorldParent;   // �e�̃��[���h�s��

    std::vector<Vertex> vertices;                       // ���_�f�[�^
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffer;  // ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW vbView{};                  // ���_�o�b�t�@�r���[
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // �萔�o�b�t�@

    bool isSetTexResolutionOnlyOnce;    // Draw�֐��Ăяo������SetSize�֐�����x�����Ăяo�����߂̃t���O
    //bool dirtyFlag;                     // �萔�o�b�t�@�p�̍��ڂŕύX���������ꍇ�̂�true

    // �s��̍X�V����
    void UpdateMatrix();

    // ���_����
    void CreateVertex();

    // ���_�o�b�t�@�̐���
    HRESULT CreateVertBuffer();

    // ���_�o�b�t�@�ւ̓]��
    HRESULT TransferVertBuffer();

    // �萔�o�b�t�@�̐���
    HRESULT CreateConstBuffer();

    // �萔�o�b�t�@�ւ̃f�[�^�]��
    HRESULT TransferConstBuffer();

public:
    Sprite(DirectX::XMFLOAT3 position,
           float rotation,
           DirectX::XMFLOAT4 color,
           DirectX::XMMATRIX *matWorldParent,
           DirectX12Wrapper &dx12,
           Render &render,
           Texture &texture);
    ~Sprite();

    // ����������
    HRESULT Initialize();

    // �X�V����
    void Update();

    // �`�揈��
    void Draw(const int &texIndex, bool isFlipX = false, bool isFlipY = false, bool isInvisible = false, bool isSetTexResolution = true);

    // �T�C�Y��ݒ�
    void SetSize(DirectX::XMFLOAT2 size);

    DirectX::XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};