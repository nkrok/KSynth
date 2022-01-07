#define PLUG_NAME "KSynth"
#define PLUG_MFR "nkrok"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID 'ZCOi'
#define PLUG_MFR_ID 'NKRO'
#define PLUG_URL_STR "https://iplug2.github.io"
#define PLUG_EMAIL_STR "nkrok7@gmail.com"
#define PLUG_COPYRIGHT_STR "Copyright 2020 Nathaniel Krok"
#define PLUG_CLASS_NAME KSynth

#define BUNDLE_NAME "KSynth"
#define BUNDLE_MFR "nkrok"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "KSynth"

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 1
#define PLUG_DOES_MIDI_IN 1
#define PLUG_DOES_MIDI_OUT 1
#define PLUG_DOES_MPE 1
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 640
#define PLUG_HEIGHT 480
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY KSynth_Entry
#define AUV2_ENTRY_STR "KSynth_Entry"
#define AUV2_FACTORY KSynth_Factory
#define AUV2_VIEW_CLASS KSynth_View
#define AUV2_VIEW_CLASS_STR "KSynth_View"

#define AAX_TYPE_IDS 'IEF1', 'IEF2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2'
#define AAX_PLUG_MFR_STR "NKRO"
#define AAX_PLUG_NAME_STR "KSynth\nIPEF"
#define AAX_PLUG_CATEGORY_STR "Synth"
#define AAX_DOES_AUDIOSUITE 0

#define VST3_SUBCATEGORY "Instrument|Synth"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define ROBOTO_FN "Roboto-Regular.ttf"

// Image Resources

#define PNG_SKIN "skin.PNG"
#define PNG_WAVETYPES "wavetypes.png"
#define PNG_BUTTON_TOGGLE "button_toggle.png"
#define PNG_KNOB "knob.png"
