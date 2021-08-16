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

// �}�E�X���̍X�V
void InputMouse::Update(IDirectInputDevice8 *devMouse)
{
    // �}�E�X���̎擾�J�n
    devMouse->Acquire();

    // 1�t���[���O�̓��͏�Ԏ擾
    prevMouse = currentMouse;

    // �ŐV�̓��͏��
    devMouse->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse);
}

// ������Ă����Ԃ�
bool InputMouse::IsMousePress(int button)
{
    return currentMouse.rgbButtons[button] & MOUSE_ON_VALUE;
}

// �������u�Ԃ�
bool InputMouse::IsMouseDown(int button)
{
    return !(prevMouse.rgbButtons[button] & MOUSE_ON_VALUE) &&
        currentMouse.rgbButtons[button] & MOUSE_ON_VALUE;
}

// �������u�Ԃ�
bool InputMouse::IsMouseUp(int button)
{
    return prevMouse.rgbButtons[button] & MOUSE_ON_VALUE &&
        !(currentMouse.rgbButtons[button] & MOUSE_ON_VALUE);
}
