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
    // �E�B���h�E�̐���
    CreateGameWindow();

    // DirectX12����&������
    dx12.reset(new DirectX12Wrapper(w, hwnd));

    // Render����&������
    render.reset(new Render(*dx12));

    // �J�����̏�����
    camera.reset(new Camera(XMFLOAT3(0.0f, 0.0f, -50.0f)));

    // �E�B���h�E�\��
    ShowWindow(hwnd, SW_SHOW);

    // �I�u�W�F�N�g
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

    // �e�N�X�`��
    texture = new Texture(*dx12, render->GetBasicDescHeap());
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture01.png")); // �e�N�X�`���ǂݍ���
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture02.png")); // �e�N�X�`���ǂݍ���
    textureHandles.push_back(texture->LoadTexture(L"Resources/Textures/texture03.png")); // �e�N�X�`���ǂݍ���

    // ���b�V��
    meshs.push_back(new Mesh(MeshList::e_RECTANGLE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_CUBE, *dx12));
    meshs.push_back(new Mesh(MeshList::e_TRIANGULAR_PYRAMID, *dx12));

    return true;
}

// ���[�v - �N��
void Application::Run()
{
    MSG msg{};

    // �L�[���͐���
    auto &ik = InputKey::GetInstance();

    // ���_�p�x
    float eyeAngle = 0.0f;

    // �v���C���[�R���g���[���[����
    PlayerController plctrl(PlayerController(*object3Ds[0], ik, *camera));

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
        if ( ik.IsKeyRelease(DIK_ESCAPE) )
        {
            break;
        }

        /*-------------------�X�V����-------------------*/
        // �L�[�{�[�h���̍X�V
        ik.KeyInfoUpdate(dx12->GetDevkeyboard());

        // �v���C���[�R���g���[���[
        plctrl.Update();

        // �J�����̍X�V����
        camera->Update();

        // �I�u�W�F�N�g
        for ( int i = 0; i < object3Ds.size(); i++ )
        {
            object3Ds[i]->Update();
        }

        // ���b�V��
        for ( int i = 0; i < meshs.size(); i++ )
        {
            meshs[i]->Update();
        }

        //dx12.get()->Update();

        /*-------------------�`�揈��-------------------*/
        // �`�揀��
        dx12->BeginDraw();

        // �p�C�v���C���X�e�[�g�⃋�[�g�V�O�l�`���̐ݒ�
        render->BeginDraw();

        // �I�u�W�F�N�g
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

        // �`��㏈��
        dx12->EndDraw();
    }

    // GPU�̏������I���̂�҂�
    dx12->FlushGPU();
}

// �㏈��
void Application::Terminate()
{
    // �E�B���h�E�N���X��o�^����
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