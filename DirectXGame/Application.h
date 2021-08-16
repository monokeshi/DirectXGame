#pragma once

#include "Texture.h"
#include "Object3D.h"
#include "Mesh.h"
#include "Sprite.h"

#include <memory>
#include <vector>

class DirectX12Wrapper;
class Render;
class Camera;

class Application
{
private:
    std::shared_ptr<DirectX12Wrapper> dx12;
    std::shared_ptr<Render> render;
    std::shared_ptr<Camera> camera;

    // �I�u�W�F�N�g
    std::vector<Object3D *> object3Ds;

    // ���b�V��
    std::vector<Mesh *> meshs;

    // �X�v���C�g
    std::vector<Sprite *> sprites;

    // �e�N�X�`��
    Texture *texture;

    // �e�N�X�`���n���h��
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> texObj3DHandles;   // �I�u�W�F�N�g3D�p
    std::vector<int> texSpriteHandles;                          // �X�v���C�g�p
    int texDebugTextHandle;                                     // �f�o�b�O�e�L�X�g�p

    // �T�E���h
    int shotSound;

    // �����ɕK�v�ȕϐ�(�o�b�t�@��q�[�v�Ȃ�)������
    // �E�B���h�E����
    WNDCLASSEX w{};
    HWND hwnd{};

    // �Q�[���p�E�B���h�E�̐���
    void CreateGameWindow();

    // �V���O���g���̂��߂ɃR���X�g���N�^��private��
    // �R�s�[�Ƒ�����֎~����
    Application();
    Application(const Application &) = delete;
    void operator=(const Application &) = delete;

public:
    // Application�̃V���O���g���C���X�^���X�𓾂�
    static Application &GetInstance();
    // ������
    bool Initialize();

    // ���[�v - �N��
    void Run();

    // �㏈��
    void Terminate();
    ~Application();
};