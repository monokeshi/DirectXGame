#pragma once

#include <xaudio2.h>
#include <wrl.h>
#include <vector>
#include <memory>

// チャンクヘッダ
struct ChunkHeader
{
    char id[4];     // チャンク毎のID
    int32_t size;   // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader
{
    ChunkHeader chunk;  // "RIFF"
    char type[4];       // "WAVE"
};

// FMTチャンク
struct FotmatChunk
{
    ChunkHeader chunk;  // "fmt"
    WAVEFORMATEX fmt;   // 波形フォーマット
};

// 音声データ
struct SoundData
{
    WAVEFORMATEX wfex;          // 波形フォーマット
    BYTE *pBuffer;              // バッファの先頭アドレス
    unsigned int bufferSize;    // バッファのサイズ
};

// プレイタイプ
enum PlayType
{
    PLAY_TYPE_ONLY_ONCE,    // 1度だけ再生
    PLAY_TYPE_LOOP,         // ループ
};

enum StopType
{
    STOP_TYPE_PAUSE,            // 一時停止
    STOP_TYPE_COMPLETE_STOP,    // 完全停止
};

extern const float MIN_VOLUME;
extern const float MAX_VOLUME;

class Sound
{
private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice *masterVoice;

    std::vector<SoundData> soundData{};

    std::vector<IXAudio2SourceVoice *> sourceVoice;
    std::vector<XAUDIO2_BUFFER> buffer{};
    std::vector<bool> isStop;

    static int soundDataIndex;

    Sound();
    ~Sound();
    Sound(const Sound &) = delete;
    void operator=(const Sound &) = delete;

public:
    static Sound *GetInstance();

    // 初期化処理
    void Initialize();

    // 後処理
    void Terminate();

    // WAVEファイルの読み込み
    int LoadSoundWave(const char *fileName);

    // 音声再生
    void PlaySoundWave(const int &handle, PlayType playType);

    // 再生停止
    void StopSoundWave(const int &handle, StopType stopType);

    // 再生中かチェック true:再生中 false:再生されていない
    bool CheckSoundPlay(const int &handle);

    // 音量変更
    // volume 0.0f ~ 1.0f
    void ChangeSoundVolume(const int &handle, float volume);

    // 音声データ解放
    void UnloadSound(SoundData *soundData);

    // 音量取得
    float GetSoundVolume(const int &handle) const
    {
        // nullチェック
        if ( sourceVoice[handle] == nullptr )
        {
            return 0.0f;
        }

        float volume;
        sourceVoice[handle]->GetVolume(&volume);
        return volume;
    }
};