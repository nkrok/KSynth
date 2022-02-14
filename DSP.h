#pragma once

#define NUM_VOICES 16
#define NUM_OSCILLATORS 2

const double TWOPI = 4.0 * acos(0);

#include "Envelope.h"
#include "KLFO.h"
#include "KOscillator.h"

class DSP
{
public:
  DSP()
  {
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
      m_osc[i].m_iOscID = i;
    }

    //NoteOn(64, 0.5, 0);
  }

  double* Process(double time)
  {
    double out[2] = { 0.0 };

    for (int i = 0; i < NUM_VOICES; i++)
    {
      if (m_voices[i].m_bDormant)
        continue;

      double dAmp = m_voices[i].GetAmplitude(time, m_envelope);

      for (int k = 0; k < NUM_OSCILLATORS; k++)
      {
        if (m_osc[k].m_bActive)
        {
          double *o = m_osc[k].Process(m_voices[i], m_iSampleRate, time, m_LFO);
          out[0] += o[0] * dAmp;
          out[1] += o[1] * dAmp;
        }
      }
    }

    return out;
  }

  void NoteOn(int noteNum, double velocity, double time)
  {
    m_voices[m_iNextVoice].Start(noteNum, velocity, time);
    m_iNextVoice++;

    if (m_iNextVoice >= NUM_VOICES) m_iNextVoice = 0;
  }

  void NoteOff(int noteNum, double time)
  {
    for (int i = 0; i < NUM_VOICES; i++)
    {
      if (m_voices[i].m_iNoteNum == noteNum)
      {
        m_voices[i].Release(time);
      }
    }
  }

  void StopAllVoices()
  {
    for (int i = 0; i < NUM_VOICES; i++)
    {
      m_voices[i].Stop();
    }
  }

  void SetParam(int paramIdx, double value)
  {
    switch (paramIdx)
    {
    case kOsc1Enabled:
      m_osc[0].m_bActive = (value == 1) ? true : false;
      break;

    case kOsc2Enabled:
      m_osc[1].m_bActive = (value == 1) ? true : false;
      break;

    case kOsc1WaveType:
      m_osc[0].SetWaveType(static_cast<WaveType>(value), m_defaultWavetables);
      break;

    case kOsc2WaveType:
      m_osc[1].SetWaveType(static_cast<WaveType>(value), m_defaultWavetables);
      break;

    case kOsc1Gain:
      m_osc[0].m_dGain = value / 100.0;
      break;

    case kOsc2Gain:
      m_osc[1].m_dGain = value / 100.0;
      break;

    case kOsc1Pan:
      m_osc[0].m_dPan = value / 100.0;
      break;

    case kOsc2Pan:
      m_osc[1].m_dPan = value / 100.0;
      break;

    case kOsc1OffsetOctave:
      if (abs(value) < 0.01)
        m_osc[0].m_dFreqMultiplier = 1;
      else
        m_osc[0].m_dFreqMultiplier = pow(2.0, value);

      break;

    case kOsc2OffsetOctave:
      if (abs(value) < 0.01)
        m_osc[1].m_dFreqMultiplier = 1;
      else
        m_osc[1].m_dFreqMultiplier = pow(2.0, value);

      break;

    case kEnvAttack:
      m_envelope.SetAttack(value);
      break;

    case kEnvDecay:
      m_envelope.SetDecay(value);
      break;

    case kEnvSustain:
      m_envelope.SetSustain(value / 100.0);
      break;

    case kEnvRelease:
      m_envelope.SetRelease(value);
      break;

    case kLFOEnabled:
      m_LFO.m_bEnabled = (value == 1) ? true : false;
      break;

    case kLFOMode:
      m_LFO.m_mode = static_cast<LFOMode>(value);
      break;

    case kLFOAmp:
      m_LFO.m_dAmplitude = value / 100.0;
      break;

    case kLFORate:
      m_LFO.m_dFreq = value;
      break;
    }
  }

  void SetSampleRate(int rate)
  {
    m_iSampleRate = rate;
  }

private:
  int m_iSampleRate = 44100;
  Voice m_voices[NUM_VOICES];
  int m_iNextVoice = 0;

  KOscillator m_osc[NUM_OSCILLATORS];
  DefaultWavetables m_defaultWavetables;
  Envelope m_envelope;
  LFO m_LFO;
};