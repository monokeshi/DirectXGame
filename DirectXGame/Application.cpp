#include "Application.h"
#include "DirectX12Wrapper.h"
#include "Render.h"
#include "InputKey.h"
#include "PlayerController.h"
#include "Camera.h"
#include "Define.h"

#include <DirectXMath.h>

#include <assert.h>

using namespace DirectX;

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch ( msg )
    {
        case WM_DESTROY:        // ウィンドウが破棄された
            PostQuitMessage(0); // OSに対して、アプリの終了を伝える
            return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

// ゲーム用ウィンドウの生成
void Application::CreateGameWindow()
{
    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // ウィンドウプロシージャを設定
    w.lpszClassName = GAME_TITLE;                       // ウィンドウクラス名
    w.hInstance = GetModuleHandle(nullptr);             // ウィンドウハンドル
    w.hCursor = LoadCursor(NULL, IDC_ARROW);            // カーソル指定

     // ウィンドウクラスをOSに登録
    RegisterClassEx(&w);

    // ウィンドウサイズ{ X座標, Y座標, 横幅, 縦幅 }
    RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // 自動でサイズ補正

    // ウィンドウオブジェクトの生成
    hwnd = CreateWindow(w.lpszClassName,      // クラス名
                        GAME_TITLE,           // タイトルバーの文字
                        WS_OVERLAPPEDWINDOW,  // 標準的なウィンドウスタイル
                        CW_USEDEFAULT,        // 表示X座標（OSに任せる）
                        CW_USEDEFAULT,        // 表示Y座標（OSに任せる）
                        wrc.right - wrc.left, // ウィンドウ横幅
                        wrc.bottom - wrc.top, // ウィンドウ縦幅
                        nullptr,              // 親ウィンドウハンドル
                        nullptr,              // メニューハンドル
                        w.hInstance,          // 呼び出しアプリケーションハンドル
                        nullptr);             // オプション


}

Application::Application()
{
}

Application &Application::GetInstance()
{
    static Application instance;
    return instance;
}

// 初期化
bool Application::Initialize()
{
    // ウィンドウの生成
    CreateGameWindow();

    // DirectX12生成&初期化
    dx12.reset(new DirectX12Wrapper(w, hwnd));

    // Render生成&初期化
    render.reset(new Render(*dx12));

    // カメラの初期化
    camera.reset(new Camera(XMFLOAT3(0.0f, 0.0f, -50.0f)));

    // ウィンドウ表示
    ShowWindow(hwnd, SW_SHOW);

    // オブジェクト
    for ( int i = 0; i < 10; i++ )
    {
        XMMATRIX *matWorldParent = nullptr;
        if ( i > 0 )
        {
            matWorldParent = object3Ds[0]->GetMatWorld();
        }

        auto obj3d = new Object3D(XMFLOAT3(0.0f, 0.0f, 10.0f * i),  // position
                                  XMFLOAT3(1.0f, 1.0f, 1.0f),       // scale
                                  XMFLOAT3(0.0f, 0.0f, 0.0f),       // rotation
                                  *camera->GetMatView(),
                                  matWorldParent,                   // 親オブジェクトのmatWorld 自身が親オブジェクトの場合はnull
                                  *dx12,
                                  *render,
                                  i,
                                  true);                            // 透視投影(true)or平行投影(false)
        if ( FAILED(obj3d->Initialize()) )
        {
            assert(0);
            return false;
        }

        object3Ds.push_back(obj3d);
    }

    // テクスチャ
    texture = new Texture(*dx12, render->GetBasicDescHeap());
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture01.png")); // テクスチャ読み込み
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture02.png")); // テクスチャ読み込み
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture03.png")); // テクスチャ読み込み

    // メッシュ
    meshs.push_back(new Mesh(MeshList::e_RECTANGLE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_CUBE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_TRIANGULAR_PYRAMID, *dx12));

    return true;
}

// ループ - 起動
void Application::Run()
{
    MSG msg{};

    // キー入力生成
    auto &ik = InputKey::GetInstance();

    // 視点角度
    float eyeAngle = 0.0f;

    // プレイヤーコントローラー生成
    PlayerController plctrl(PlayerController(*object3Ds[0], ik, *camera));

    // ゲームループ
    while ( true )
    {
        // ウィンドウからのメッセージ
        if ( PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // ゲームループを抜ける
        if ( msg.message == WM_QUIT )
        {
            break;
        }

        // ゲーム終了
        if ( ik.IsKeyRelease(DIK_ESCAPE) )
        {
            break;
        }

        /*-------------------更新処理-------------------*/
        // キーボード情報の更新
        ik.KeyInfoUpdate(dx12->GetDevkeyboard());

        // プレイヤーコントローラー
        plctrl.Update();

        // カメラの更新処理
        camera->Update();

        // オブジェクト
        for ( int i = 0; i < object3Ds.size(); i++ )
        {
            object3Ds[i]->Update();
        }

        // メッシュ
        for ( int i = 0; i < meshs.size(); i++ )
        {
            meshs[i]->Update();
        }

        //dx12.get()->Update();

        /*-------------------描画処理-------------------*/
        // 描画準備
        dx12->BeginDraw();

        // パイプラインステートやルートシグネチャの設定
        render->BeginDraw();

        // オブジェクト
        for ( int i = 0; i < object3Ds.size(); i++ )
        {
            int j = 1;
            if ( i >= object3Ds.size() / 2 )
            {
                j = 0;
            }
            object3Ds[i]->Draw(textureHandles[j + 1],
                               meshs[j + 0]->GetVbView(),
                               meshs[j + 0]->GetIbView(),
                               meshs[j + 0]->GetIndicesNum());
        }

        // 描画後処理
        dx12->EndDraw();
    }

    // GPUの処理が終わるのを待つ
    dx12->FlushGPU();
}

// 後処理
void Application::Terminate()
{
    // ウィンドウクラスを登録解除
    assert(w.lpszClassName != nullptr);
    UnregisterClass(w.lpszClassName, w.hInstance);
}

Application::~Application()
{
    for ( int i = static_cast<int>(object3Ds.size()) - 1; i >= 0; i-- )
    {
        delete object3Ds[i];
        object3Ds.erase(object3Ds.begin() + i);
    }

    for ( int i = static_cast<int>(meshs.size()) - 1; i >= 0; i-- )
    {
        delete meshs[i];
        meshs.erase(meshs.begin() + i);
    }

    delete texture;
}