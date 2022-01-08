#pragma once

enum class LFOMode
{
  LFO_VOLUME,
  LFO_PITCH
};

class LFO
{
public:
  LFO()
  {
    m_mode = LFOMode::LFO_PITCH;
    m_bEnabled = false;
    m_dAmplitude = 0.01;
    m_dFreq = 3.0;
  }

public:
  LFOMode m_mode;
  bool m_bEnabled;
  double m_dAmplitude;
  double m_dFreq;
};