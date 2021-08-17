#pragma once

#include <xaudio2.h>
#include <wrl.h>
#include <vector>
#include <memory>

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

// �v���C�^�C�v
enum PlayType
{
    PLAY_TYPE_ONLY_ONCE,    // 1�x�����Đ�
    PLAY_TYPE_LOOP,         // ���[�v
};

enum StopType
{
    STOP_TYPE_PAUSE,            // �ꎞ��~
    STOP_TYPE_COMPLETE_STOP,    // ���S��~
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

    // ����������
    void Initialize();

    // �㏈��
    void Terminate();

    // WAVE�t�@�C���̓ǂݍ���
    int LoadSoundWave(const char *fileName);

    // �����Đ�
    void PlaySoundWave(const int &handle, PlayType playType);

    // �Đ���~
    void StopSoundWave(const int &handle, StopType stopType);

    // �Đ������`�F�b�N true:�Đ��� false:�Đ�����Ă��Ȃ�
    bool CheckSoundPlay(const int &handle);

    // ���ʕύX
    // volume 0.0f ~ 1.0f
    void ChangeSoundVolume(const int &handle, float volume);

    // �����f�[�^���
    void UnloadSound(SoundData *soundData);

    // ���ʎ擾
    float GetSoundVolume(const int &handle) const
    {
        // null�`�F�b�N
        if ( sourceVoice[handle] == nullptr )
        {
            return 0.0f;
        }

        float volume;
        sourceVoice[handle]->GetVolume(&volume);
        return volume;
    }
};