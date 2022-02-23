#pragma once

using namespace iplug;

class KOscillator
{
public:
  double* Process(int sampleRate, double time)
  {
    double out[2] = { 0.0 };
    Wavetable *wt = m_oscParams->m_wavetable;

    if (m_oscParams->m_waveType == WaveType::NOISE)
    {
      out[1] = out[0] = OscNoise();
    }
    else
    {
      // Apply unison and detune
      for (int i = 0; i < m_iCurUnisonAmount + 1; i++)
      {
        double increment = m_dFreq[i] * wt->m_iWavetableSize / sampleRate;
        m_dPhase[i] = std::fmod(m_dPhase[i] + increment, wt->m_iWavetableSize);

        double wtData[2];
        uint32_t readIndex[2];

        double intPart;
        double fracPart = std::modf(m_dPhase[i], &intPart);
        readIndex[0] = (uint32_t) intPart;
        readIndex[1] = (readIndex[0] + 1) & wt->m_wrapMask;

        wtData[0] = m_activeWavetableVector[readIndex[0]];
        wtData[1] = m_activeWavetableVector[readIndex[1]];

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

  void NoteOn(double freq)
  {
    SetFrequency(freq);

    int noteNum = 12.0 * std::log2(m_dFreq[0] / 440.0) + 69;
    m_activeWavetableVector = *(m_oscParams->m_wavetable->GetWavetableForMidiNote(noteNum));
  }

  void SetFrequency(double freq)
  {
    freq *= m_oscParams->m_dFreqMultiplier;
    m_iCurUnisonAmount = m_oscParams->m_iUnisonAmount;

    for (int i = 0; i < m_iCurUnisonAmount + 1; i++)
    {
      m_dFreq[i] = freq * std::pow(2.0, m_oscParams->m_dUnisonDetune * m_oscParams->m_unisonDetuneFactors[i] * 100.0 / 1200.0);
    }
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

private:
  double m_dFreq[MAX_UNISON] = { 0.0 };
  double m_dPhase[MAX_UNISON] = { 0.0 };
  double m_dPhaseIncrement[MAX_UNISON] = { 0.0 };
  int m_iCurUnisonAmount = 0;

  std::shared_ptr<OscParams> m_oscParams;
  std::vector<wt_datatype> m_activeWavetableVector;
};