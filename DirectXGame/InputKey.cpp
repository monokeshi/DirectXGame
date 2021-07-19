#include "InputKey.h"

#include <string.h>
#include <assert.h>

InputKey::InputKey()
{
    arraySize = _countof(key);
    for ( int i = 0; i < arraySize; i++ )
    {
        key[i] = 0;
        oldKey[i] = 0;
    }
}

InputKey &InputKey::GetInstance()
{
    static InputKey instance;
    return instance;
}

// �L�[�{�[�h���̍X�V
void InputKey::KeyInfoUpdate(IDirectInputDevice8W *devkeyboard)
{
    // �L�[�{�[�h���̎擾�J�n
    devkeyboard->Acquire();

    // 1�t���[���O�̓��͏�Ԏ擾
    memcpy(oldKey, key, arraySize); // key�̓��e��oldkey�ɃR�s�[

    // �S�L�[�̓��͏�Ԏ擾
    devkeyboard->GetDeviceState(sizeof(BYTE) * arraySize, key);
}

// ������Ă����Ԃ�
bool InputKey::IsKeyPressState(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode];
}

// ������Ă����Ԃ�
bool InputKey::IsKeyReleaseState(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode];
}

// �������u�Ԃ�
bool InputKey::IsKeyTrigger(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode] && !oldKey[keyCode];
}

// �������u�Ԃ�
bool InputKey::IsKeyRelease(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode] && oldKey[keyCode];
}