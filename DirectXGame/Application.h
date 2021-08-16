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

    // オブジェクト
    std::vector<Object3D *> object3Ds;

    // メッシュ
    std::vector<Mesh *> meshs;

    // スプライト
    std::vector<Sprite *> sprites;

    // テクスチャ
    Texture *texture;

    // テクスチャハンドル
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> texObj3DHandles;   // オブジェクト3D用
    std::vector<int> texSpriteHandles;                          // スプライト用
    int texDebugTextHandle;                                     // デバッグテキスト用

    // サウンド
    int shotSound;

    // ここに必要な変数(バッファやヒープなど)を書く
    // ウィンドウ周り
    WNDCLASSEX w{};
    HWND hwnd{};

    // ゲーム用ウィンドウの生成
    void CreateGameWindow();

    // シングルトンのためにコンストラクタをprivateに
    // コピーと代入を禁止する
    Application();
    Application(const Application &) = delete;
    void operator=(const Application &) = delete;

public:
    // Applicationのシングルトンインスタンスを得る
    static Application &GetInstance();
    // 初期化
    bool Initialize();

    // ループ - 起動
    void Run();

    // 後処理
    void Terminate();
    ~Application();
};