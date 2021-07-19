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
    static const int MAX_CHAR_COUNT = 256;  // 1フレームでの最大表示文字数
    static const int FONT_WIDTH = 9;        // フォント画像内1文字分の横幅
    static const int FONT_HEIGHT = 18;      // フォント画像内1文字分の縦幅
    static const int FONT_LINE_COUNT = 14;  // フォント画像内1行分の文字数

    int texIndex;

    std::vector<Sprite> sprites;

    int spriteIndex;    // スプライト配列の添え字

public:
    // 初期化処理
    void Initialize(const int &texIndex, DirectX12Wrapper &dx12, Render &render, Texture &texture);

    // 画面に描画したい文字列を1行分追加
    void Print(float x, float y, const std::string &text, float scale = 1.0f);

    // まとめて描画
    void DrawAll();
};