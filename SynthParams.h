#pragma once

#define LFO_MODE_NAMES "volume", "pitch"

enum class LFOMode
{
  LFO_VOLUME,
  LFO_PITCH
};

class OscParams
{
public:
  void SetWavetableAndWaveType(Wavetable* wt, WaveType waveType)
  {
    m_wavetable = wt;
    m_waveType = waveType;
  }

public:
  bool m_bActive = false;
  double m_dGain = 0.0;
  double m_dPan = 0.0;
  double m_dFreqMultiplier = 1.0;

  WaveType m_waveType;
  Wavetable* m_wavetable;

  int m_iUnisonAmount = 0;
  double m_dUnisonDetune = 0.0;

  double m_unisonDetuneFactors[MAX_UNISON] = { 0.0, 1.0, -1.0, 0.707 };
  double m_unisonPanValues[MAX_UNISON] = { 0.0, -1.0, 1.0, -0.5 };
  int m_unisonPhaseOffsets[MAX_UNISON] = { 0, 13, -13, 37 };
};

class LFOParams
{
public:
  LFOMode m_mode = LFOMode::LFO_VOLUME;
  bool m_bEnabled = false;
  double m_dAmplitude = 0.0;
  double m_dFreq = 0.0;
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

    for (int i = 0; i < NUM_LFO; i++)
    {
      lfoParams[i].reset(new LFOParams);
    }
  }

  std::shared_ptr<OscParams> GetOscParams(int oscNum)
  {
    return oscParams[oscNum];
  }

  std::shared_ptr<LFOParams> GetLFOParams(int lfoNum)
  {
    return lfoParams[lfoNum];
  }

public:
  const std::unique_ptr<Envelope> ampEnv = std::make_unique<Envelope>();

private:
  std::shared_ptr<OscParams> oscParams[NUM_OSCILLATORS];
  std::shared_ptr<LFOParams> lfoParams[NUM_LFO];
};