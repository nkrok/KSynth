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
    m_dFreqMultiplier = 1.0;
  }

  double Process(double freq, double time)
  {
    return m_oscFunc(freq, time);
  }

  double OscSin(double freq, double time)
  {
    return std::sin(TWOPI * freq * time);
  }

  double OscSquare(double freq, double time)
  {
    return (std::sin(TWOPI * freq * time) > 0) ? 1.0 : -1.0;
  }

  double OscTriangle(double freq, double time)
  {
    return std::asin(std::sin(TWOPI * freq * time)) * (2.0 / PI);
  }

  double OscSaw(double freq, double time)
  {
    return (freq * PI * std::fmod(time, 1.0 / freq) - (PI / 2.0)) * (2.0 / PI);
  }

  double OscNoise(double freq, double time)
  {
    return 2.0 * ((double)std::rand() / RAND_MAX) - 1.0;
  }

  void SetWaveType(WaveType type)
  {
    m_waveType = type;

    switch (m_waveType)
    {
    case WaveType::SINE:
      m_oscFunc = std::bind(&KOscillator::OscSin, this, std::placeholders::_1, std::placeholders::_2);
      break;

    case WaveType::SQUARE:
      m_oscFunc = std::bind(&KOscillator::OscSquare, this, std::placeholders::_1, std::placeholders::_2);
      break;

    case WaveType::TRIANGLE:
      m_oscFunc = std::bind(&KOscillator::OscTriangle, this, std::placeholders::_1, std::placeholders::_2);
      break;

    case WaveType::SAW:
      m_oscFunc = std::bind(&KOscillator::OscSaw, this, std::placeholders::_1, std::placeholders::_2);
      break;

    case WaveType::NOISE:
      m_oscFunc = std::bind(&KOscillator::OscNoise, this, std::placeholders::_1, std::placeholders::_2);
      break;

    default:
      m_oscFunc = std::bind(&KOscillator::OscSin, this, std::placeholders::_1, std::placeholders::_2);
      break;
    }
  }

public:
  bool m_bActive;
  double m_dGain;
  double m_dFreqMultiplier;

private:
  WaveType m_waveType;
  std::function<double(double, double)> m_oscFunc;
};