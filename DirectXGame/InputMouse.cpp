#include "InputMouse.h"

#include <assert.h>

const int MOUSE_ON_VALUE = 0x80;

InputMouse::InputMouse()
{
    currentMouse = { };
    prevMouse = { };
}

InputMouse::~InputMouse()
{
}

InputMouse *InputMouse::GetInstance()
{
    static InputMouse instance;
    return &instance;
}

// マウス情報の更新
void InputMouse::Update(IDirectInputDevice8 *devMouse)
{
    // マウス情報の取得開始
    devMouse->Acquire();

    // 1フレーム前の入力状態取得
    prevMouse = currentMouse;

    // 最新の入力情報
    devMouse->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse);
}

// 押されている状態か
bool InputMouse::IsMousePress(int button)
{
    return currentMouse.rgbButtons[button] & MOUSE_ON_VALUE;
}

// 押した瞬間か
bool InputMouse::IsMouseDown(int button)
{
    return !(prevMouse.rgbButtons[button] & MOUSE_ON_VALUE) &&
        currentMouse.rgbButtons[button] & MOUSE_ON_VALUE;
}

// 離した瞬間か
bool InputMouse::IsMouseUp(int button)
{
    return prevMouse.rgbButtons[button] & MOUSE_ON_VALUE &&
        !(currentMouse.rgbButtons[button] & MOUSE_ON_VALUE);
}
