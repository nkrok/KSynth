#pragma once

#include "KOscillator.h"

class Voice
{
public:
  Voice()
  {
    m_bDormant = true;
    m_iNoteNum = 69;
    m_dFreq = 440.0;
    m_dVel = 0.0;

    m_bActive = false;
    m_dTriggerOnTime = 0.0;
    m_dActiveTime = 0.0;
    m_dPrevAmplitude = 0.0;
  }

  Voice(std::shared_ptr<SynthParams> synthParams) : Voice()
  {
    SetSynthParams(synthParams);
  }

  double* Process(double time, int sampleRate)
  {
    double out[2] = { 0.0 };
    double dAmp = GetAmplitude(time);

    // TODO: Do LFO here instead of in the oscillator

    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
      if (!m_synthParams->GetOscParams(i)->m_bActive)
        continue;

      double* oscOut = m_osc[i].Process(m_dFreq, sampleRate, time, lfo);
      out[0] += oscOut[0] * dAmp;
      out[1] += oscOut[1] * dAmp;
    }

    return out;
  }

  double GetAmplitude(double time) {
    double dAmp = m_synthParams->ampEnv->GetAmplitude(m_bActive, m_dActiveTime, time - m_dTriggerOnTime - m_dActiveTime, m_dPrevAmplitude);

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
    m_dFreq = 440.0 * std::pow(2.0, (noteNum - 69) / 12.0);
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

  /*
  * Forcefully stops a voice from making any noise
  */
  void Stop()
  {
    m_bDormant = true;
    m_bActive = false;
  }

  void SetSynthParams(std::shared_ptr<SynthParams> params)
  {
    m_synthParams = params;

    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
      m_osc[i].SetOscParams(m_synthParams->GetOscParams(i));
    }
  }

public:
  bool m_bDormant;
  int m_iNoteNum;
  double m_dFreq;
  double m_dVel;

  bool m_bActive;
  double m_dTriggerOnTime;
  double m_dActiveTime;
  double m_dPrevAmplitude;

private:
  std::shared_ptr<SynthParams> m_synthParams;
  KOscillator m_osc[NUM_OSCILLATORS];
  LFO lfo;
};