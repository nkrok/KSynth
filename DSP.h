#pragma once

#define NUM_VOICES 16
#define NUM_OSCILLATORS 2
#define MAX_UNISON 4
#define NUM_LFO 2

const double TWOPI = 4.0 * acos(0);

#include "Wavetable.h"
#include "Envelope.h"
#include "SynthParams.h"
#include "Voice.h"

class DSP
{
public:
  DSP()
  {
    for (int i = 0; i < NUM_VOICES; i++)
    {
      m_voices[i].reset(new Voice(m_synthParams));
    }

    //NoteOn(64, 0.5, 0);
  }

  double* Process(double time)
  {
    double out[2] = { 0.0 };

    for (int i = 0; i < NUM_VOICES; i++)
    {
      if (m_voices[i]->m_bDormant)
        continue;

      double *voiceOut = m_voices[i]->Process(time, m_iSampleRate);
      out[0] += voiceOut[0];
      out[1] += voiceOut[1];
    }

    return out;
  }

  void NoteOn(int noteNum, double velocity, double time)
  {
    m_voices[m_iNextVoice]->Start(noteNum, velocity, time);
    m_iNextVoice++;

    if (m_iNextVoice >= NUM_VOICES) m_iNextVoice = 0;
  }

  void NoteOff(int noteNum, double time)
  {
    for (int i = 0; i < NUM_VOICES; i++)
    {
      if (m_voices[i]->m_iNoteNum == noteNum)
      {
        m_voices[i]->Release(time);
      }
    }
  }

  void StopAllVoices()
  {
    for (int i = 0; i < NUM_VOICES; i++)
    {
      m_voices[i]->Stop();
    }
  }

  void SetParam(int paramIdx, double value)
  {
    switch (paramIdx)
    {
    case kOsc1Enabled:
      m_synthParams->GetOscParams(0)->m_bActive = (value == 1) ? true : false;
      break;

    case kOsc2Enabled:
      m_synthParams->GetOscParams(1)->m_bActive = (value == 1) ? true : false;
      break;

    case kOsc1WaveType:
      m_synthParams->GetOscParams(0)->SetWavetableAndWaveType(m_defaultWavetables->GetWavetableFromWaveType(static_cast<WaveType>(value)), static_cast<WaveType>(value));
      break;

    case kOsc2WaveType:
      m_synthParams->GetOscParams(1)->SetWavetableAndWaveType(m_defaultWavetables->GetWavetableFromWaveType(static_cast<WaveType>(value)), static_cast<WaveType>(value));
      break;

    case kOsc1Gain:
      m_synthParams->GetOscParams(0)->m_dGain = value / 100.0;
      break;

    case kOsc2Gain:
      m_synthParams->GetOscParams(1)->m_dGain = value / 100.0;
      break;

    case kOsc1Pan:
      m_synthParams->GetOscParams(0)->m_dPan = value / 100.0;
      break;

    case kOsc2Pan:
      m_synthParams->GetOscParams(1)->m_dPan = value / 100.0;
      break;

    case kOsc1OffsetOctave:
      if (abs(value) < 0.01)
        m_synthParams->GetOscParams(0)->m_dFreqMultiplier = 1.0;
      else
        m_synthParams->GetOscParams(0)->m_dFreqMultiplier = pow(2.0, value);

      break;

    case kOsc2OffsetOctave:
      if (abs(value) < 0.01)
        m_synthParams->GetOscParams(1)->m_dFreqMultiplier = 1;
      else
        m_synthParams->GetOscParams(1)->m_dFreqMultiplier = pow(2.0, value);

      break;

    case kOsc1UnisonAmount:
      m_synthParams->GetOscParams(0)->m_iUnisonAmount = value;
      break;

    case kOsc2UnisonAmount:
      m_synthParams->GetOscParams(1)->m_iUnisonAmount = value;
      break;

    case kOsc1UnisonDetune:
      m_synthParams->GetOscParams(0)->m_dUnisonDetune = value / 100;
      break;

    case kOsc2UnisonDetune:
      m_synthParams->GetOscParams(1)->m_dUnisonDetune = value / 100;
      break;

    case kEnvAttack:
      m_synthParams->ampEnv->SetAttack(value);
      break;

    case kEnvDecay:
      m_synthParams->ampEnv->SetDecay(value);
      break;

    case kEnvSustain:
      m_synthParams->ampEnv->SetSustain(value / 100.0);
      break;

    case kEnvRelease:
      m_synthParams->ampEnv->SetRelease(value);
      break;

    case kLFO1Enabled:
      m_synthParams->GetLFOParams(0)->m_bEnabled = (value == 1) ? true : false;
      break;

    case kLFO1Mode:
      m_synthParams->GetLFOParams(0)->m_mode = static_cast<LFOMode>(value);
      break;

    case kLFO1Amp:
      m_synthParams->GetLFOParams(0)->m_dAmplitude = value / 100.0;
      break;

    case kLFO1Rate:
      m_synthParams->GetLFOParams(0)->m_dFreq = value;
      break;
    }
  }

  void SetSampleRate(int rate)
  {
    m_iSampleRate = rate;
  }

private:
  int m_iSampleRate = 44100;
  std::unique_ptr<Voice> m_voices[NUM_VOICES];
  int m_iNextVoice = 0;

  std::shared_ptr<SynthParams> m_synthParams = std::make_unique<SynthParams>();
  std::shared_ptr<DefaultWavetables> m_defaultWavetables = std::make_unique<DefaultWavetables>();
};