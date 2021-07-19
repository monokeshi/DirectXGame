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

// キーボード情報の更新
void InputKey::KeyInfoUpdate(IDirectInputDevice8W *devkeyboard)
{
    // キーボード情報の取得開始
    devkeyboard->Acquire();

    // 1フレーム前の入力状態取得
    memcpy(oldKey, key, arraySize); // keyの内容をoldkeyにコピー

    // 全キーの入力状態取得
    devkeyboard->GetDeviceState(sizeof(BYTE) * arraySize, key);
}

// 押されている状態か
bool InputKey::IsKeyPressState(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode];
}

// 離されている状態か
bool InputKey::IsKeyReleaseState(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode];
}

// 押した瞬間か
bool InputKey::IsKeyTrigger(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode] && !oldKey[keyCode];
}

// 離した瞬間か
bool InputKey::IsKeyRelease(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode] && oldKey[keyCode];
}