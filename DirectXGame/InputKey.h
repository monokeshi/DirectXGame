#pragma once

#include <dinput.h>

#include <Windows.h>

class InputKey
{
private:
    BYTE key[256];
    BYTE oldKey[256];

    // �V���O���g���̂��߂ɃR���X�g���N�^��private��
    // �R�s�[�Ƒ�����֎~����
    InputKey();
    InputKey(const InputKey &) = delete;
    void operator=(const InputKey &) = delete;

public:
    ~InputKey();

    // InputKey�̃V���O���g���C���X�^���X�𓾂�
    static InputKey *GetInstance();

    // �L�[�{�[�h���̍X�V
    void Update(IDirectInputDevice8 *devKeyboard);
    // ������Ă����Ԃ�
    bool IsKeyPress(int keyCode);
    // �������u�Ԃ�
    bool IsKeyDown(int keyCode);
    // �������u�Ԃ�
    bool IsKeyUp(int keyCode);
};