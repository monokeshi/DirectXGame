#pragma once

#include <dinput.h>

#include <Windows.h>

enum MouseButton
{
    DIM_LEFT,     // ���{�^�� = 0
    DIM_RIGHT,    // �E�{�^�� = 1
    DIM_CENTER    // �����{�^�� = 2
};

class InputMouse
{
private:
    DIMOUSESTATE currentMouse;
    DIMOUSESTATE prevMouse;
    
    // �V���O���g���̂��߂ɃR���X�g���N�^��private��
    // �R�s�[�Ƒ�����֎~����
    InputMouse();
    InputMouse(const InputMouse &) = delete;
    void operator=(const InputMouse &) = delete;

public:
    ~InputMouse();

    // InputMouse�̃V���O���g���C���X�^���X�𓾂�
    static InputMouse *GetInstance();

    // �}�E�X���̍X�V
    void Update(IDirectInputDevice8 *devMouse);
    // ������Ă����Ԃ�
    bool IsMousePress(int button);
    // �������u�Ԃ�
    bool IsMouseDown(int button);
    // �������u�Ԃ�
    bool IsMouseUp(int button);
};