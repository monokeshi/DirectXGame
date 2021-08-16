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

// キーボード情報の更新
void InputKey::Update(IDirectInputDevice8 *devKeyboard)
{
    // キーボード情報の取得開始
    devKeyboard->Acquire();

    // 1フレーム前の入力状態取得
    memcpy(oldKey, key, _countof(key)); // keyの内容をoldkeyにコピー

    // 全キーの入力状態取得
    devKeyboard->GetDeviceState(sizeof(key), key);
}

// 押されている状態か
bool InputKey::IsKeyPress(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode];
}

// 押した瞬間か
bool InputKey::IsKeyDown(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return key[keyCode] && !oldKey[keyCode];
}

// 離した瞬間か
bool InputKey::IsKeyUp(int keyCode)
{
    assert(0 <= keyCode && keyCode < 255);
    return !key[keyCode] && oldKey[keyCode];
}