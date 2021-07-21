#pragma once

#include <DirectXMath.h>

class Matrix
{
private:
    DirectX::XMFLOAT3 &position;         // �|�W�V����
    DirectX::XMFLOAT3 &scale;            // �X�P�[�����O�{��
    DirectX::XMFLOAT3 &rotation;         // ��]�p
    DirectX::XMMATRIX &matView;          // �r���[�ϊ��s��
    DirectX::XMMATRIX *matWorldParent;   // �e�̃��[���h�s��

    DirectX::XMMATRIX matProjection;     // �ˉe�ϊ��s��
    DirectX::XMMATRIX matWorld;          // ���[���h�ϊ��s��
    DirectX::XMMATRIX matScale;          // �X�P�[�����O�s��
    DirectX::XMMATRIX matRotate;         // ��]�s��
    DirectX::XMMATRIX matTrans;          // ���s�ړ��s��

    // �ˉe�ϊ��s��̌v�Z���s��
    void CalcMatProjection(bool isPerspective); // �������e�̏ꍇtrue

    // �X�P�[�����O�s��̌v�Z���s��
    void CalcMatScale();

    // ��]�s��̌v�Z���s��
    void CalcMatRotate();

    // ���s�ړ��s��̌v�Z���s��
    void CalcMatTrans();

    // ���[���h�ϊ��s��̌v�Z���s��
    void CalcMatWorld();

    // �ˉe�ϊ��ȊO���ׂĂ̍s��̌v�Z���s��
    void CalcAllMatrix();

public:
    Matrix(DirectX::XMFLOAT3 &position,
           DirectX::XMFLOAT3 &scale,
           DirectX::XMFLOAT3 &rotation,
           DirectX::XMMATRIX &matView,
           DirectX::XMMATRIX *matWorldParent,
           bool isPerspective);
    ~Matrix();

    // �X�V����
    void Update();

    // �s��̍���
    const DirectX::XMMATRIX GetMat() const
    {
        return matWorld * matView * matProjection;
    }

    DirectX::XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};