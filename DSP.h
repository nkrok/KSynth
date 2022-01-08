#pragma once

#define NUM_VOICES 16
#define NUM_OSCILLATORS 2

#include "KLFO.h"
#include "KOscillator.h"
#include "Envelope.h"

class DSP
{
public:
  class Voice
  {
  public:
    Voice()
    {
      m_bDormant = true;
      m_bActive = false;
      m_iNoteNum = 69;
      m_dFreq = 440.0;
      m_dVel = 0.0;

      m_dTriggerOnTime = 0.0;
      m_dActiveTime = 0.0;
      m_dPrevAmplitude = 0.0;
    }

    double GetAmplitude(double time, Envelope &env) {
      double dAmp = env.GetAmplitude(m_bActive, m_dActiveTime, time - m_dTriggerOnTime - m_dActiveTime, m_dPrevAmplitude);

      if (m_bActive)
      {
        m_dActiveTime = time - m_dTriggerOnTime;
        m_dPrevAmplitude = dAmp;
      }
      else
      {
        if (dAmp < 0.001)
        {
          m_bDormant = true;
          return 0.0;
        }
      }

      return dAmp * m_dVel;
    }

    void Start(int noteNum, double velocity, double time)
    {
      m_bDormant = false;
      m_iNoteNum = noteNum;
      m_dFreq = 440.0 * pow(2.0, (noteNum - 69) / 12.0);
      m_bActive = true;
      m_dVel = velocity;

      m_dTriggerOnTime = time;
      m_dActiveTime = 0.0;
      m_dPrevAmplitude = 0.0;
    }

    void Release(double time)
    {
      m_bActive = false;
    }

  public:
    bool m_bDormant;
    int m_iNoteNum;
    bool m_bActive;
    double m_dFreq;
    double m_dVel;

    double m_dTriggerOnTime;
    double m_dActiveTime;
    double m_dPrevAmplitude;
  };

public:
  DSP()
  {
    //NoteOn(64, 0.5, 0);
  }

  double Process(double time)
  {
    double out = 0.0;

    for (int i = 0; i < NUM_VOICES; i++)
    {
      if (m_voices[i].m_bDormant)
        continue;

      double dAmp = m_voices[i].GetAmplitude(time, m_envelope);

      if (dAmp > 0.01)
      {
        for (int k = 0; k < NUM_OSCILLATORS; k++)
        {
          if (m_osc[k].m_bActive)
            out += m_osc[k].Process(m_voices[i].m_dFreq, time, m_LFO) * m_osc[k].m_dGain * dAmp;
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
      m_osc[0].SetWaveType(static_cast<WaveType>(value));
      break;

    case kOsc2WaveType:
      m_osc[1].SetWaveType(static_cast<WaveType>(value));
      break;

    case kOsc1Gain:
      m_osc[0].m_dGain = value / 100.0;
      break;

    case kOsc2Gain:
      m_osc[1].m_dGain = value / 100.0;
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
    }
  }

private:
  Voice m_voices[NUM_VOICES];
  int m_iNextVoice = 0;

  KOscillator m_osc[NUM_OSCILLATORS];
  Envelope m_envelope;
  LFO m_LFO;
};