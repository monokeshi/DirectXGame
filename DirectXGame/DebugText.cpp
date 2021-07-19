#include "DebugText.h"
#include "DirectX12Wrapper.h"
#include "Render.h"
#include "Texture.h"
#include "Define.h"

void DebugText::Initialize(const int &texIndex, DirectX12Wrapper &dx12, Render &render, Texture &texture)
{
    this->texIndex = texIndex;
    spriteIndex = 0;

    for ( int i = 0; i < MAX_CHAR_COUNT; ++i )
    {
        auto sprite = Sprite({ 0.0f, 0.0f, 0.0f },          // position
                             0.0f,                          // rotation
                             { 1.0f, 1.0f, 1.0f, 1.0f },    // color
                             { 0.0f, 0.0f },                // size
                             nullptr,
                             dx12,
                             render,
                             texture);
        sprite.Initialize();

        sprites.push_back(sprite);
    }
}

void DebugText::Print(float x, float y, const std::string &text, float scale)
{
    for ( int i = 0; i < text.size(); ++i )
    {
        // �ő啶��������
        if ( spriteIndex >= MAX_CHAR_COUNT )
        {
            break;
        }

        // �ꕶ�����o��
        const unsigned char &character = text[i];

        // ASCII�R�[�h��2�i����΂����ԍ����v�Z
        int fontIndex = character - 32;
        if ( character >= 0x7f )
        {
            fontIndex = 0;
        }
        int fontIndexY = fontIndex / FONT_LINE_COUNT;
        int fontIndexX = fontIndex % FONT_LINE_COUNT;


        // ���W�v�Z
        sprites[spriteIndex].SetPosition({ x + FONT_WIDTH * scale * i, y, 0.0f });
        sprites[spriteIndex].ClippingTexture({ static_cast<float>(fontIndexX) * FONT_WIDTH, static_cast<float>(fontIndexY) * FONT_HEIGHT },
                                             { FONT_WIDTH, FONT_HEIGHT },
                                             texIndex);
        sprites[spriteIndex].SetSize({ FONT_WIDTH * scale, FONT_HEIGHT * scale });

        // �X�V
        sprites[spriteIndex].Update();

        // ��������i�߂�
        ++spriteIndex;
    }
}

// �܂Ƃ߂ĕ`��
void DebugText::DrawAll()
{
    for ( int i = 0; i < spriteIndex; ++i )
    {
        sprites[i].Draw(texIndex);
    }

    spriteIndex = 0;
}