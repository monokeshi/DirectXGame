#include "InputKey.h"

#include <string.h>
#include <assert.h>

InputKey::InputKey()
{
    for ( int i = 0; i < _countof(key); i++ )
    {
        key[i] = 0;
        oldKey[i] = 0;
    }
}

InputKey::~InputKey()
{
}

InputKey *InputKey::GetInstance()
{
    static InputKey instance;
    return &instance;
}

// �L�[�{�[�h���̍X�V
void InputKey::Update(IDirectInputDevice8 *devKeyboard)
{
    // �L�[�{�[�h���̎擾�J�n
    devKeyboard->Acquire();

    // 1�t���[���O�̓��͏�Ԏ擾
    memcpy(oldKey, key, _countof(key)); // key�̓��e��oldkey�ɃR�s�[

    // �S�L�[�̓��͏�Ԏ擾
    devKeyboard->GetDeviceState(sizeof(key), key);
}

// ������Ă����Ԃ�
bool InputKey::IsKeyPress(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode];
}

// �������u�Ԃ�
bool InputKey::IsKeyDown(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode] && !oldKey[keyCode];
}

// �������u�Ԃ�
bool InputKey::IsKeyUp(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode] && oldKey[keyCode];
}