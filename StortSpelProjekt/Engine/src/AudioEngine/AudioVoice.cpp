#include "stdafx.h"
#include "AudioVoice.h"
#include "AudioBuffer.h"
#include "../Misc/Option.h"
#include "../Events/EventBus.h"

AudioVoice::AudioVoice()
{
    m_Initialized = false;
}

AudioVoice::AudioVoice(AudioBuffer& audio)
{
    initialize(&audio.m_Buffer, &audio.m_Wfx);
}

AudioVoice::~AudioVoice()
{
}

void AudioVoice::initialize(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat)
{
    m_pBuffer = buff;
    m_pWfxFormat = wfxFormat;
    if (FAILED(AudioEngine::GetInstance().GetAudioEngine()->CreateSourceVoice(&m_pSourceVoice, (WAVEFORMATEX*)m_pWfxFormat)))
    {
        Log::Print("Failed creating sound source\n");
    }

    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(m_pBuffer)))
    {
        Log::Print("Failed to submit source buffer\n");
    }

    m_pSourceVoice->SetVolume(std::atof(Option::GetInstance().GetVariable("f_volume").c_str()));
    HRESULT hRes = m_pSourceVoice->SetOutputVoices(NULL);

    Stop();
}

void AudioVoice::Initialize(AudioBuffer& audio)
{
    initialize(&audio.m_Buffer, &audio.m_Wfx);
}

IXAudio2SourceVoice* AudioVoice::GetSourceVoice()
{
    return m_pSourceVoice;
}

void AudioVoice::Play()
{
    if (m_Initialized)
    {
        // reset the buffer if sound has ended
        XAUDIO2_VOICE_STATE test;
        m_pSourceVoice->GetState(&test);
        if (test.BuffersQueued >= 0)
        {
            Stop();
        }

        // play the sound
        if (FAILED(m_pSourceVoice->Start(0)))
        {
            Log::Print("Error playing audio\n");
        }
        else
        {
            m_Playing = true;
        }
    }
}

void AudioVoice::Stop()
{
    if (m_Initialized)
    {
        // stop playback
        if (FAILED(m_pSourceVoice->Stop(0)))
        {
            Log::Print("Error stopping audio\n");
        }
        else
        {
            m_Playing = false;
        }
        // reset the buffer so the sound starts from the beginning at next playback
        m_pSourceVoice->FlushSourceBuffers();
        m_pSourceVoice->SubmitSourceBuffer(m_pBuffer);
    }
}

void AudioVoice::Resume()
{
    if (m_Initialized)
    {
        // reset the buffer if sound has ended
        XAUDIO2_VOICE_STATE voiceState;
        m_pSourceVoice->GetState(&voiceState);
        if (voiceState.BuffersQueued == 0)
        {
            // reset the buffer so the sound starts from the beginning at next playback
            m_pSourceVoice->FlushSourceBuffers();
            m_pSourceVoice->SubmitSourceBuffer(m_pBuffer);
        }

        // play the sound
        if (FAILED(m_pSourceVoice->Start(0)))
        {
            Log::PrintSeverity(Log::Severity::WARNING,"Error playing audio\n");
        }
        else
        {
            m_Playing = true;
        }
    }
}

void AudioVoice::Pause()
{
    if (m_Initialized)
    {
        // stop playback
        if (FAILED(m_pSourceVoice->Stop(0)))
        {
            Log::PrintSeverity(Log::Severity::WARNING, "Error stopping audio\n");
        }
        else
        {
            m_Playing = false;
        }
    }
}

bool AudioVoice::IsPlaying()
{
    return m_Playing;
}
