#pragma once

#include <xaudio2.h>
#include <wrl.h>
#include <vector>

// �`�����N�w�b�_
struct ChunkHeader
{
    char id[4];     // �`�����N����ID
    int32_t size;   // �`�����N�T�C�Y
};

// RIFF�w�b�_�`�����N
struct RiffHeader
{
    ChunkHeader chunk;  // "RIFF"
    char type[4];       // "WAVE"
};

// FMT�`�����N
struct FotmatChunk
{
    ChunkHeader chunk;  // "fmt"
    WAVEFORMATEX fmt;   // �g�`�t�H�[�}�b�g
};

// �����f�[�^
struct SoundData
{
    WAVEFORMATEX wfex;          // �g�`�t�H�[�}�b�g
    BYTE *pBuffer;              // �o�b�t�@�̐擪�A�h���X
    unsigned int bufferSize;    // �o�b�t�@�̃T�C�Y
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

    // ����������
    void Initialize();

    // �㏈��
    void Terminate();

    // WAVE�t�@�C���̓ǂݍ���
    int LoadSoundWave(const char *fileName);

    // �����Đ�
    void SoundPlayWave(const int &index);

    // �����f�[�^���
    void SoundUnload(SoundData *soundData);
};