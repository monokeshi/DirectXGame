#include "Application.h"

#include <Windows.h>

#pragma warning(disable : 6001)
#pragma warning(disable : 26812)

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "xaudio2.lib")

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定

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