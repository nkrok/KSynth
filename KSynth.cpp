#include "KSynth.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

KSynth::KSynth(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  /* Initialize parameters */

  GetParam(kGain)->InitDouble("Gain", 10.0, 0.0, 100.0, 0.01, "%");

  GetParam(kOsc1Enabled)->InitBool("Osc 1 Enabled", true);
  GetParam(kOsc2Enabled)->InitBool("Osc 2 Enabled", false);
  GetParam(kOsc1WaveType)->InitEnum("Osc 1 Wavetype", 0, {WAVE_TYPE_NAMES});
  GetParam(kOsc2WaveType)->InitEnum("Osc 2 Wavetype", 1, { WAVE_TYPE_NAMES });
  GetParam(kOsc1Gain)->InitDouble("Osc 1 Gain", 100.0, 0.0, 100.0, 0.01);
  GetParam(kOsc2Gain)->InitDouble("Osc 2 Gain", 0.0, 0.0, 100.0, 0.01);

  GetParam(kEnvAttack)->InitDouble("Env Attack", 0.0, 0.0, 5.0, 0.01);
  GetParam(kEnvDecay)->InitDouble("Env Decay", 1.0, 0.0, 5.0, 0.01);
  GetParam(kEnvSustain)->InitDouble("Env Sustain", 100.0, 0.0, 100.0, 0.01);
  GetParam(kEnvRelease)->InitDouble("Env Release", 0.0, 0.0, 5.0, 0.01);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_LIGHT_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IRECT b = pGraphics->GetBounds();

    // Load images
    const IBitmap bmpWaveTypePanel = pGraphics->LoadBitmap(PNG_WAVETYPES, 5);
    const IBitmap bmpKnob = pGraphics->LoadBitmap(PNG_KNOB);
    const IBitmap bmpButtonToggle = pGraphics->LoadBitmap(PNG_BUTTON_TOGGLE, 2);

    /* Attach controls */

    pGraphics->AttachControl(new IBKnobRotaterControl(8, 12, bmpKnob, kGain));

    pGraphics->AttachControl(new IBSwitchControl(8, 74, bmpButtonToggle, kOsc1Enabled));
    pGraphics->AttachControl(new IBSwitchControl(158, 74, bmpButtonToggle, kOsc2Enabled));
    pGraphics->AttachControl(new IBSwitchControl(8, 118, bmpWaveTypePanel, kOsc1WaveType));
    pGraphics->AttachControl(new IBSwitchControl(158, 118, bmpWaveTypePanel, kOsc2WaveType));
    pGraphics->AttachControl(new IBKnobRotaterControl(30, 70, bmpKnob, kOsc1Gain));
    pGraphics->AttachControl(new IBKnobRotaterControl(180, 70, bmpKnob, kOsc2Gain));

    pGraphics->AttachControl(new IBKnobRotaterControl(8, PLUG_HEIGHT - 40, bmpKnob, kEnvAttack));
    pGraphics->AttachControl(new IBKnobRotaterControl(44, PLUG_HEIGHT - 40, bmpKnob, kEnvDecay));
    pGraphics->AttachControl(new IBKnobRotaterControl(80, PLUG_HEIGHT - 40, bmpKnob, kEnvSustain));
    pGraphics->AttachControl(new IBKnobRotaterControl(116, PLUG_HEIGHT - 40, bmpKnob, kEnvRelease));
  };
#endif
}

#if IPLUG_DSP
void KSynth::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;

  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++out1, ++out2)
  {
    double result = mDSP.Process(m_dGlobalTime) * gain;

    *out1 = result;
    *out2 = result;

    m_dGlobalTime += (1.0 / m_iSampleRate);
  }
}

void KSynth::ProcessMidiMsg(const IMidiMsg& msg)
{
  int status = msg.StatusMsg();

  switch (status)
  {
  case IMidiMsg::kControlChange: // Check for changed midi controls
  {
    // Toggle sustain
    if (msg.Channel() == 0 && msg.ControlChangeIdx() == IMidiMsg::kSustainOnOff) {
      // TODO: Toggle sustain
    }

    break;
  }

  case IMidiMsg::kNoteOn: // Handle note on/off messages
  case IMidiMsg::kNoteOff:
  {
    int velocity = msg.Velocity();

    if (status == IMidiMsg::kNoteOn && velocity) {
      mDSP.NoteOn(msg.NoteNumber(), velocity, m_dGlobalTime);
    }
    else {
      mDSP.NoteOff(msg.NoteNumber(), m_dGlobalTime);
    }

    break;
  }

  default:
    return;
  }
}

void KSynth::OnReset()
{
  m_iSampleRate = GetSampleRate();
  m_dGlobalTime = 0;
}

void KSynth::OnParamChange(int paramIdx)
{
  mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
}
#endif
