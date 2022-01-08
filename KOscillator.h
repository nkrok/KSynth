#pragma once

const double TWOPI = 4.0 * acos(0);

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

class KOscillator
{
public:
  KOscillator()
  {
    m_waveType = WaveType::SINE;
    m_bActive = false;
    m_dGain = 0.0;
  }

  double Process(double freq, double time)
  {
    switch (m_waveType)
    {
    case WaveType::SINE:
      return std::sin(TWOPI * freq * time);

    case WaveType::SQUARE:
      return (std::sin(TWOPI * freq * time) > 0) ? 1.0 : -1.0;

    case WaveType::TRIANGLE:
      return std::asin(std::sin(TWOPI * freq * time)) * (2.0 / PI);

    case WaveType::SAW:
      return (freq * PI * std::fmod(time, 1.0 / freq) - (PI / 2.0)) * (2.0 / PI);

    case WaveType::NOISE:
      return 2.0 * ((double)std::rand() / RAND_MAX) - 1.0;

    default:
      return std::sin(TWOPI * freq * time);
    }
  }

public:
  bool m_bActive;
  double m_dGain;
  WaveType m_waveType;

  double m_dFreqMultiplier;
};