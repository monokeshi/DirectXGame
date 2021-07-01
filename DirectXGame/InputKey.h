#pragma once

#include <dinput.h>

#include <Windows.h>

class InputKey
{
private:
    BYTE key[256];
    BYTE oldKey[256];
    int arraySize;

    // シングルトンのためにコンストラクタをprivateに
    // コピーと代入を禁止する
    InputKey();
    InputKey(const InputKey &) = delete;
    void operator=(const InputKey &) = delete;
public:
    // InputKeyのシングルトンインスタンスを得る
    static InputKey &GetInstance();

    // キーボード情報の更新
    void KeyInfoUpdate(IDirectInputDevice8W *devkeyboard);
    // 押されている状態か
    bool IsKeyPressState(int keyCode);
    // 離されている状態か
    bool IsKeyReleaseState(int keyCode);
    // 押した瞬間か
    bool IsKeyTrigger(int keyCode);
    // 離した瞬間か
    bool IsKeyRelease(int keyCode);
};