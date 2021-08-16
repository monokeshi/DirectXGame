#pragma once

#include <dinput.h>

#include <Windows.h>

class InputKey
{
private:
    BYTE key[256];
    BYTE oldKey[256];

    // シングルトンのためにコンストラクタをprivateに
    // コピーと代入を禁止する
    InputKey();
    InputKey(const InputKey &) = delete;
    void operator=(const InputKey &) = delete;

public:
    ~InputKey();

    // InputKeyのシングルトンインスタンスを得る
    static InputKey *GetInstance();

    // キーボード情報の更新
    void Update(IDirectInputDevice8 *devKeyboard);
    // 押されている状態か
    bool IsKeyPress(int keyCode);
    // 押した瞬間か
    bool IsKeyDown(int keyCode);
    // 離した瞬間か
    bool IsKeyUp(int keyCode);
};