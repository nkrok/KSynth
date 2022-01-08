#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kOsc1Enabled,
  kOsc2Enabled,
  kOsc1WaveType,
  kOsc2WaveType,
  kOsc1Gain,
  kOsc2Gain,
  kOsc1OffsetOctave,
  kOsc2OffsetOctave,

  kEnvAttack,
  kEnvDecay,
  kEnvSustain,
  kEnvRelease,

  kNumParams
};

#if IPLUG_DSP
#include "DSP.h"
#endif

using namespace iplug;
using namespace igraphics;

#if IPLUG_EDITOR
const IText TEXT_LIGHT = IText(DEFAULT_TEXT_SIZE, COLOR_LIGHT_GRAY);
const IText TEXT_LIGHT_MED = IText(18.0F, COLOR_LIGHT_GRAY);
#endif

class KSynth final : public Plugin
{
public:
  KSynth(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  int m_iSampleRate;
  double m_dGlobalTime;

  DSP mDSP;
#endif

#if IPLUG_EDITOR
public:
  void AddText(IGraphics* pGraphics, int x, int y, IText textProps, char* cString);
#endif
};
