#pragma once

using namespace iplug;

class KOscillator
{
public:
  double* Process(double inputFreq, int sampleRate, double time)
  {
    double out[2] = { 0.0 };
    double freq = inputFreq * m_oscParams->m_dFreqMultiplier;
    Wavetable *wt = m_oscParams->m_wavetable;

    if (m_oscParams->m_waveType == WaveType::NOISE)
    {
      out[1] = out[0] = OscNoise();
    }
    else
    {
      // Apply unison and detune
      for (int i = 0; i < m_oscParams->m_iUnisonAmount + 1; i++)
      {
        double unisonFreq = freq * std::pow(2.0, m_oscParams->m_dUnisonDetune * m_oscParams->m_unisonDetuneFactors[i] * 100.0 / 1200.0);

        double increment = unisonFreq * wt->m_iWavetableSize / sampleRate;
        m_dPhase[i] = std::fmod(m_dPhase[i] + increment, wt->m_iWavetableSize);

        double wtData[2];
        uint32_t readIndex[2];

        double intPart;
        double fracPart = std::modf(m_dPhase[i], &intPart);
        readIndex[0] = (uint32_t) intPart;
        readIndex[1] = (readIndex[0] + 1) & wt->m_wrapMask;

        wtData[0] = wt->GetWavetable()[readIndex[0]];
        wtData[1] = wt->GetWavetable()[readIndex[1]];

        // Linearly interpolate the two table values
        double wtOut = (wtData[1] - wtData[0]) * fracPart + wtData[0];

        // Pan (creates stereo effect with detuned unison)
        out[0] += wtOut * std::min(1.0, 1.0 - m_oscParams->m_unisonPanValues[i]);
        out[1] += wtOut * std::min(1.0, 1.0 + m_oscParams->m_unisonPanValues[i]);
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