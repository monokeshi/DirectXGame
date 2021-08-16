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

// ����������
void Sound::Initialize()
{
    // XAudio�G���W���̃C���X�^���X�𐶐�
    auto result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));

    // �}�X�^�[�{�C�X�𐶐�
    result = xAudio2->CreateMasteringVoice(&masterVoice);
    assert(SUCCEEDED(result));
}

// �㏈��
void Sound::Terminate()
{
    // �\�[�X�{�C�X�̔j��
    for ( int i = 0; i < sourceVoice.size(); i++ )
    {
        if ( sourceVoice[i] != nullptr )
        {
            sourceVoice[i]->DestroyVoice();
        }
    }

    // XAudio2���
    xAudio2.Reset();

    // �����f�[�^���
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

// WAVE�t�@�C���̓ǂݍ���
int Sound::LoadSoundWave(const char *fileName)
{
    /*------------�t�@�C���I�[�v��------------*/
    // �t�@�C�����̓X�g���[���̃C���X�^���X
    std::ifstream file;

    // .wav�t�@�C�����o�C�i�����[�h�ŊJ��
    file.open(fileName, std::ios_base::binary);

    // �t�@�C���I�[�v�����s�����o����
    assert(file.is_open());

    /*------------.wav�f�[�^�ǂݍ���------------*/
    // RIFF�w�b�_�[�̓ǂݍ���
    RiffHeader riff;
    file.read((char *)&riff, sizeof(riff));

    // �t�@�C����RIFF���`�F�b�N
    if ( strncmp(riff.chunk.id, "RIFF", 4) != 0 )
    {
        assert(0);
    }

    // �^�C�v��WAVE���`�F�b�N
    if ( strncmp(riff.type, "WAVE", 4) != 0 )
    {
        assert(0);
    }

    // Format�`�����N�̓ǂݍ���
    FotmatChunk format{};

    // �`�����N�w�b�_�[�̊m�F
    file.read((char *)&format, sizeof(ChunkHeader));
    if ( strncmp(format.chunk.id, "fmt ", 4) != 0 )
    {
        assert(0);
    }

    // �`�����N�{�̂̓ǂݍ���
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char *)&format.fmt, format.chunk.size);

    // Data�`�����N�̓ǂݍ���
    ChunkHeader data;
    file.read((char *)&data, sizeof(data));

    // JUNK�`�����N�����o�����ꍇ
    if ( strncmp(data.id, "JUNK", 4) == 0 )
    {
        // �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
        file.seekg(data.size, std::ios_base::cur);

        // �ēǂݍ���
        file.read((char *)&data, sizeof(data));
    }

    if ( strncmp(data.id, "data ", 4) != 0 )
    {
        assert(0);
    }

    // Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
    char *pBuffer = new char[data.size];
    file.read(pBuffer, data.size);

    // WAVLE�t�@�C�������
    file.close();

    /*------------�ǂݍ��񂾃f�[�^�̓Y������return------------*/
    SoundData sData;
    sData.wfex = format.fmt;
    sData.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
    sData.bufferSize = data.size;

    soundData.push_back(sData);
    sourceVoice.push_back(nullptr); // �\�[�X�{�C�X�̗̈���m�ۂ��Ă���
    XAUDIO2_BUFFER buf{};
    buffer.push_back(buf);          // �o�b�t�@���T�C�Y�𑝂₷
    isStop.push_back(false);

    ++soundDataIndex;

    return soundDataIndex - 1;
}

// �����Đ�
void Sound::PlaySoundWave(const int &handle, PlayType playType)
{
    HRESULT result;
    // �g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
    if ( sourceVoice[handle] == nullptr )
    {
        result = xAudio2->CreateSourceVoice(&sourceVoice[handle], (WAVEFORMATEX *)&soundData[handle].wfex);
        assert(SUCCEEDED(result));
    }

    // �Đ�����g�`�f�[�^�̐ݒ�
    buffer[handle].pAudioData = soundData[handle].pBuffer;
    buffer[handle].AudioBytes = soundData[handle].bufferSize;
    buffer[handle].Flags = XAUDIO2_END_OF_STREAM;

    // �������[�v
    if ( playType == PLAY_TYPE_LOOP )
    {
        buffer[handle].LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    else if ( playType == PLAY_TYPE_ONLY_ONCE )
    {
        buffer[handle].LoopCount = 0;
    }


    // ��~���łȂ�
    if ( !isStop[handle] )
    {
        // �L���[�Ƀo�b�t�@��ǉ�
        result = sourceVoice[handle]->SubmitSourceBuffer(&buffer[handle]);
        assert(SUCCEEDED(result));
    }

    // �g�`�f�[�^�̍Đ�
    result = sourceVoice[handle]->Start();
    assert(SUCCEEDED(result));

    isStop[handle] = false;
}

// �Đ���~
void Sound::StopSoundWave(const int &handle, StopType stopType)
{
    // null�`�F�b�N
    if ( sourceVoice[handle] == nullptr )
    {
        return;
    }

    // �Đ����łȂ�
    if ( !CheckSoundPlay(handle) )
    {
        return;
    }

    sourceVoice[handle]->Stop(0);

    // ���S��~
    if ( stopType == STOP_TYPE_COMPLETE_STOP )
    {
        auto result = sourceVoice[handle]->FlushSourceBuffers();
        assert(SUCCEEDED(result));

        result = sourceVoice[handle]->SubmitSourceBuffer(&buffer[handle]);
        assert(SUCCEEDED(result));
    }

    isStop[handle] = true;
}

// �Đ������`�F�b�N true:�Đ��� false:�Đ�����Ă��Ȃ�
bool Sound::CheckSoundPlay(const int &handle)
{
    // null&stop�`�F�b�N
    if ( sourceVoice[handle] == nullptr || isStop[handle] )
    {
        return false;
    }

    XAUDIO2_VOICE_STATE state;
    sourceVoice[handle]->GetState(&state);

    return !(state.BuffersQueued == 0);
}

// ���ʕύX
void Sound::ChangeSoundVolume(const int &handle, float volume)
{
    // null�`�F�b�N
    if ( sourceVoice[handle] == nullptr )
    {
        return;
    }

    Utility::Clamp(volume, 0.0f, 1.0f);

    auto result = sourceVoice[handle]->SetVolume(volume);
    assert(SUCCEEDED(result));
}

// �����f�[�^���
void Sound::UnloadSound(SoundData *soundData)
{
    // �o�b�t�@�����������
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}
