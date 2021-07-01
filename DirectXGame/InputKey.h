#pragma once

#include <dinput.h>

#include <Windows.h>

class InputKey
{
private:
    BYTE key[256];
    BYTE oldKey[256];
    int arraySize;

    // �V���O���g���̂��߂ɃR���X�g���N�^��private��
    // �R�s�[�Ƒ�����֎~����
    InputKey();
    InputKey(const InputKey &) = delete;
    void operator=(const InputKey &) = delete;
public:
    // InputKey�̃V���O���g���C���X�^���X�𓾂�
    static InputKey &GetInstance();

    // �L�[�{�[�h���̍X�V
    void KeyInfoUpdate(IDirectInputDevice8W *devkeyboard);
    // ������Ă����Ԃ�
    bool IsKeyPressState(int keyCode);
    // ������Ă����Ԃ�
    bool IsKeyReleaseState(int keyCode);
    // �������u�Ԃ�
    bool IsKeyTrigger(int keyCode);
    // �������u�Ԃ�
    bool IsKeyRelease(int keyCode);
};