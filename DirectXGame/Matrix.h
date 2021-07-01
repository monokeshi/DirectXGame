#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Matrix
{
private:
    XMFLOAT3 &position;         // �|�W�V����
    XMFLOAT3 &scale;            // �X�P�[�����O�{��
    XMFLOAT3 &rotation;         // ��]�p
    XMMATRIX &matView;          // �r���[�ϊ��s��
    XMMATRIX *matWorldParent;   // �e�̃��[���h�s��

    XMMATRIX matProjection;     // �ˉe�ϊ��s��
    XMMATRIX matWorld;          // ���[���h�ϊ��s��
    XMMATRIX matScale;          // �X�P�[�����O�s��
    XMMATRIX matRotate;         // ��]�s��
    XMMATRIX matTrans;          // ���s�ړ��s��

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
    Matrix(XMFLOAT3 &position,
           XMFLOAT3 &scale,
           XMFLOAT3 &rotation,
           XMMATRIX &matView,
           XMMATRIX *matWorldParent,
           bool isPerspective);
    ~Matrix();

    // �X�V����
    void Update();

    // �s��̍���
    XMMATRIX GetMat()
    {
        return matWorld * matView * matProjection;
    }

    XMMATRIX *GetMatWorld()
    {
        return &matWorld;
    }
};