#ifndef EFFECTS_H
#define EFFECTS_H
#include "../core/globals.h"

extern void effect_reverb(gui::context* ui, effects::reverb* base);
extern void effect_chorus(gui::context* ui, effects::chorus* base);
extern void effect_distortion(gui::context* ui, effects::distortion* base);
extern void effect_echo(gui::context* ui, effects::echo* base);
extern void effect_flanger(gui::context* ui, effects::flanger* base);
extern void effect_frequency_shifter(gui::context* ui, effects::frequency_shifter* base);
extern void effect_vocal_morpher(gui::context* ui, effects::vocal_morpher* base);
extern void effect_pitch_shifter(gui::context* ui, effects::pitch_shifter* base);
extern void effect_ring_modulator(gui::context* ui, effects::ring_modulator* base);
extern void effect_autowah(gui::context* ui, effects::autowah* base);
extern void effect_equalizer(gui::context* ui, effects::equalizer* base);
#endif