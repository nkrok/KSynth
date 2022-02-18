#pragma once

using namespace iplug;

class KOscillator
{
public:
  double* Process(double inputFreq, int sampleRate, double time)
  {
    double out[2] = { 0.0 };
    double freq = inputFreq * m_oscParams->m_dFreqMultiplier;

    if (m_oscParams->m_waveType == WaveType::NOISE)
    {
      out[1] = out[0] = OscNoise();
    }
    else
    {
      // Apply unison and detune
      for (int i = 0; i < m_oscParams->m_iUnisonAmount + 1; i++)
      {
        double unisonFreq = freq * pow(2.0, m_oscParams->m_dUnisonDetune * (100.0 / std::max(1, m_oscParams->m_iUnisonAmount) * i) / 1200.0);
        double increment = unisonFreq * m_oscParams->m_wavetable->m_iWavetableSize / sampleRate;
        double phase = m_dPhase[i];
        m_dPhase[i] = std::fmod(phase + increment, m_oscParams->m_wavetable->m_iWavetableSize);

        double wtOut = m_oscParams->m_wavetable->GetWavetable()[(int)phase];
        out[0] += wtOut / (m_oscParams->m_iUnisonAmount + 1);
        out[1] += wtOut / (m_oscParams->m_iUnisonAmount + 1);
      }
    }

    // Apply gain and panning
    out[0] *= m_oscParams->m_dGain * std::min(1.0, 1.0 - m_oscParams->m_dPan);
    out[1] *= m_oscParams->m_dGain * std::min(1.0, 1.0 + m_oscParams->m_dPan);

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

  void SetOscParams(std::shared_ptr<OscParams> params)
  {
    m_oscParams = params;
  }

public:
  double m_dPhase[MAX_UNISON] = { 0.0 };

private:
  std::shared_ptr<OscParams> m_oscParams;
};