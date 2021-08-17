#include "Application.h"
#include "DirectX12Wrapper.h"
#include "Render.h"
#include "InputKey.h"
#include "InputMouse.h"
#include "Player.h"
#include "Camera.h"
#include "Define.h"
#include "Sound.h"
#include "DebugText.h"
#include "Graphic.h"
#include "Utility.h"

#include <DirectXMath.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>

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
    /*-------------------システムや描画-------------------*/
    // シード値設定
    srand(static_cast<unsigned int>(time(NULL)));

    // ウィンドウの生成
    CreateGameWindow();

    // DirectX12生成&初期化
    dx12.reset(new DirectX12Wrapper(w, hwnd));
    dx12->SetClearColor(0.0f, 0.25f, 0.5f, 1.0f);

    // Render生成&初期化
    render.reset(new Render(*dx12));

    // カメラの初期化
    camera.reset(new Camera({ 0.0f, 0.0f, -50.0f }));

    // ウィンドウ表示
    ShowWindow(hwnd, SW_SHOW);

    /*-------------------テクスチャ-------------------*/
    texture = new Texture(*dx12, *render, render->GetBasicDescHeap());

    // オブジェクト3D用
    texObj3DHandles.push_back(texture->CreateTexture({ 1.0f, 0.5f, 0.0f, 1.0f }));          // テクスチャ生成
    texObj3DHandles.push_back(texture->CreateTexture({ 1.0f, 0.0f, 0.0f, 1.0f }));          // テクスチャ生成
    texObj3DHandles.push_back(texture->CreateTexture({ 0.0f, 1.0f, 0.0f, 1.0f }));          // テクスチャ生成
    texObj3DHandles.push_back(texture->CreateTexture({ 0.0f, 0.0f, 1.0f, 1.0f }));          // テクスチャ生成
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture01.png"));   // テクスチャ読み込み
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture02.png"));   // テクスチャ読み込み
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture03.png"));   // テクスチャ読み込み

    // スプライト用
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture01.png"));    // テクスチャ読み込み
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture02.png"));    // テクスチャ読み込み
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture03.png"));    // テクスチャ読み込み

    // デバッグテキスト用
    texDebugTextHandle = texture->LoadSpriteTexture(L"Resources/Textures/debug_font.png");

    /*-------------------メッシュ-------------------*/
    meshs.push_back(new Mesh(MeshList::e_RECTANGLE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_CUBE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_TRIANGULAR_PYRAMID, *dx12));

    /*-------------------オブジェクト-------------------*/
    for ( int i = 0; i < 10; ++i )
    {
        XMMATRIX *matWorldParent = nullptr;
        if ( i > 0 )
        {
            matWorldParent = object3Ds[0]->GetMatWorld();
        }

        auto obj3d = new Object3D({ 0.0f, 0.0f, 10.0f * i },  // position
                                  { 1.0f, 1.0f, 1.0f },       // scale
                                  { 0.0f, 0.0f, 0.0f },       // rotation
                                  { 1.0f, 1.0f, 1.0f, 1.0f }, // color
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

    /*-------------------スプライト-------------------*/
    for ( int i = 0; i < 1; ++i )
    {
        auto sp = new Sprite({ WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 100 + i * 50, 0.0f },    // position
                             0.0f,                                              // rotation
                             { 1.0f, 1.0f, 1.0f, 1.0f },                        // color
                             { 100.0f, 100.0f },                                // size
                             nullptr,
                             *dx12,
                             *render,
                             *texture);
        if ( FAILED(sp->Initialize()) )
        {
            assert(0);
            return false;
        }

        sprites.push_back(sp);
    }

    Graphic::GetInstance()->Initialize(*dx12, *render, render->GetBasicDescHeap());

    /*-------------------デバッグテキスト-------------------*/
    DebugText::GetInstance()->Initialize(texDebugTextHandle, *dx12, *render, *texture);

    /*-------------------サウンド-------------------*/
    Sound::GetInstance()->Initialize();
    shotSound = Sound::GetInstance()->LoadSoundWave("Resources/Sounds/shot.wav");

    return true;
}

// ループ - 起動
void Application::Run()
{
    MSG msg{};

    // 視点角度
    float eyeAngle = 0.0f;

    // プレイヤー生成
    Player player(Player(*object3Ds[0], *camera));

    // 一時的なオブジェクトのテクスチャハンドルのインデックス
    std::vector<int> texIndex;
    for ( int i = 0; i < object3Ds.size(); ++i )
    {
        texIndex.push_back((rand() % (texObj3DHandles.size() - 0)) + 0);
    }

    // 入力
    auto inputKey = InputKey::GetInstance();
    auto inputMouse = InputMouse::GetInstance();

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
        if ( inputKey->IsKeyUp(DIK_ESCAPE) )
        {
            break;
        }

        /*-------------------更新処理-------------------*/
        // 入力デバイスの更新
        inputKey->Update(dx12->GetDevKeyboard());
        inputMouse->Update(dx12->GetDevMouse());

        // プレイヤーコントローラー
        player.Update();

        // カメラの更新処理
        camera->Update();

        // スプライト
        for ( int i = 0; i < sprites.size(); ++i )
        {
            sprites[i]->Update();
        }

        // オブジェクト
        for ( int i = 0; i < object3Ds.size(); ++i )
        {
            object3Ds[i]->Update();
        }

        // メッシュ
        for ( int i = 0; i < meshs.size(); ++i )
        {
            meshs[i]->Update();
        }

        // サウンド
        static auto sound = Sound::GetInstance();
        if ( inputKey->IsKeyDown(DIK_SPACE) &&
            !sound->CheckSoundPlay(shotSound) )
        {
            sound->PlaySoundWave(shotSound, PLAY_TYPE_LOOP);
        }

        if ( inputKey->IsKeyDown(DIK_M) )
        {
            sound->StopSoundWave(shotSound, STOP_TYPE_PAUSE);
        }

        static float vol = 0.50f;
        if ( inputKey->IsKeyDown(DIK_UP) )
        {

            vol += 0.1f;
        }
        if ( inputKey->IsKeyDown(DIK_DOWN) )
        {
            vol -= 0.1f;
        }
        Utility::Clamp(vol, MIN_VOLUME, MAX_VOLUME);
        sound->ChangeSoundVolume(shotSound, vol);

        //dx12.get()->Update();

        /*-------------------描画処理-------------------*/
        // 描画準備
        dx12->BeginDraw();

        // オブジェクト
        render->BeginDrawObj3D();
        for ( int i = 0; i < object3Ds.size(); ++i )
        {
            object3Ds[i]->Draw(texObj3DHandles[texIndex[i]],
                               meshs[2]->GetVbView(),
                               meshs[2]->GetIbView(),
                               meshs[2]->GetIndicesNum());
        }

        // スプライト
        render->BeginDrawSprite();
        for ( int i = 0; i < sprites.size(); ++i )
        {
            sprites[i]->Draw(texSpriteHandles[0]);
        }

        // デバッグテキスト
        static auto debugText = DebugText::GetInstance();
        debugText->PrintFormat(200, 300, 1.0f, "Hello World!");
        debugText->PrintFormat(200, 320, 1.0f, "0.123329");
        debugText->PrintFormat(200, 350, 1.0f, "vol = %.2f", vol);
        debugText->DrawAll();

        // 描画後処理
        dx12->EndDraw();
    }

    // GPUの処理が終わるのを待つ
    dx12->FlushGPU();
}

// 後処理
void Application::Terminate()
{
    // グラフィック
    Graphic::GetInstance()->Terminate();

    // サウンド
    Sound::GetInstance()->Terminate();

    // ウィンドウクラスを登録解除
    assert(w.lpszClassName != nullptr);
    UnregisterClass(w.lpszClassName, w.hInstance);
}

Application::~Application()
{
    for ( int i = static_cast<int>(sprites.size()) - 1; i >= 0; i-- )
    {
        delete sprites[i];
        sprites.erase(sprites.begin() + i);
    }

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