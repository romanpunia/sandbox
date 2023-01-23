#ifndef EFFECTS_H
#define EFFECTS_H
#include "../core/globals.h"

extern void EffectReverb(GUI::Context* UI, Effects::Reverb* Base);
extern void EffectChorus(GUI::Context* UI, Effects::Chorus* Base);
extern void EffectDistortion(GUI::Context* UI, Effects::Distortion* Base);
extern void EffectEcho(GUI::Context* UI, Effects::Echo* Base);
extern void EffectFlanger(GUI::Context* UI, Effects::Flanger* Base);
extern void EffectFrequencyShifter(GUI::Context* UI, Effects::FrequencyShifter* Base);
extern void EffectVocalMorpher(GUI::Context* UI, Effects::VocalMorpher* Base);
extern void EffectPitchShifter(GUI::Context* UI, Effects::PitchShifter* Base);
extern void EffectRingModulator(GUI::Context* UI, Effects::RingModulator* Base);
extern void EffectAutowah(GUI::Context* UI, Effects::Autowah* Base);
extern void EffectEqualizer(GUI::Context* UI, Effects::Equalizer* Base);
#endif