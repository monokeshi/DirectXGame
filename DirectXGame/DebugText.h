#pragma once

#include "Sprite.h"

#include <vector>
#include <string>

class DirectX12Wrapper;
class Render;
class Texture;

class DebugText
{
private:
    static const int MAX_CHAR_COUNT = 256;  // 1�t���[���ł̍ő�\��������
    static const int FONT_WIDTH = 9;        // �t�H���g�摜��1�������̉���
    static const int FONT_HEIGHT = 18;      // �t�H���g�摜��1�������̏c��
    static const int FONT_LINE_COUNT = 14;  // �t�H���g�摜��1�s���̕�����

    int texHandle;

    std::vector<Sprite> sprites;

    int spriteIndex;    // �X�v���C�g�z��̓Y����

    DebugText();
    ~DebugText();
    DebugText(const DebugText &) = delete;
    void operator=(const DebugText &) = delete;

public:
    static DebugText *GetInstance();

    // ����������
    void Initialize(const int &texHandle, DirectX12Wrapper &dx12, Render &render, Texture &texture);

    // ��ʂɕ`�悵�����������1�s���ǉ�
    void Print(float x, float y, float scale, const std::string &text);

    void PrintFormat(float x, float y, float scale, const std::string &text, ...);

    // �܂Ƃ߂ĕ`��
    void DrawAll();
};