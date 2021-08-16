#pragma once

#include "Texture.h"

class DirectX12Wrapper;
class Render;

class Graphic
{
private:
    Texture *texture;

    Graphic();
    ~Graphic();
    Graphic(const Graphic &) = delete;
    void operator=(const Graphic &) = delete;

public:
    static Graphic *GetInstance();

    void Initialize(DirectX12Wrapper &dx12, Render &render, ID3D12DescriptorHeap *basicDescHeap);
    void Terminate();
};