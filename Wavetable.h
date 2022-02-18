#pragma once

using namespace iplug;

#define WAVETABLE_DEFAULT_SIZE 1024

#define WAVE_TYPE_NAMES "sine", "square", "triangle", "saw", "noise"

enum class WaveType {
  SINE,
  SQUARE,
  TRIANGLE,
  SAW,
  NOISE,
  NUM_WAVE_TYPES
};

class Wavetable
{
public:
  Wavetable(int size = WAVETABLE_DEFAULT_SIZE)
  {
    m_iWavetableSize = size;
    m_wavetable = std::vector(size, 0.0F);
  }

  std::vector<float>& GetWavetable()
  {
    return m_wavetable;
  }

public:
  int m_iWavetableSize;

private:
  std::vector<float> m_wavetable;
};

class DefaultWavetables
{
public:
  DefaultWavetables()
  {
    CreateSineWavetable(m_wtSine);
    CreateSquareWavetable(m_wtSquare);
    CreateTriangleWavetable(m_wtTriangle);
    CreateSawWavetable(m_wtSaw);
  }

  void CreateSineWavetable(Wavetable& wt)
  {
    std::vector<float>& wt_vector = wt.GetWavetable();

    for (int i = 0; i < wt.m_iWavetableSize; i++)
    {
      wt_vector[i] = std::sin(TWOPI * i / wt.m_iWavetableSize);
    }
  }

  void CreateSquareWavetable(Wavetable& wt)
  {
    std::vector<float>& wt_vector = wt.GetWavetable();

    for (int i = 0; i < wt.m_iWavetableSize; i++)
    {
      wt_vector[i] = (std::sin(TWOPI * i / wt.m_iWavetableSize) > 0) ? 1.0 : -1.0;
    }
  }

  void CreateTriangleWavetable(Wavetable& wt)
  {
    std::vector<float>& wt_vector = wt.GetWavetable();

    for (int i = 0; i < wt.m_iWavetableSize; i++)
    {
      wt_vector[i] = std::asin(std::sin(TWOPI * i / wt.m_iWavetableSize)) * (2.0 / PI);
    }
  }

  void CreateSawWavetable(Wavetable& wt)
  {
    std::vector<float>& wt_vector = wt.GetWavetable();

    for (int i = 0; i < wt.m_iWavetableSize; i++)
    {
      for (int k = 1; k < 100; k++) {
        wt_vector[i] += (std::sin(k * TWOPI * i / wt.m_iWavetableSize)) / k;
      }
    }
  }

  Wavetable& GetSineWavetable()
  {
    return m_wtSine;
  }

  Wavetable& GetSquareWavetable()
  {
    return m_wtSquare;
  }

  Wavetable& GetTriangleWavetable()
  {
    return m_wtTriangle;
  }

  Wavetable& GetSawWavetable()
  {
    return m_wtSaw;
  }

  Wavetable* GetWavetableFromWaveType(WaveType type)
  {
    switch (type)
    {
    case WaveType::SINE:
      return &m_wtSine;

    case WaveType::SQUARE:
      return &m_wtSquare;

    case WaveType::TRIANGLE:
      return &m_wtTriangle;

    case WaveType::SAW:
      return &m_wtSaw;

    case WaveType::NOISE:
      return &m_wtSine;

    default:
      return &m_wtSine;
    }
  }

private:
  Wavetable m_wtSine;
  Wavetable m_wtSquare;
  Wavetable m_wtTriangle;
  Wavetable m_wtSaw;
};