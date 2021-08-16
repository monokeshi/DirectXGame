#pragma once

#include <dinput.h>

#include <Windows.h>

enum MouseButton
{
    DIM_LEFT,     // 左ボタン = 0
    DIM_RIGHT,    // 右ボタン = 1
    DIM_CENTER    // 中央ボタン = 2
};

class InputMouse
{
private:
    DIMOUSESTATE currentMouse;
    DIMOUSESTATE prevMouse;
    
    // シングルトンのためにコンストラクタをprivateに
    // コピーと代入を禁止する
    InputMouse();
    InputMouse(const InputMouse &) = delete;
    void operator=(const InputMouse &) = delete;

public:
    ~InputMouse();

    // InputMouseのシングルトンインスタンスを得る
    static InputMouse *GetInstance();

    // マウス情報の更新
    void Update(IDirectInputDevice8 *devMouse);
    // 押されている状態か
    bool IsMousePress(int button);
    // 押した瞬間か
    bool IsMouseDown(int button);
    // 離した瞬間か
    bool IsMouseUp(int button);
};