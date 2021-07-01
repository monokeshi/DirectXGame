#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dinput.h>

#include <vector>
#include <wrl.h>

class DirectX12Wrapper
{
private:
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    // DXGI�܂��
    ComPtr<IDXGIFactory6> dxgiFactory = nullptr;
    ComPtr<IDXGISwapChain4> swapChain = nullptr;

    // DirectX12�܂��
    ComPtr<ID3D12Device >dev = nullptr;
    ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
    ComPtr<ID3D12GraphicsCommandList> cmdList = nullptr;
    ComPtr<ID3D12CommandQueue> cmdQueue = nullptr;

    // �o�b�t�@�܂��
    std::vector<ComPtr<ID3D12Resource>> backBuffers;    // �o�b�N�o�b�t�@
    ComPtr<ID3D12DescriptorHeap> rtvHeaps = nullptr;    // �����_�[�^�[�Q�b�g�p�f�X�N���v�^�q�[�v
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};              // �f�X�N���v�^�q�[�v

    ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

    ComPtr<ID3D12Resource> depthBuffer = nullptr;

    // �t�F���X
    ComPtr<ID3D12Fence> fence = nullptr;
    UINT64 fenceVal = 0;

    D3D12_RESOURCE_BARRIER barrierDesc{};

    D3D12_VIEWPORT viewPort;    // �r���[�|�[�g
    D3D12_RECT scissorRect;     // �V�U�[��`

    // ���̓f�o�C�X
    IDirectInputDevice8 *devkeyboard = nullptr;

    // DXGI�܂�菉����
    HRESULT InitDXGIDevice();

    // �R�}���h�܂�菉����
    HRESULT InitCommand();

    // �X���b�v�`�F�C���̐���
    HRESULT CreateSwapChain(const HWND &hwnd);

    // �ŏI�I�ȃ����_�[�^�[�Q�b�g�̐���
    HRESULT CreateFinalRenderTarget();

    // �[�x�o�b�t�@�̐���
    HRESULT CreateDepthBuffer();

    // ���̓f�o�C�X�̐���
    HRESULT CreateInputDevice(WNDCLASSEX w, HWND hwnd);

public:
    DirectX12Wrapper(WNDCLASSEX w, HWND hwnd);
    ~DirectX12Wrapper();

    // �X�V����
    void Update();

    // �`�揈��
    void BeginDraw();   // �`�揀��
    void EndDraw();     // �`��㏈��

    ID3D12Device *GetDevice()
    {
        return dev.Get();
    }

    ID3D12GraphicsCommandList *GetCmdList()
    {
        return cmdList.Get();
    }

    IDirectInputDevice8 *GetDevkeyboard()
    {
        return devkeyboard;
    }
};