#include "Sound.h"
#include "Utility.h"

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
    static Sound instance;
    return &instance;
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
    // ソースボイスの破棄
    for ( int i = 0; i < sourceVoice.size(); i++ )
    {
        if ( sourceVoice[i] != nullptr )
        {
            sourceVoice[i]->DestroyVoice();
        }
    }

    // XAudio2解放
    xAudio2.Reset();

    // 音声データ解放
    for ( int i = 0; i < soundData.size(); ++i )
    {
        UnloadSound(&soundData[i]);
    }

    for ( int i = static_cast<int>(sourceVoice.size()) - 1; i >= 0; i-- )
    {
        if ( sourceVoice[i] == nullptr )
        {
            delete sourceVoice[i];
            sourceVoice.erase(sourceVoice.begin() + i);
        }
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
    sourceVoice.push_back(nullptr); // ソースボイスの領域を確保しておく
    XAUDIO2_BUFFER buf{};
    buffer.push_back(buf);          // バッファをサイズを増やす
    isStop.push_back(false);

    ++soundDataIndex;

    return soundDataIndex - 1;
}

// 音声再生
void Sound::PlaySoundWave(const int &handle, PlayType playType)
{
    HRESULT result;
    // 波形フォーマットをもとにSourceVoiceの生成
    if ( sourceVoice[handle] == nullptr )
    {
        result = xAudio2->CreateSourceVoice(&sourceVoice[handle], (WAVEFORMATEX *)&soundData[handle].wfex);
        assert(SUCCEEDED(result));
    }

    // 再生する波形データの設定
    buffer[handle].pAudioData = soundData[handle].pBuffer;
    buffer[handle].AudioBytes = soundData[handle].bufferSize;
    buffer[handle].Flags = XAUDIO2_END_OF_STREAM;

    // 無限ループ
    if ( playType == PLAY_TYPE_LOOP )
    {
        buffer[handle].LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    else if ( playType == PLAY_TYPE_ONLY_ONCE )
    {
        buffer[handle].LoopCount = 0;
    }


    // 停止中でない
    if ( !isStop[handle] )
    {
        // キューにバッファを追加
        result = sourceVoice[handle]->SubmitSourceBuffer(&buffer[handle]);
        assert(SUCCEEDED(result));
    }

    // 波形データの再生
    result = sourceVoice[handle]->Start();
    assert(SUCCEEDED(result));

    isStop[handle] = false;
}

// 再生停止
void Sound::StopSoundWave(const int &handle, StopType stopType)
{
    // nullチェック
    if ( sourceVoice[handle] == nullptr )
    {
        return;
    }

    // 再生中でない
    if ( !CheckSoundPlay(handle) )
    {
        return;
    }

    sourceVoice[handle]->Stop(0);

    // 完全停止
    if ( stopType == STOP_TYPE_COMPLETE_STOP )
    {
        auto result = sourceVoice[handle]->FlushSourceBuffers();
        assert(SUCCEEDED(result));

        result = sourceVoice[handle]->SubmitSourceBuffer(&buffer[handle]);
        assert(SUCCEEDED(result));
    }

    isStop[handle] = true;
}

// 再生中かチェック true:再生中 false:再生されていない
bool Sound::CheckSoundPlay(const int &handle)
{
    // null&stopチェック
    if ( sourceVoice[handle] == nullptr || isStop[handle] )
    {
        return false;
    }

    XAUDIO2_VOICE_STATE state;
    sourceVoice[handle]->GetState(&state);

    return !(state.BuffersQueued == 0);
}

// 音量変更
void Sound::ChangeSoundVolume(const int &handle, float volume)
{
    // nullチェック
    if ( sourceVoice[handle] == nullptr )
    {
        return;
    }

    Utility::Clamp(volume, 0.0f, 1.0f);

    auto result = sourceVoice[handle]->SetVolume(volume);
    assert(SUCCEEDED(result));
}

// 音声データ解放
void Sound::UnloadSound(SoundData *soundData)
{
    // バッファメモリを解放
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}
