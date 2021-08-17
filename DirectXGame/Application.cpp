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

// �E�B���h�E�v���V�[�W��
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch ( msg )
    {
        case WM_DESTROY:        // �E�B���h�E���j�����ꂽ
            PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
            return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam); // �W���̏������s��
}

// �Q�[���p�E�B���h�E�̐���
void Application::CreateGameWindow()
{
    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // �E�B���h�E�v���V�[�W����ݒ�
    w.lpszClassName = GAME_TITLE;                       // �E�B���h�E�N���X��
    w.hInstance = GetModuleHandle(nullptr);             // �E�B���h�E�n���h��
    w.hCursor = LoadCursor(NULL, IDC_ARROW);            // �J�[�\���w��

     // �E�B���h�E�N���X��OS�ɓo�^
    RegisterClassEx(&w);

    // �E�B���h�E�T�C�Y{ X���W, Y���W, ����, �c�� }
    RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �����ŃT�C�Y�␳

    // �E�B���h�E�I�u�W�F�N�g�̐���
    hwnd = CreateWindow(w.lpszClassName,      // �N���X��
                        GAME_TITLE,           // �^�C�g���o�[�̕���
                        WS_OVERLAPPEDWINDOW,  // �W���I�ȃE�B���h�E�X�^�C��
                        CW_USEDEFAULT,        // �\��X���W�iOS�ɔC����j
                        CW_USEDEFAULT,        // �\��Y���W�iOS�ɔC����j
                        wrc.right - wrc.left, // �E�B���h�E����
                        wrc.bottom - wrc.top, // �E�B���h�E�c��
                        nullptr,              // �e�E�B���h�E�n���h��
                        nullptr,              // ���j���[�n���h��
                        w.hInstance,          // �Ăяo���A�v���P�[�V�����n���h��
                        nullptr);             // �I�v�V����


}

Application::Application()
{
}

Application &Application::GetInstance()
{
    static Application instance;
    return instance;
}

// ������
bool Application::Initialize()
{
    /*-------------------�V�X�e����`��-------------------*/
    // �V�[�h�l�ݒ�
    srand(static_cast<unsigned int>(time(NULL)));

    // �E�B���h�E�̐���
    CreateGameWindow();

    // DirectX12����&������
    dx12.reset(new DirectX12Wrapper(w, hwnd));
    dx12->SetClearColor(0.0f, 0.25f, 0.5f, 1.0f);

    // Render����&������
    render.reset(new Render(*dx12));

    // �J�����̏�����
    camera.reset(new Camera({ 0.0f, 0.0f, -50.0f }));

    // �E�B���h�E�\��
    ShowWindow(hwnd, SW_SHOW);

    /*-------------------�e�N�X�`��-------------------*/
    texture = new Texture(*dx12, *render, render->GetBasicDescHeap());

    // �I�u�W�F�N�g3D�p
    texObj3DHandles.push_back(texture->CreateTexture({ 1.0f, 0.5f, 0.0f, 1.0f }));          // �e�N�X�`������
    texObj3DHandles.push_back(texture->CreateTexture({ 1.0f, 0.0f, 0.0f, 1.0f }));          // �e�N�X�`������
    texObj3DHandles.push_back(texture->CreateTexture({ 0.0f, 1.0f, 0.0f, 1.0f }));          // �e�N�X�`������
    texObj3DHandles.push_back(texture->CreateTexture({ 0.0f, 0.0f, 1.0f, 1.0f }));          // �e�N�X�`������
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture01.png"));   // �e�N�X�`���ǂݍ���
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture02.png"));   // �e�N�X�`���ǂݍ���
    texObj3DHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture03.png"));   // �e�N�X�`���ǂݍ���

    // �X�v���C�g�p
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture01.png"));    // �e�N�X�`���ǂݍ���
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture02.png"));    // �e�N�X�`���ǂݍ���
    texSpriteHandles.push_back(texture->LoadSpriteTexture(L"Resources/Textures/texture03.png"));    // �e�N�X�`���ǂݍ���

    // �f�o�b�O�e�L�X�g�p
    texDebugTextHandle = texture->LoadSpriteTexture(L"Resources/Textures/debug_font.png");

    /*-------------------���b�V��-------------------*/
    meshs.push_back(new Mesh(MeshList::e_RECTANGLE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_CUBE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_TRIANGULAR_PYRAMID, *dx12));

    /*-------------------�I�u�W�F�N�g-------------------*/
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
                                  matWorldParent,                   // �e�I�u�W�F�N�g��matWorld ���g���e�I�u�W�F�N�g�̏ꍇ��null
                                  *dx12,
                                  *render,
                                  i,
                                  true);                            // �������e(true)or���s���e(false)
        if ( FAILED(obj3d->Initialize()) )
        {
            assert(0);
            return false;
        }

        object3Ds.push_back(obj3d);
    }

    /*-------------------�X�v���C�g-------------------*/
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

    /*-------------------�f�o�b�O�e�L�X�g-------------------*/
    DebugText::GetInstance()->Initialize(texDebugTextHandle, *dx12, *render, *texture);

    /*-------------------�T�E���h-------------------*/
    Sound::GetInstance()->Initialize();
    shotSound = Sound::GetInstance()->LoadSoundWave("Resources/Sounds/shot.wav");

    return true;
}

// ���[�v - �N��
void Application::Run()
{
    MSG msg{};

    // ���_�p�x
    float eyeAngle = 0.0f;

    // �v���C���[����
    Player player(Player(*object3Ds[0], *camera));

    // �ꎞ�I�ȃI�u�W�F�N�g�̃e�N�X�`���n���h���̃C���f�b�N�X
    std::vector<int> texIndex;
    for ( int i = 0; i < object3Ds.size(); ++i )
    {
        texIndex.push_back((rand() % (texObj3DHandles.size() - 0)) + 0);
    }

    // ����
    auto inputKey = InputKey::GetInstance();
    auto inputMouse = InputMouse::GetInstance();

    // �Q�[�����[�v
    while ( true )
    {
        // �E�B���h�E����̃��b�Z�[�W
        if ( PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // �Q�[�����[�v�𔲂���
        if ( msg.message == WM_QUIT )
        {
            break;
        }

        // �Q�[���I��
        if ( inputKey->IsKeyUp(DIK_ESCAPE) )
        {
            break;
        }

        /*-------------------�X�V����-------------------*/
        // ���̓f�o�C�X�̍X�V
        inputKey->Update(dx12->GetDevKeyboard());
        inputMouse->Update(dx12->GetDevMouse());

        // �v���C���[�R���g���[���[
        player.Update();

        // �J�����̍X�V����
        camera->Update();

        // �X�v���C�g
        for ( int i = 0; i < sprites.size(); ++i )
        {
            sprites[i]->Update();
        }

        // �I�u�W�F�N�g
        for ( int i = 0; i < object3Ds.size(); ++i )
        {
            object3Ds[i]->Update();
        }

        // ���b�V��
        for ( int i = 0; i < meshs.size(); ++i )
        {
            meshs[i]->Update();
        }

        // �T�E���h
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

        /*-------------------�`�揈��-------------------*/
        // �`�揀��
        dx12->BeginDraw();

        // �I�u�W�F�N�g
        render->BeginDrawObj3D();
        for ( int i = 0; i < object3Ds.size(); ++i )
        {
            object3Ds[i]->Draw(texObj3DHandles[texIndex[i]],
                               meshs[2]->GetVbView(),
                               meshs[2]->GetIbView(),
                               meshs[2]->GetIndicesNum());
        }

        // �X�v���C�g
        render->BeginDrawSprite();
        for ( int i = 0; i < sprites.size(); ++i )
        {
            sprites[i]->Draw(texSpriteHandles[0]);
        }

        // �f�o�b�O�e�L�X�g
        static auto debugText = DebugText::GetInstance();
        debugText->PrintFormat(200, 300, 1.0f, "Hello World!");
        debugText->PrintFormat(200, 320, 1.0f, "0.123329");
        debugText->PrintFormat(200, 350, 1.0f, "vol = %.2f", vol);
        debugText->DrawAll();

        // �`��㏈��
        dx12->EndDraw();
    }

    // GPU�̏������I���̂�҂�
    dx12->FlushGPU();
}

// �㏈��
void Application::Terminate()
{
    // �O���t�B�b�N
    Graphic::GetInstance()->Terminate();

    // �T�E���h
    Sound::GetInstance()->Terminate();

    // �E�B���h�E�N���X��o�^����
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