#include "effects.h"

void EffectReverb(GUI::Context* UI, Effects::Reverb* Base)
{
	UI->GetElementById(0, "cmp_audio_source_reverb_lrp_x").CastFormFloat(&Base->LateReverbPan.X);
	UI->GetElementById(0, "cmp_audio_source_reverb_lrp_y").CastFormFloat(&Base->LateReverbPan.Y);
	UI->GetElementById(0, "cmp_audio_source_reverb_lrp_z").CastFormFloat(&Base->LateReverbPan.Z);
	UI->GetElementById(0, "cmp_audio_source_reverb_rp_x").CastFormFloat(&Base->ReflectionsPan.X);
	UI->GetElementById(0, "cmp_audio_source_reverb_rp_y").CastFormFloat(&Base->ReflectionsPan.Y);
	UI->GetElementById(0, "cmp_audio_source_reverb_rp_z").CastFormFloat(&Base->ReflectionsPan.Z);
	UI->GetElementById(0, "cmp_audio_source_reverb_dens").CastFormFloat(&Base->Density);
	UI->GetElementById(0, "cmp_audio_source_reverb_diff").CastFormFloat(&Base->Diffusion);
	UI->GetElementById(0, "cmp_audio_source_reverb_gain").CastFormFloat(&Base->Gain);
	UI->GetElementById(0, "cmp_audio_source_reverb_gain_hf").CastFormFloat(&Base->GainHF);
	UI->GetElementById(0, "cmp_audio_source_reverb_gain_lf").CastFormFloat(&Base->GainLF);
	UI->GetElementById(0, "cmp_audio_source_reverb_dectm").CastFormFloat(&Base->DecayTime);
	UI->GetElementById(0, "cmp_audio_source_reverb_dechfr").CastFormFloat(&Base->DecayHFRatio);
	UI->GetElementById(0, "cmp_audio_source_reverb_declfr").CastFormFloat(&Base->DecayLFRatio);
	UI->GetElementById(0, "cmp_audio_source_reverb_rgain").CastFormFloat(&Base->ReflectionsGain);
	UI->GetElementById(0, "cmp_audio_source_reverb_rdec").CastFormFloat(&Base->ReflectionsDelay);
	UI->GetElementById(0, "cmp_audio_source_reverb_etime").CastFormFloat(&Base->EchoTime);
	UI->GetElementById(0, "cmp_audio_source_reverb_edepth").CastFormFloat(&Base->EchoDepth);
	UI->GetElementById(0, "cmp_audio_source_reverb_mtime").CastFormFloat(&Base->ModulationTime);
	UI->GetElementById(0, "cmp_audio_source_reverb_edepth").CastFormFloat(&Base->ModulationDepth);
	UI->GetElementById(0, "cmp_audio_source_reverb_aaghf").CastFormFloat(&Base->AirAbsorptionGainHF);
	UI->GetElementById(0, "cmp_audio_source_reverb_hfref").CastFormFloat(&Base->HFReference);
	UI->GetElementById(0, "cmp_audio_source_reverb_lfref").CastFormFloat(&Base->LFReference);
	UI->GetElementById(0, "cmp_audio_source_reverb_rro").CastFormFloat(&Base->RoomRolloffFactor);
	UI->GetElementById(0, "cmp_audio_source_reverb_dhfl").CastFormBoolean(&Base->IsDecayHFLimited);
}
void EffectChorus(GUI::Context* UI, Effects::Chorus* Base)
{
	UI->GetElementById(0, "cmp_audio_source_chorus_rate").CastFormFloat(&Base->Rate);
	UI->GetElementById(0, "cmp_audio_source_chorus_depth").CastFormFloat(&Base->Depth);
	UI->GetElementById(0, "cmp_audio_source_chorus_fb").CastFormFloat(&Base->Feedback);
	UI->GetElementById(0, "cmp_audio_source_chorus_delay").CastFormFloat(&Base->Delay);
	UI->GetElementById(0, "cmp_audio_source_chorus_wform").CastFormInt32(&Base->Waveform);
	UI->GetElementById(0, "cmp_audio_source_chorus_phase").CastFormInt32(&Base->Phase);
}
void EffectDistortion(GUI::Context* UI, Effects::Distortion* Base)
{
	UI->GetElementById(0, "cmp_audio_source_distortion_edge").CastFormFloat(&Base->Edge);
	UI->GetElementById(0, "cmp_audio_source_distortion_gain").CastFormFloat(&Base->Gain);
	UI->GetElementById(0, "cmp_audio_source_distortion_lco").CastFormFloat(&Base->LowpassCutOff);
	UI->GetElementById(0, "cmp_audio_source_distortion_eqc").CastFormFloat(&Base->EQCenter);
	UI->GetElementById(0, "cmp_audio_source_distortion_eqb").CastFormFloat(&Base->EQBandwidth);
}
void EffectEcho(GUI::Context* UI, Effects::Echo* Base)
{
	UI->GetElementById(0, "cmp_audio_source_echo_delay").CastFormFloat(&Base->Delay);
	UI->GetElementById(0, "cmp_audio_source_echo_lrdelay").CastFormFloat(&Base->LRDelay);
	UI->GetElementById(0, "cmp_audio_source_echo_damping").CastFormFloat(&Base->Damping);
	UI->GetElementById(0, "cmp_audio_source_echo_fb").CastFormFloat(&Base->Feedback);
	UI->GetElementById(0, "cmp_audio_source_echo_spread").CastFormFloat(&Base->Spread);
}
void EffectFlanger(GUI::Context* UI, Effects::Flanger* Base)
{
	UI->GetElementById(0, "cmp_audio_source_flanger_rate").CastFormFloat(&Base->Rate);
	UI->GetElementById(0, "cmp_audio_source_flanger_depth").CastFormFloat(&Base->Depth);
	UI->GetElementById(0, "cmp_audio_source_flanger_fb").CastFormFloat(&Base->Feedback);
	UI->GetElementById(0, "cmp_audio_source_flanger_delay").CastFormFloat(&Base->Delay);
	UI->GetElementById(0, "cmp_audio_source_flanger_wform").CastFormInt32(&Base->Waveform);
	UI->GetElementById(0, "cmp_audio_source_flanger_phase").CastFormInt32(&Base->Phase);
}
void EffectFrequencyShifter(GUI::Context* UI, Effects::FrequencyShifter* Base)
{
	UI->GetElementById(0, "cmp_audio_source_frequency_shifter_fq").CastFormFloat(&Base->Frequency);
	UI->GetElementById(0, "cmp_audio_source_frequency_shifter_ld").CastFormInt32(&Base->LeftDirection);
	UI->GetElementById(0, "cmp_audio_source_frequency_shifter_rd").CastFormInt32(&Base->RightDirection);
}
void EffectVocalMorpher(GUI::Context* UI, Effects::VocalMorpher* Base)
{
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_rate").CastFormFloat(&Base->Rate);
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_pa").CastFormInt32(&Base->Phonemea);
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_pac").CastFormInt32(&Base->PhonemeaCoarseTuning);
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_pb").CastFormInt32(&Base->Phonemeb);
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_pbc").CastFormInt32(&Base->PhonemebCoarseTuning);
	UI->GetElementById(0, "cmp_audio_source_vocal_morpher_wform").CastFormInt32(&Base->Waveform);
}
void EffectPitchShifter(GUI::Context* UI, Effects::PitchShifter* Base)
{
	UI->GetElementById(0, "cmp_audio_source_pitch_shifter_ct").CastFormInt32(&Base->CoarseTune);
	UI->GetElementById(0, "cmp_audio_source_pitch_shifter_ft").CastFormInt32(&Base->FineTune);
}
void EffectRingModulator(GUI::Context* UI, Effects::RingModulator* Base)
{
	UI->GetElementById(0, "cmp_audio_source_ring_modulator_fq").CastFormFloat(&Base->Frequency);
	UI->GetElementById(0, "cmp_audio_source_ring_modulator_hco").CastFormFloat(&Base->HighpassCutOff);
	UI->GetElementById(0, "cmp_audio_source_ring_modulator_wform").CastFormInt32(&Base->Waveform);
}
void EffectAutowah(GUI::Context* UI, Effects::Autowah* Base)
{
	UI->GetElementById(0, "cmp_audio_source_autowah_at").CastFormFloat(&Base->AttackTime);
	UI->GetElementById(0, "cmp_audio_source_autowah_rt").CastFormFloat(&Base->ReleaseTime);
	UI->GetElementById(0, "cmp_audio_source_autowah_r").CastFormFloat(&Base->Resonance);
	UI->GetElementById(0, "cmp_audio_source_autowah_pg").CastFormFloat(&Base->PeakGain);
}
void EffectEqualizer(GUI::Context* UI, Effects::Equalizer* Base)
{
	UI->GetElementById(0, "cmp_audio_source_equalizer_lg").CastFormFloat(&Base->LowGain);
	UI->GetElementById(0, "cmp_audio_source_equalizer_lco").CastFormFloat(&Base->LowCutOff);
	UI->GetElementById(0, "cmp_audio_source_equalizer_hg").CastFormFloat(&Base->HighGain);
	UI->GetElementById(0, "cmp_audio_source_equalizer_hco").CastFormFloat(&Base->HighCutOff);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m1g").CastFormFloat(&Base->Mid1Gain);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m1c").CastFormFloat(&Base->Mid1Center);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m1w").CastFormFloat(&Base->Mid1Width);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m2g").CastFormFloat(&Base->Mid2Gain);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m2c").CastFormFloat(&Base->Mid2Center);
	UI->GetElementById(0, "cmp_audio_source_equalizer_m2w").CastFormFloat(&Base->Mid2Width);
}