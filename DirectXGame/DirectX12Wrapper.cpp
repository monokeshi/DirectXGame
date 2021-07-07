#include "DirectX12Wrapper.h"
#include "Application.h"
#include "Define.h"

#include <d3d12.h>
#include <d3dx12.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#include <assert.h>
#include <vector>
#include <string>

using namespace Microsoft::WRL;

// �f�o�b�O���C���[��L���ɂ���
void EnableDebugLayer()
{
    ComPtr<ID3D12Debug> debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
}

DirectX12Wrapper::DirectX12Wrapper(WNDCLASSEX w, HWND hwnd)
{
#ifdef _DEBUG
    //�f�o�b�O���C���[���I��
    EnableDebugLayer();
#endif
    if ( FAILED(InitDXGIDevice()) )
    {
        assert(0);
        return;
    }

    // �R�}���h�܂�菉����
    if ( FAILED(InitCommand()) )
    {
        assert(0);
        return;
    }

    // �X���b�v�`�F�[���̐���
    if ( FAILED(CreateSwapChain(hwnd)) )
    {
        assert(0);
        return;
    }

    // �ŏI�I�ȃ����_�[�^�[�Q�b�g
    if ( FAILED(CreateFinalRenderTarget()) )
    {
        assert(0);
        return;
    }

    // �[�x�o�b�t�@�̐���
    if ( FAILED(CreateDepthBuffer()) )
    {
        assert(0);
        return;
    }

    // �t�F���X�̐���
    if ( FAILED(dev->CreateFence(fenceVal,
                                 D3D12_FENCE_FLAG_NONE,
                                 IID_PPV_ARGS(&fence))) )
    {
        assert(0);
        return;
    }

    // ���̓f�o�C�X�̐���
    if ( FAILED(CreateInputDevice(w, hwnd)) )
    {
        assert(0);
        return;
    }
}

DirectX12Wrapper::~DirectX12Wrapper()
{
}

// DXGI�܂�菉����
HRESULT DirectX12Wrapper::InitDXGIDevice()
{
    // DXGI�t�@�N�g���[�̐���
    auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(result));

    // �A�_�v�^�[�̗񋓗p
    std::vector<ComPtr<IDXGIAdapter1>> adapters;

    // �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
    ComPtr<IDXGIAdapter1> tmpAdapter = nullptr;
    for ( int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) !=
         DXGI_ERROR_NOT_FOUND; i++ )
    {
        adapters.push_back(tmpAdapter.Get()); // ���I�z��ɒǉ�����
    }

    for ( unsigned int i = 0; i < adapters.size(); i++ )
    {
        DXGI_ADAPTER_DESC1 adesc;
        adapters[i]->GetDesc1(&adesc); // �A�_�v�^�[�̏����擾

        // �\�t�g�E�F�A�f�o�C�X�����
        if ( adesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE )
        {
            continue;
        }

        std::wstring strDesc = adesc.Description; // �A�_�v�^�[��
        // Intel UHD Graphics(�I���{�[�h�O���t�B�b�N)�����
        if ( strDesc.find(L"Intel") == std::wstring::npos )
        {
            tmpAdapter = adapters[i]; // �̗p
            break;
        }
    }

    // �Ή����x���̔z��
    D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;
    for ( int i = 0; i < _countof(levels); levels )
    {
        // �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
        result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
        if ( SUCCEEDED(result) )
        {
            // �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
            featureLevel = levels[i];
            break;
        }
    }

    return result;
}

// �R�}���h�܂�菉����
HRESULT DirectX12Wrapper::InitCommand()
{
    // �R�}���h�A���P�[�^�[�𐶐�
    auto result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              IID_PPV_ARGS(&cmdAllocator));
    assert(SUCCEEDED(result));

    // �R�}���h���X�g�𐶐�
    result = dev->CreateCommandList(0,
                                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                                    cmdAllocator.Get(),
                                    nullptr,
                                    IID_PPV_ARGS(&cmdList));
    assert(SUCCEEDED(result));

    // �W���ݒ�ŃR�}���h�L���[�𐶐�
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
    result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
    assert(SUCCEEDED(result));

    return result;
}

// �X���b�v�`�F�C���̐���
HRESULT DirectX12Wrapper::CreateSwapChain(const HWND &hwnd)
{
    // �e��ݒ�
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
    swapchainDesc.Width = WINDOW_WIDTH;
    swapchainDesc.Height = WINDOW_HEIGHT;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;        // �F���̏���
    swapchainDesc.SampleDesc.Count = 1;                       // �}���`�T���v�����Ȃ�
    swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;       // �o�b�N�o�b�t�@�p
    swapchainDesc.BufferCount = 2;                            // �o�b�t�@����2�ɐݒ�
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    assert(cmdQueue != nullptr);

    //ComPtr<IDXGISwapChain1> swapChain1;

    // �X���b�v�`�F�[������
    auto result = dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(),
                                                      hwnd,
                                                      &swapchainDesc,
                                                      nullptr,
                                                      nullptr,
                                                      //&swapChain1);
                                                      (IDXGISwapChain1 **)swapChain.ReleaseAndGetAddressOf());

    // ��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�
    //swapChain1.As(&swapChain);

    return result;
}

// �ŏI�I�ȃ����_�[�^�[�Q�b�g�̐���
HRESULT DirectX12Wrapper::CreateFinalRenderTarget()
{
    // �e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
    heapDesc.NumDescriptors = 2;                    // ���\��2��
    auto result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    if ( FAILED(result) )
    {
        assert(0);
        return result;
    }

    DXGI_SWAP_CHAIN_DESC swcDesc{};
    result = swapChain->GetDesc(&swcDesc);
    backBuffers.resize(swcDesc.BufferCount);

    for ( int i = 0; i < 2; i++ )
    {
        // �X���b�v�`�F�[������o�b�t�@���擾
        result = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
        assert(SUCCEEDED(result));
        dev->CreateRenderTargetView(backBuffers[i].Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
                                                                                                 i,
                                                                                                 dev->GetDescriptorHandleIncrementSize(heapDesc.Type)));
    }

    return result;
}

// �[�x�o�b�t�@�̐���
HRESULT DirectX12Wrapper::CreateDepthBuffer()
{
    // �[�x�o�b�t�@���\�[�X�ݒ�
    CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                                                                      WINDOW_WIDTH,
                                                                      WINDOW_HEIGHT,
                                                                      1, 0,
                                                                      1, 0,
                                                                      D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    // �[�x�o�b�t�@�̐���
    auto result = dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                               D3D12_HEAP_FLAG_NONE,
                                               &depthResDesc,
                                               D3D12_RESOURCE_STATE_DEPTH_WRITE,    // �[�x�l�������݂Ɏg�p
                                               &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
                                               IID_PPV_ARGS(&depthBuffer));
    assert(SUCCEEDED(result));

    // �[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;                     // �[�x�r���[��1��
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;  // �f�v�X�X�e���V���r���[
    result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
    assert(SUCCEEDED(result));

    // �[�x�r���[�쐬
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // �[�x�l�t�H�[�}�b�g
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dev->CreateDepthStencilView(depthBuffer.Get(),
                                &dsvDesc,
                                dsvHeap->GetCPUDescriptorHandleForHeapStart());

    return result;
}

// ���̓f�o�C�X�̐���
HRESULT DirectX12Wrapper::CreateInputDevice(WNDCLASSEX w, HWND hwnd)
{
    // ���͂̐���
    IDirectInput8 *dinput = nullptr;
    auto result = DirectInput8Create(w.hInstance,
                                     DIRECTINPUT_VERSION,
                                     IID_IDirectInput8,
                                     (void **)&dinput,
                                     nullptr);
    assert(SUCCEEDED(result));

    // �L�[�{�[�h�f�o�C�X�̐���
    result = dinput->CreateDevice(GUID_SysKeyboard,
                                  &devkeyboard,
                                  NULL);
    assert(SUCCEEDED(result));

    // ���̓f�[�^�`���̃Z�b�g
    result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
    assert(SUCCEEDED(result));

    // �r�����䃌�x���̃Z�b�g
    result = devkeyboard->SetCooperativeLevel(hwnd,
                                              DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(result));

    return result;
}

// �X�V����
void DirectX12Wrapper::Update()
{
}

// GPU�̏������I���̂�҂�
void DirectX12Wrapper::FlushGPU()
{
    cmdQueue->Signal(fence.Get(), ++fenceVal);
    if ( fence->GetCompletedValue() != fenceVal )
    {
        HANDLE event = CreateEvent(nullptr, false, false, nullptr);
        fence->SetEventOnCompletion(fenceVal, event);
        assert(event != nullptr);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

// �`�揀��
void DirectX12Wrapper::BeginDraw()
{
    // �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
    UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

    // ���\�[�X�o���A�ŏ������݉\�ɕύX
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
                                                                      D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // �`���w��
    // �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
    // �[�x�X�e���V���r���[�p�̃f�X�N���v�^�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

    // ��ʃN���A
    /*                       R      G     B     A                       */
    float clearColor[] = { 0.9f, 0.5f, 0.5f, 0.0f };
    cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
    cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // �`��R�}���h��������
    // �r���[�|�[�g�̐ݒ�
    cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<FLOAT>(WINDOW_WIDTH), static_cast<FLOAT>(WINDOW_HEIGHT)));

    // �V�U�[��`�̐ݒ�
    cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
}

// �`��㏈��
void DirectX12Wrapper::EndDraw()
{
    // �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
    UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

    // ���\�[�X�o���A��߂�
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
                                                                      D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                      D3D12_RESOURCE_STATE_PRESENT));

    // ���߂̃N���[�Y
    cmdList->Close();

    // �R�}���h���X�g�̎��s
    ID3D12CommandList *cmdLists[] = { cmdList.Get() };  // �R�}���h���X�g�̔z��
    cmdQueue->ExecuteCommandLists(1, cmdLists);         // �R�}���h���X�g�̎��s������҂�
    FlushGPU();

    cmdAllocator->Reset();                          // �L���[���N���A
    cmdList->Reset(cmdAllocator.Get(), nullptr);    // �ĂуR�}���h���X�g�����߂鏀��

    // �o�b�t�@���t���b�v(���\�̓���ւ�)
    swapChain->Present(1, 0);
}