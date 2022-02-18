#pragma once

class OscParams
{
public:
  void SetWavetableAndWaveType(Wavetable* wt, WaveType waveType)
  {
    m_wavetable = wt;
    m_waveType = waveType;
  }

public:
  bool m_bActive;
  double m_dGain;
  double m_dPan;
  double m_dFreqMultiplier;
  int m_iUnisonAmount;
  double m_dUnisonDetune;

  WaveType m_waveType;
  Wavetable* m_wavetable;
};

class SynthParams
{
public:
  SynthParams()
  {
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
      oscParams[i].reset(new OscParams);
    }
  }

  std::shared_ptr<OscParams> GetOscParams(int oscNum)
  {
    return oscParams[oscNum];
  }

public:
  const std::unique_ptr<Envelope> ampEnv = std::make_unique<Envelope>();

private:
  std::shared_ptr<OscParams> oscParams[NUM_OSCILLATORS];
};