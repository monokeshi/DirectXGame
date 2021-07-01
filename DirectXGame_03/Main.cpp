#include "Application.h"

#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
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