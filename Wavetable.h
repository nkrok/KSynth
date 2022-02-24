#pragma once

using namespace iplug;

#define WAVETABLE_DEFAULT_SIZE 1024

#define WAVE_TYPE_NAMES "sine", "square", "triangle", "saw", "noise"

typedef double wt_datatype;

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
    m_wavetable = std::make_shared<std::vector<wt_datatype>>(std::vector(size, (wt_datatype)0.0));
    m_wrapMask = size - 1;

    for (int i = 0; i < 128; i++)
    {
      m_pWavetables[i] = m_wavetable;
    }
  }

  std::shared_ptr<std::vector<wt_datatype>> GetWavetable()
  {
    return m_wavetable;
  }

  std::shared_ptr<std::vector<wt_datatype>> GetWavetableForMidiNote(int noteId)
  {
    return m_pWavetables[noteId];
  }

  void SetWavetableForMidiNote(int noteId, std::shared_ptr<std::vector<wt_datatype>> p_wtVector)
  {
    m_pWavetables[noteId] = p_wtVector;
  }

  void ShareWavetables(int srcNote, int destNote)
  {
    m_pWavetables[destNote] = m_pWavetables[srcNote];
  }

public:
  uint32_t m_iWavetableSize;
  uint32_t m_wrapMask;

private:
  std::shared_ptr<std::vector<wt_datatype>> m_wavetable;
  std::shared_ptr<std::vector<wt_datatype>> m_pWavetables[128];
};

class DefaultWavetables
{
public:
  DefaultWavetables(int samplingRate)
  {
    CreateSineWavetable(m_wtSine);
    CreateDefaultWavetable(m_wtSquare, WaveType::SQUARE, samplingRate);
    CreateDefaultWavetable(m_wtTriangle, WaveType::TRIANGLE, samplingRate);
    CreateDefaultWavetable(m_wtSaw, WaveType::SAW, samplingRate);
  }

  void CreateSineWavetable(Wavetable& wt)
  {
    std::vector<wt_datatype> &wt_vector = *(wt.GetWavetable());

    for (int i = 0; i < wt.m_iWavetableSize; i++)
    {
      wt_vector[i] = std::sin(TWOPI * i / wt.m_iWavetableSize);
    }
  }

  std::shared_ptr<std::vector<wt_datatype>> MakeSquareTable(int size, int numHarmonics)
  {
    std::vector<wt_datatype> table = std::vector(size, (wt_datatype)0.0);

    for (int i = 0; i < size; i++)
    {
      for (int k = 0; k < numHarmonics; k++)
      {
        // Lanczos Sigma Factor
        double x = PI * (k + 1.0) / numHarmonics;
        double sigma = sin(x) / x;

        double phi = TWOPI * i * (2*k + 1.0) / size;
        table[i] += (1.0 / (2*k + 1.0)) * sigma * std::sin(phi);
      }
    }

    return std::make_shared<std::vector<wt_datatype>>(table);
  }

  std::shared_ptr<std::vector<wt_datatype>> MakeTriangleTable(int size, int numHarmonics)
  {
    std::vector<wt_datatype> table = std::vector(size, (wt_datatype)0.0);

    for (int i = 0; i < size; i++)
    {
      for (int k = 0; k < numHarmonics; k++)
      {
        // Lanczos Sigma Factor
        double x = PI * (k + 1.0) / numHarmonics;
        double sigma = sin(x) / x;

        double phi = TWOPI * i * (2*k + 1.0) / size;
        table[i] += std::pow(-1.0, k) * (1.0 / std::pow(2*k + 1.0, 2)) * std::sin(phi);
      }
    }

    return std::make_shared<std::vector<wt_datatype>>(table);
  }

  std::shared_ptr<std::vector<wt_datatype>> MakeSawTable(int size, int numHarmonics)
  {
    std::vector<wt_datatype> table = std::vector(size, (wt_datatype)0.0);

    for (int i = 0; i < size; i++)
    {
      for (int k = 1; k <= numHarmonics; k++)
      {
        // Lanczos Sigma Factor
        double x = PI * k / numHarmonics;
        double sigma = sin(x) / x;

        double phi = TWOPI * i * k / size;
        table[i] += std::pow(-1.0, k + 1.0) * (1.0 / k) * sigma * std::sin(phi);
      }
    }

    return std::make_shared<std::vector<wt_datatype>>(table);
  }

  void CreateDefaultWavetable(Wavetable& wt, WaveType waveType, int samplingRate)
  {
    int numHarmonicsPrev = -1;

    for (int i = 0; i < 128; i++)
    {
      double pitchHz = FreqForNote(i);
      int numHarmonics = samplingRate / (2.0 * pitchHz);

      // No. harmonics should not exceed tableSize/2
      if (numHarmonics > (wt.m_iWavetableSize - 1) / 2)
      {
        numHarmonics = (wt.m_iWavetableSize - 1) / 2;
      }

      // If the number of harmonics is the same as the previous note, just reuse the wavetable
      if (numHarmonics == numHarmonicsPrev)
      {
        //wt.SetWavetableForMidiNote(i, wt.GetWavetableForMidiNote(i - 1));
        wt.ShareWavetables(i - 1, i);
        continue;
      }

      numHarmonicsPrev = numHarmonics;

      // Otherwise, make a new table
      std::shared_ptr<std::vector<wt_datatype>> table = nullptr;

      switch (waveType)
      {
      case WaveType::SQUARE:
        table = MakeSquareTable(wt.m_iWavetableSize, numHarmonics);
        break;

      case WaveType::TRIANGLE:
        table = MakeTriangleTable(wt.m_iWavetableSize, numHarmonics);
        break;

      case WaveType::SAW:
        table = MakeSawTable(wt.m_iWavetableSize, numHarmonics);
        break;

      default:
        table = MakeSawTable(wt.m_iWavetableSize, numHarmonics);
        break;
      }

      wt.SetWavetableForMidiNote(i, table);
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

  double FreqForNote(int noteNum)
  {
    return 440.0 * std::pow(2.0, (noteNum - 69) / 12.0);
  }

private:
  Wavetable m_wtSine;
  Wavetable m_wtSquare;
  Wavetable m_wtTriangle;
  Wavetable m_wtSaw;
};