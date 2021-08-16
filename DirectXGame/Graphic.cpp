#include "Graphic.h"
#include "DirectX12Wrapper.h"
#include "Render.h"

Graphic::Graphic()
{
    texture = nullptr;
}

Graphic::~Graphic()
{
}

Graphic *Graphic::GetInstance()
{
    static Graphic instance;
    return &instance;
}

void Graphic::Initialize(DirectX12Wrapper &dx12, Render &render, ID3D12DescriptorHeap *basicDescHeap)
{
    texture = new Texture(dx12, render, basicDescHeap);
}

void Graphic::Terminate()
{
    delete texture;
}
