#include "Application.h"

#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0800 // DirectInput�̃o�[�W�����w��

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    auto &app = Application::GetInstance();
    if ( !app.Initialize() )
    {
        return -1;
    }

    app.Run();
    app.Terminate();

    return 0;
}