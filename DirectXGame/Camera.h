#pragma once

#include <DirectXMath.h>

class Camera
{
private:
    DirectX::XMFLOAT3 eye;           // ���_���W
    DirectX::XMFLOAT3 target;        // �����_���W
    DirectX::XMFLOAT3 up;            // ������x�N�g��
   
    DirectX::XMMATRIX matView;       // �r���[�ϊ��s��

    float length;           // �J�����Ɛ}�`�̋���

    // �r���[�ϊ��s��̐���
    void CreateMatrixView(DirectX::XMFLOAT3 eyePos);

    // �J�����ʒu���璍���_�܂ł̋��������߂�
    void CalcLength();

    // �r���[�ϊ��̌v�Z���s��
    void CalcMatView();

public:
    Camera(DirectX::XMFLOAT3 eyePos);
    ~Camera();

    // �X�V����
    void Update();

    // �ړ�
    void MoveX(float move);
    void MoveY(float move);
    void MoveZ(float move);

    // ��]
    void RotateBaseX(float angle);  // X���
    void RotateBaseZ(float angle);  // Y���
    void RotateBaseY(float angle);  // Z���

    DirectX::XMMATRIX *GetMatView()
    {
        return &matView;
    }
};