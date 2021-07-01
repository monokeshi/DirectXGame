#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMFLOAT3 eyePos)
{
    // �r���[�ϊ��s��̐���
    CreateMatrixView(eyePos);

    // �J�����ʒu���璍���_�܂ł̋��������߂�
    CalcLength();
}

Camera::~Camera()
{
}

// �r���[�ϊ��s��̐���
void Camera::CreateMatrixView(XMFLOAT3 eyePos)
{
    eye = XMFLOAT3(eyePos.x, eyePos.y, eyePos.z);   // ���_���W
    target = XMFLOAT3(0.0f, 0.0f, 0.0f);            // �����_���W
    up = XMFLOAT3(0.0f, 1.0f, 0.0f);                // ������x�N�g��
}

// �J�����ʒu���璍���_�܂ł̋��������߂�
void Camera::CalcLength()
{
    float lengthX = fabsf(fabsf(eye.x) - fabsf(target.x));
    float lengthY = fabsf(fabsf(eye.y) - fabsf(target.y));
    float lengthZ = fabsf(fabsf(eye.z) - fabsf(target.z));
    length = sqrtf(lengthX * lengthX + lengthY * lengthY + lengthZ * lengthZ);
}

// �r���[�ϊ��̌v�Z���s��
void Camera::CalcMatView()
{
    matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

// �X�V����
void Camera::Update()
{
    CalcMatView();
}

// �ړ�
void Camera::MoveX(float move)
{
    eye.x += move;
}

void Camera::MoveY(float move)
{
    eye.y += move;
}

void Camera::MoveZ(float move)
{
    eye.z += move;
}

// ��]
void Camera::RotateX(float angle)
{
    eye.x = -length * sinf(angle);
}

void Camera::RotateY(float angle)
{
    eye.y = -length * sinf(angle);
}

void Camera::RotateZ(float angle)
{
    eye.z = -length * cosf(angle);
}