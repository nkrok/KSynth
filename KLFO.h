#pragma once

class LFO
{
public:
  LFO()
  {
    m_mode = LFOMode::LFO_VOLUME;
    m_bEnabled = false;
    m_dAmplitude = 0.0;
    m_dFreq = 0.0;
  }

public:
  LFOMode m_mode;
  bool m_bEnabled;
  double m_dAmplitude;
  double m_dFreq;
};