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
    // XAudio2���
    xAudio2.Reset();

    // �����f�[�^���
    for ( int i = 0; i < soundData.size(); ++i )
    {
        SoundUnload(&soundData[i]);
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

    ++soundDataIndex;

    return soundDataIndex - 1;
}

// �����Đ�
void Sound::SoundPlayWave(const int &index)
{
    // �g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
    IXAudio2SourceVoice *pSourceVoice = nullptr;
    auto result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[index].wfex);
    assert(SUCCEEDED(result));

    // �Đ�����g�`�f�[�^�̐ݒ�
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData[index].pBuffer;
    buf.AudioBytes = soundData[index].bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // �g�`�f�[�^�̍Đ�
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}

// �����f�[�^���
void Sound::SoundUnload(SoundData *soundData)
{
    // �o�b�t�@�����������
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}
