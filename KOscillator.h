#pragma once

#include "Wavetable.h"

using namespace iplug;

#define WAVE_TYPE_NAMES "sine", "square", "triangle", "saw", "noise"

enum class WaveType {
  SINE,
  SQUARE,
  TRIANGLE,
  SAW,
  NOISE,
  NUM_WAVE_TYPES
};

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

    for (int i = 0; i < NUM_OSCILLATORS; i++)
      for (int k = 0; k < MAX_UNISON; k++)
        m_dPhase[i][k] = 0.0;

    m_dTriggerOnTime = 0.0;
    m_dActiveTime = 0.0;
    m_dPrevAmplitude = 0.0;
  }

  double GetAmplitude(double time, Envelope& env) {
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

    for (int i = 0; i < NUM_OSCILLATORS; i++)
      for (int k = 0; k < MAX_UNISON; k++)
        m_dPhase[i][k] = 0.0;

    m_dTriggerOnTime = time;
    m_dActiveTime = 0.0;
    m_dPrevAmplitude = 0.0;
  }

  void Release(double time)
  {
    m_bActive = false;
  }

  /*
  * Forcefully stops a voice from making any noise
  */
  void Stop()
  {
    m_bDormant = true;
    m_bActive = false;
  }

public:
  bool m_bDormant;
  int m_iNoteNum;
  bool m_bActive;
  double m_dFreq;
  double m_dPhase[NUM_OSCILLATORS][MAX_UNISON];
  double m_dVel;

  double m_dTriggerOnTime;
  double m_dActiveTime;
  double m_dPrevAmplitude;
};

class KOscillator
{
public:
  KOscillator()
  {
    m_bActive = false;
    m_dGain = 0.0;
    m_dPan = 0.0;
    m_dFreqMultiplier = 1.0;

    m_iUnisonAmount = 0;
    m_dUnisonDetune = 0.0;
  }

  double* Process(Voice &voice, int sampleRate, double time, LFO &lfo)
  {
    double out[2] = { 0.0 };
    double freq = voice.m_dFreq * m_dFreqMultiplier;
    double volOsc = 0.0;

    if (lfo.m_bEnabled)
    {
      if (lfo.m_mode == LFOMode::LFO_PITCH)
      {
        freq *= pow(2.0, OscSin(lfo.m_dFreq, time) * lfo.m_dAmplitude);
      }
      else if (lfo.m_mode == LFOMode::LFO_VOLUME)
      {
        volOsc = abs(OscSin(lfo.m_dFreq, time) * lfo.m_dAmplitude);
      }
    }

    if (m_waveType == WaveType::NOISE)
    {
      out[1] = out[0] = OscNoise();
    }
    else
    {
      // Apply unison and detune
      for (int i = 0; i < m_iUnisonAmount + 1; i++)
      {
        double unisonFreq = freq * pow(2.0, m_dUnisonDetune * (50.0 / std::max(1, m_iUnisonAmount) * i) / 1200.0);
        double increment = unisonFreq * m_wavetable->m_iWavetableSize / sampleRate;
        double phase = voice.m_dPhase[m_iOscID][i];
        voice.m_dPhase[m_iOscID][i] = std::fmod(phase + increment, m_wavetable->m_iWavetableSize);

        double wtOut = m_wavetable->GetWavetable()[(int)phase];
        out[0] += wtOut / (m_iUnisonAmount + 1);
        out[1] += wtOut / (m_iUnisonAmount + 1);
      }
    }

    out[0] -= out[0] * volOsc;
    out[1] -= out[1] * volOsc;

    // Apply gain and panning
    out[0] *= m_dGain * std::min(1.0, 1.0 - m_dPan);
    out[1] *= m_dGain * std::min(1.0, 1.0 + m_dPan);

    return out;
  }

  double OscSin(double freq, double time)
  {
    return std::sin(TWOPI * freq * time);
  }

  double OscNoise()
  {
    return 2.0 * ((double)std::rand() / RAND_MAX) - 1.0;
  }

  void SetWaveType(WaveType type, DefaultWavetables &wavetables)
  {
    m_waveType = type;

    switch (m_waveType)
    {
    case WaveType::SINE:
      m_wavetable = &wavetables.GetSineWavetable();
      break;

    case WaveType::SQUARE:
      m_wavetable = &wavetables.GetSquareWavetable();
      break;

    case WaveType::TRIANGLE:
      m_wavetable = &wavetables.GetTriangleWavetable();
      break;

    case WaveType::SAW:
      m_wavetable = &wavetables.GetSawWavetable();
      break;

    case WaveType::NOISE:
      m_wavetable = &wavetables.GetSineWavetable();
      break;

    default:
      m_wavetable = &wavetables.GetSineWavetable();
      break;
    }
  }

  Wavetable* GetWavetable()
  {
    return m_wavetable;
  }

public:
  int m_iOscID;
  bool m_bActive;
  double m_dGain;
  double m_dPan;
  double m_dFreqMultiplier;

  int m_iUnisonAmount;
  double m_dUnisonDetune;

private:
  WaveType m_waveType;
  Wavetable *m_wavetable;
};