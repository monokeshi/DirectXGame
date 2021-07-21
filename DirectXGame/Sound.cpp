#include "Sound.h"

#include <fstream>
#include <assert.h>

int Sound::soundDataIndex = 0;

Sound::Sound()
{
}

Sound::~Sound()
{
}

Sound *Sound::GetInstance()
{
    static Sound sound;
    return &sound;
}

// 初期化処理
void Sound::Initialize()
{
    // XAudioエンジンのインスタンスを生成
    auto result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));

    // マスターボイスを生成
    result = xAudio2->CreateMasteringVoice(&masterVoice);
    assert(SUCCEEDED(result));
}

// 後処理
void Sound::Terminate()
{
    // XAudio2解放
    xAudio2.Reset();

    // 音声データ解放
    for ( int i = 0; i < soundData.size(); ++i )
    {
        SoundUnload(&soundData[i]);
    }
}

// WAVEファイルの読み込み
int Sound::LoadSoundWave(const char *fileName)
{
    /*------------ファイルオープン------------*/
    // ファイル入力ストリームのインスタンス
    std::ifstream file;

    // .wavファイルをバイナリモードで開く
    file.open(fileName, std::ios_base::binary);

    // ファイルオープン失敗を検出する
    assert(file.is_open());

    /*------------.wavデータ読み込み------------*/
    // RIFFヘッダーの読み込み
    RiffHeader riff;
    file.read((char *)&riff, sizeof(riff));

    // ファイルがRIFFかチェック
    if ( strncmp(riff.chunk.id, "RIFF", 4) != 0 )
    {
        assert(0);
    }

    // タイプがWAVEかチェック
    if ( strncmp(riff.type, "WAVE", 4) != 0 )
    {
        assert(0);
    }

    // Formatチャンクの読み込み
    FotmatChunk format{};

    // チャンクヘッダーの確認
    file.read((char *)&format, sizeof(ChunkHeader));
    if ( strncmp(format.chunk.id, "fmt ", 4) != 0 )
    {
        assert(0);
    }

    // チャンク本体の読み込み
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char *)&format.fmt, format.chunk.size);

    // Dataチャンクの読み込み
    ChunkHeader data;
    file.read((char *)&data, sizeof(data));

    // JUNKチャンクを検出した場合
    if ( strncmp(data.id, "JUNK", 4) == 0 )
    {
        // 読み取り位置をJUNKチャンクの終わりまで進める
        file.seekg(data.size, std::ios_base::cur);

        // 再読み込み
        file.read((char *)&data, sizeof(data));
    }

    if ( strncmp(data.id, "data ", 4) != 0 )
    {
        assert(0);
    }

    // Dataチャンクのデータ部(波形データ)の読み込み
    char *pBuffer = new char[data.size];
    file.read(pBuffer, data.size);

    // WAVLEファイルを閉じる
    file.close();

    /*------------読み込んだデータの添え字をreturn------------*/
    SoundData sData;
    sData.wfex = format.fmt;
    sData.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
    sData.bufferSize = data.size;

    soundData.push_back(sData);

    ++soundDataIndex;

    return soundDataIndex - 1;
}

// 音声再生
void Sound::SoundPlayWave(const int &index)
{
    // 波形フォーマットをもとにSourceVoiceの生成
    IXAudio2SourceVoice *pSourceVoice = nullptr;
    auto result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[index].wfex);
    assert(SUCCEEDED(result));

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData[index].pBuffer;
    buf.AudioBytes = soundData[index].bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // 波形データの再生
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}

// 音声データ解放
void Sound::SoundUnload(SoundData *soundData)
{
    // バッファメモリを解放
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}
