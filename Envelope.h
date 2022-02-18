#pragma once

class Envelope
{
public:
  Envelope()
  {
    SetAttack(0.0);
    SetDecay(0.0);
    SetSustain(1.0);
    SetRelease(0.0);
  }

  void SetAttack(double value)
  {
    m_dAttack = value + 0.005; // Add 0.01 to attack so it doesn't pop when the value is 0
  }

  void SetDecay(double value)
  {
    m_dDecay = value + 0.01; // Add 0.01 to decay so we're never dividing by zero during GetAmplitude()
  }

  void SetSustain(double value)
  {
    m_dSustain = value;
    m_dSustainInverse = 1.0 - value;
  }

  void SetRelease(double value)
  {
    m_dRelease = value + 0.01; // Add 0.01 to release so it doesn't pop when the value is 0
  }

  /*
  activeTime is the duration that the note has been active
  releaseTime is the duration since the note has been released
  */
  double GetAmplitude(bool noteActive, double activeTime, double releaseTime, double prevAmplitude)
  {
    if (noteActive)
    {
      if (activeTime < m_dAttack) // Attack
      {
        return activeTime / (m_dAttack);
      }
      else if (activeTime < m_dAttack + m_dDecay) // Decay
      {
        return 1.0 - m_dSustainInverse * ((activeTime - m_dAttack) / m_dDecay);
      }
      else // Sustain
      {
        return m_dSustain;
      }
    }
    else // Release
    {
      return prevAmplitude * (1 - releaseTime / m_dRelease);
    }
  }

private:
  double m_dAttack;
  double m_dDecay;
  double m_dSustain;
  double m_dSustainInverse;
  double m_dRelease;
};