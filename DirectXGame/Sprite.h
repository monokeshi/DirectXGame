#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <vector>

class DirectX12Wrapper;
class Render;

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

    DirectX::XMFLOAT3 position; // ���W
    float rotation;             // ��]
    DirectX::XMMATRIX matWorld; // ���[���h�s��
    DirectX::XMFLOAT4 color;    // �F

    std::vector<Vertex> vertices;                       // ���_�f�[�^
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffer;  // ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW vbView{};                  // ���_�o�b�t�@�r���[
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer; // �萔�o�b�t�@

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
           DirectX12Wrapper &dx12,
           Render &render);
    ~Sprite();

    // ����������
    HRESULT Initialize();

    // �X�V����
    void Update();

    // �`�揈��
    void Draw(const int &textureNum);
};