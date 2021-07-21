#pragma once

#include <xaudio2.h>
#include <wrl.h>
#include <vector>

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

class Sound
{
private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice *masterVoice;

    std::vector<SoundData> soundData{};

    static int soundDataIndex;

    Sound();
    Sound(const Sound &) = delete;
    void operator=(const Sound &) = delete;

public:
    ~Sound();

    static Sound *GetInstance();

    // 初期化処理
    void Initialize();

    // 後処理
    void Terminate();

    // WAVEファイルの読み込み
    int LoadSoundWave(const char *fileName);

    // 音声再生
    void SoundPlayWave(const int &index);

    // 音声データ解放
    void SoundUnload(SoundData *soundData);
};