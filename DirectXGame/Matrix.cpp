#include "Matrix.h"
#include "Define.h"

using namespace DirectX;

Matrix::Matrix(XMFLOAT3 &position,
               XMFLOAT3 &scale,
               XMFLOAT3 &rotation,
               XMMATRIX &matView,
               XMMATRIX *matWorldParent,
               bool isPerspective):
    position(position), scale(scale), rotation(rotation), matView(matView), matWorldParent(matWorldParent)
{
    // �ˉe�ϊ��s��̐���
    CalcMatProjection(isPerspective);

    // �ˉe�ϊ��ȊO���ׂĂ̍s��̌v�Z���s��
    CalcAllMatrix();
}

Matrix::~Matrix()
{
}

// �ˉe�ϊ��̌v�Z���s��
void Matrix::CalcMatProjection(bool isPerspective)
{
    if ( !isPerspective )
    {
        // ���s���e
        matProjection = XMMatrixIdentity();
        matProjection *= XMMatrixOrthographicOffCenterLH(0.0f,
                                                         static_cast<float>(WINDOW_WIDTH),
                                                         static_cast<float>(WINDOW_HEIGHT),
                                                         0.0f,
                                                         0.1f,
                                                         1000.0f);
    }
    else
    {
        // �������e
        matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f),                         // �㉺��p60�x
                                                 static_cast<float>(WINDOW_WIDTH) /
                                                 static_cast<float>(WINDOW_HEIGHT), // �A�X�y�N�g��
                                                 0.1f,                                              // �O�[
                                                 1000.0f);                                          // ���[
    }
}

// �X�P�[�����O�s��̌v�Z���s��
void Matrix::CalcMatScale()
{
    matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
}

// ��]�s��̌v�Z���s��
void Matrix::CalcMatRotate()
{
    // 2D�̏ꍇ��Z���݂̂ł悢
    matRotate = XMMatrixIdentity();
    matRotate *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
    matRotate *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
    matRotate *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
}

// ���s�ړ��s��̌v�Z���s��
void Matrix::CalcMatTrans()
{
    matTrans = XMMatrixTranslation(position.x, position.y, position.z); // ���s�ړ��s����Čv�Z
}

// ���[���h�ϊ��̌v�Z���s��
void Matrix::CalcMatWorld()
{
    matWorld = XMMatrixIdentity();  // ���[���h�s��͖��t���[�����Z�b�g
    matWorld *= matScale;           // ���[���h�s��ɃX�P�[�����O�𔽉f
    matWorld *= matRotate;          // ���[���h�s��ɉ�]�𔽉f
    matWorld *= matTrans;           // ���[���h�s��ɕ��s�ړ��𔽉f

    // �e���[���h�s��null�ł͂Ȃ� ���̃I�u�W�F�N�g�͎q�ƂȂ�
    if ( matWorldParent != nullptr )
    {
        // �e�I�u�W�F�N�g�̃��[���h�s���������
        matWorld *= (*matWorldParent);
    }
}

// �ˉe�ϊ��ȊO���ׂĂ̍s��̌v�Z���s��
void Matrix::CalcAllMatrix()
{
    CalcMatScale();
    CalcMatRotate();
    CalcMatTrans();
    CalcMatWorld();
}

// �X�V����
void Matrix::Update()
{
    // �ˉe�ϊ��ȊO�S�Ă̍s��ϊ�
    CalcAllMatrix();
}