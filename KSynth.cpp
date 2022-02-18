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
  GetParam(kOsc1WaveType)->InitEnum("Osc 1 Wavetype", 0, { WAVE_TYPE_NAMES });
  GetParam(kOsc2WaveType)->InitEnum("Osc 2 Wavetype", 1, { WAVE_TYPE_NAMES });
  GetParam(kOsc1Gain)->InitDouble("Osc 1 Gain", 100.0, 0.0, 100.0, 0.01);
  GetParam(kOsc2Gain)->InitDouble("Osc 2 Gain", 0.0, 0.0, 100.0, 0.01);
  GetParam(kOsc1OffsetOctave)->InitInt("Osc 1 Octave Offset", 0, -3, 3);
  GetParam(kOsc2OffsetOctave)->InitInt("Osc 2 Octave Offset", 0, -3, 3);
  GetParam(kOsc1Pan)->InitDouble("Osc 1 Panning", 0, -100.0, 100.0, 0.01);
  GetParam(kOsc2Pan)->InitDouble("Osc 2 Panning", 0, -100.0, 100.0, 0.01);
  GetParam(kOsc1UnisonAmount)->InitInt("Osc 1 Unison", 0, 0, MAX_UNISON - 1);
  GetParam(kOsc2UnisonAmount)->InitInt("Osc 2 Unison", 0, 0, MAX_UNISON - 1);
  GetParam(kOsc1UnisonDetune)->InitDouble("Osc 1 Detune", 0.0, 0.0, 100.0, 0.01);
  GetParam(kOsc2UnisonDetune)->InitDouble("Osc 2 Detune", 0.0, 0.0, 100.0, 0.01);

  GetParam(kEnvAttack)->InitDouble("Amp Env Attack", 0.0, 0.0, 5.0, 0.01);
  GetParam(kEnvDecay)->InitDouble("Amp Env Decay", 1.0, 0.0, 5.0, 0.01);
  GetParam(kEnvSustain)->InitDouble("Amp Env Sustain", 100.0, 0.0, 100.0, 0.01);
  GetParam(kEnvRelease)->InitDouble("Amp Env Release", 0.0, 0.0, 5.0, 0.01);

  GetParam(kLFO1Enabled)->InitBool("LFO Enabled", false);
  GetParam(kLFO1Mode)->InitEnum("LFO Mode", 0, { LFO_MODE_NAMES });
  GetParam(kLFO1Amp)->InitDouble("LFO Amplitude", 0.0, 0.0, 100.0, 0.01);
  GetParam(kLFO1Rate)->InitDouble("LFO Rate", 0.0, 0.0, 20.0, 0.01);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachBackground(PNG_SKIN);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IRECT b = pGraphics->GetBounds();

    // Load images
    const IBitmap bmpWaveTypePanel = pGraphics->LoadBitmap(PNG_WAVETYPES, 5);
    const IBitmap bmpKnob = pGraphics->LoadBitmap(PNG_KNOB);
    const IBitmap bmpButtonToggle = pGraphics->LoadBitmap(PNG_BUTTON_TOGGLE, 2);
    const IBitmap bmpLFOStates = pGraphics->LoadBitmap(PNG_LFO_STATES, 2);

    /* Attach controls */

    pGraphics->AttachControl(new IBKnobRotaterControl(8, 8, bmpKnob, kGain));
    AddText(pGraphics, 25, 46, TEXT_LIGHT, "Main");

    // Volume meter
    pGraphics->AttachControl(new IVLEDMeterControl<2>(IRECT(PLUG_WIDTH - 30, 64, PLUG_WIDTH - 22, 128)), kCtrlTagMeter);

    // Oscillator parameters
    pGraphics->AttachControl(new IBSwitchControl(49, 63, bmpButtonToggle, kOsc1Enabled));
    AddText(pGraphics, 30, 70, TEXT_LIGHT_MED, "Osc 1");
    pGraphics->AttachControl(new IBSwitchControl(202, 63, bmpButtonToggle, kOsc2Enabled));
    AddText(pGraphics, 178, 70, TEXT_LIGHT_MED, "Osc 2");
    pGraphics->AttachControl(new IBSwitchControl(11, 136, bmpWaveTypePanel, kOsc1WaveType));
    pGraphics->AttachControl(new IBSwitchControl(161, 136, bmpWaveTypePanel, kOsc2WaveType));
    pGraphics->AttachControl(new IBKnobRotaterControl(10, 80, bmpKnob, kOsc1Gain));
    AddText(pGraphics, 26, 120, TEXT_LIGHT, "Gain");
    pGraphics->AttachControl(new IBKnobRotaterControl(158, 80, bmpKnob, kOsc2Gain));
    AddText(pGraphics, 174, 120, TEXT_LIGHT, "Gain");
    pGraphics->AttachControl(new IBKnobRotaterControl(46, 80, bmpKnob, kOsc1Pan));
    AddText(pGraphics, 62, 120, TEXT_LIGHT, "Pan");
    pGraphics->AttachControl(new IBKnobRotaterControl(194, 80, bmpKnob, kOsc2Pan));
    AddText(pGraphics, 210, 120, TEXT_LIGHT, "Pan");
    pGraphics->AttachControl(new IBKnobRotaterControl(8, 158, bmpKnob, kOsc1OffsetOctave));
    AddText(pGraphics, 62, 173, TEXT_LIGHT, "Octave");
    pGraphics->AttachControl(new IBKnobRotaterControl(158, 158, bmpKnob, kOsc2OffsetOctave));
    AddText(pGraphics, 212, 173, TEXT_LIGHT, "Octave");
    pGraphics->AttachControl(new IBKnobRotaterControl(8, 194, bmpKnob, kOsc1UnisonAmount));
    AddText(pGraphics, 62, 209, TEXT_LIGHT, "Unison");
    pGraphics->AttachControl(new IBKnobRotaterControl(158, 194, bmpKnob, kOsc2UnisonAmount));
    AddText(pGraphics, 212, 209, TEXT_LIGHT, "Unison");
    pGraphics->AttachControl(new IBKnobRotaterControl(8, 230, bmpKnob, kOsc1UnisonDetune));
    AddText(pGraphics, 62, 245, TEXT_LIGHT, "Detune");
    pGraphics->AttachControl(new IBKnobRotaterControl(158, 230, bmpKnob, kOsc2UnisonDetune));
    AddText(pGraphics, 212, 245, TEXT_LIGHT, "Detune");


    // ADSR parameters
    AddText(pGraphics, 38, PLUG_HEIGHT - 64, TEXT_LIGHT_MED, "Envelope");
    pGraphics->AttachControl(new IBKnobRotaterControl(8, PLUG_HEIGHT - 52, bmpKnob, kEnvAttack));
    AddText(pGraphics, 24, PLUG_HEIGHT - 13, TEXT_LIGHT, "A");
    pGraphics->AttachControl(new IBKnobRotaterControl(44, PLUG_HEIGHT - 52, bmpKnob, kEnvDecay));
    AddText(pGraphics, 60, PLUG_HEIGHT - 13, TEXT_LIGHT, "D");
    pGraphics->AttachControl(new IBKnobRotaterControl(80, PLUG_HEIGHT - 52, bmpKnob, kEnvSustain));
    AddText(pGraphics, 96, PLUG_HEIGHT - 13, TEXT_LIGHT, "S");
    pGraphics->AttachControl(new IBKnobRotaterControl(116, PLUG_HEIGHT - 52, bmpKnob, kEnvRelease));
    AddText(pGraphics, 132, PLUG_HEIGHT - 13, TEXT_LIGHT, "R");

    // LFO parameters
    AddText(pGraphics, 22, PLUG_HEIGHT - 174, TEXT_LIGHT_MED, "LFO");
    pGraphics->AttachControl(new IBSwitchControl(10, PLUG_HEIGHT - 154, bmpButtonToggle, kLFO1Enabled));
    pGraphics->AttachControl(new IBSwitchControl(32, PLUG_HEIGHT - 158, bmpLFOStates, kLFO1Mode));
    pGraphics->AttachControl(new IBKnobRotaterControl(8, PLUG_HEIGHT - 128, bmpKnob, kLFO1Amp));
    AddText(pGraphics, 24, PLUG_HEIGHT - 90, TEXT_LIGHT, "Amp");
    pGraphics->AttachControl(new IBKnobRotaterControl(44, PLUG_HEIGHT - 128, bmpKnob, kLFO1Rate));
    AddText(pGraphics, 60, PLUG_HEIGHT - 90, TEXT_LIGHT, "Rate");
  };
#endif
}

#if IPLUG_EDITOR
void KSynth::AddText(IGraphics* pGraphics, int x, int y, IText textProps, char* cString)
{
  IRECT rect(x, y, x, y);
  pGraphics->MeasureText(textProps, cString, rect);
  pGraphics->AttachControl(new ITextControl(rect, cString, textProps));
}
#endif

#if IPLUG_DSP
void KSynth::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;

  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++out1, ++out2)
  {
    double *result = mDSP.Process(m_dGlobalTime);

    *out1 = result[0] * gain;
    *out2 = result[1] * gain;

    m_dGlobalTime += (1.0 / m_iSampleRate);
  }

  // Send output to volume meter
  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
}

void KSynth::OnIdle()
{
  mMeterSender.TransmitData(*this);
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
      mDSP.NoteOn(msg.NoteNumber(), velocity / 127.0, m_dGlobalTime);
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
  mDSP.StopAllVoices();
  m_iSampleRate = GetSampleRate();
  mDSP.SetSampleRate(m_iSampleRate);
  m_dGlobalTime = 0;
}

void KSynth::OnParamChange(int paramIdx)
{
  mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
}
#endif
