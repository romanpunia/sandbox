#include "effects.h"

void effect_reverb(gui::context* ui, effects::reverb* base)
{
	ui->get_element_by_id("cmp_audio_source_reverb_lrp_x").cast_form_float(&base->late_reverb_pan.x);
	ui->get_element_by_id("cmp_audio_source_reverb_lrp_y").cast_form_float(&base->late_reverb_pan.y);
	ui->get_element_by_id("cmp_audio_source_reverb_lrp_z").cast_form_float(&base->late_reverb_pan.z);
	ui->get_element_by_id("cmp_audio_source_reverb_rp_x").cast_form_float(&base->reflections_pan.x);
	ui->get_element_by_id("cmp_audio_source_reverb_rp_y").cast_form_float(&base->reflections_pan.y);
	ui->get_element_by_id("cmp_audio_source_reverb_rp_z").cast_form_float(&base->reflections_pan.z);
	ui->get_element_by_id("cmp_audio_source_reverb_dens").cast_form_float(&base->density);
	ui->get_element_by_id("cmp_audio_source_reverb_diff").cast_form_float(&base->diffusion);
	ui->get_element_by_id("cmp_audio_source_reverb_gain").cast_form_float(&base->gain);
	ui->get_element_by_id("cmp_audio_source_reverb_gain_hf").cast_form_float(&base->gain_hf);
	ui->get_element_by_id("cmp_audio_source_reverb_gain_lf").cast_form_float(&base->gain_lf);
	ui->get_element_by_id("cmp_audio_source_reverb_dectm").cast_form_float(&base->decay_time);
	ui->get_element_by_id("cmp_audio_source_reverb_dechfr").cast_form_float(&base->decay_hf_ratio);
	ui->get_element_by_id("cmp_audio_source_reverb_declfr").cast_form_float(&base->decay_lf_ratio);
	ui->get_element_by_id("cmp_audio_source_reverb_rgain").cast_form_float(&base->reflections_gain);
	ui->get_element_by_id("cmp_audio_source_reverb_rdec").cast_form_float(&base->reflections_delay);
	ui->get_element_by_id("cmp_audio_source_reverb_etime").cast_form_float(&base->echo_time);
	ui->get_element_by_id("cmp_audio_source_reverb_edepth").cast_form_float(&base->echo_depth);
	ui->get_element_by_id("cmp_audio_source_reverb_mtime").cast_form_float(&base->modulation_time);
	ui->get_element_by_id("cmp_audio_source_reverb_edepth").cast_form_float(&base->modulation_depth);
	ui->get_element_by_id("cmp_audio_source_reverb_aaghf").cast_form_float(&base->air_absorption_gain_hf);
	ui->get_element_by_id("cmp_audio_source_reverb_hfref").cast_form_float(&base->hf_reference);
	ui->get_element_by_id("cmp_audio_source_reverb_lfref").cast_form_float(&base->lf_reference);
	ui->get_element_by_id("cmp_audio_source_reverb_rro").cast_form_float(&base->room_rolloff_factor);
	ui->get_element_by_id("cmp_audio_source_reverb_dhfl").cast_form_boolean(&base->is_decay_hf_limited);
}
void effect_chorus(gui::context* ui, effects::chorus* base)
{
	ui->get_element_by_id("cmp_audio_source_chorus_rate").cast_form_float(&base->rate);
	ui->get_element_by_id("cmp_audio_source_chorus_depth").cast_form_float(&base->depth);
	ui->get_element_by_id("cmp_audio_source_chorus_fb").cast_form_float(&base->feedback);
	ui->get_element_by_id("cmp_audio_source_chorus_delay").cast_form_float(&base->delay);
	ui->get_element_by_id("cmp_audio_source_chorus_wform").cast_form_int32(&base->waveform);
	ui->get_element_by_id("cmp_audio_source_chorus_phase").cast_form_int32(&base->phase);
}
void effect_distortion(gui::context* ui, effects::distortion* base)
{
	ui->get_element_by_id("cmp_audio_source_distortion_edge").cast_form_float(&base->edge);
	ui->get_element_by_id("cmp_audio_source_distortion_gain").cast_form_float(&base->gain);
	ui->get_element_by_id("cmp_audio_source_distortion_lco").cast_form_float(&base->lowpass_cut_off);
	ui->get_element_by_id("cmp_audio_source_distortion_eqc").cast_form_float(&base->eq_center);
	ui->get_element_by_id("cmp_audio_source_distortion_eqb").cast_form_float(&base->eq_bandwidth);
}
void effect_echo(gui::context* ui, effects::echo* base)
{
	ui->get_element_by_id("cmp_audio_source_echo_delay").cast_form_float(&base->delay);
	ui->get_element_by_id("cmp_audio_source_echo_lrdelay").cast_form_float(&base->lr_delay);
	ui->get_element_by_id("cmp_audio_source_echo_damping").cast_form_float(&base->damping);
	ui->get_element_by_id("cmp_audio_source_echo_fb").cast_form_float(&base->feedback);
	ui->get_element_by_id("cmp_audio_source_echo_spread").cast_form_float(&base->spread);
}
void effect_flanger(gui::context* ui, effects::flanger* base)
{
	ui->get_element_by_id("cmp_audio_source_flanger_rate").cast_form_float(&base->rate);
	ui->get_element_by_id("cmp_audio_source_flanger_depth").cast_form_float(&base->depth);
	ui->get_element_by_id("cmp_audio_source_flanger_fb").cast_form_float(&base->feedback);
	ui->get_element_by_id("cmp_audio_source_flanger_delay").cast_form_float(&base->delay);
	ui->get_element_by_id("cmp_audio_source_flanger_wform").cast_form_int32(&base->waveform);
	ui->get_element_by_id("cmp_audio_source_flanger_phase").cast_form_int32(&base->phase);
}
void effect_frequency_shifter(gui::context* ui, effects::frequency_shifter* base)
{
	ui->get_element_by_id("cmp_audio_source_frequency_shifter_fq").cast_form_float(&base->frequency);
	ui->get_element_by_id("cmp_audio_source_frequency_shifter_ld").cast_form_int32(&base->left_direction);
	ui->get_element_by_id("cmp_audio_source_frequency_shifter_rd").cast_form_int32(&base->right_direction);
}
void effect_vocal_morpher(gui::context* ui, effects::vocal_morpher* base)
{
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_rate").cast_form_float(&base->rate);
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_pa").cast_form_int32(&base->phonemea);
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_pac").cast_form_int32(&base->phonemea_coarse_tuning);
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_pb").cast_form_int32(&base->phonemeb);
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_pbc").cast_form_int32(&base->phonemeb_coarse_tuning);
	ui->get_element_by_id("cmp_audio_source_vocal_morpher_wform").cast_form_int32(&base->waveform);
}
void effect_pitch_shifter(gui::context* ui, effects::pitch_shifter* base)
{
	ui->get_element_by_id("cmp_audio_source_pitch_shifter_ct").cast_form_int32(&base->coarse_tune);
	ui->get_element_by_id("cmp_audio_source_pitch_shifter_ft").cast_form_int32(&base->fine_tune);
}
void effect_ring_modulator(gui::context* ui, effects::ring_modulator* base)
{
	ui->get_element_by_id("cmp_audio_source_ring_modulator_fq").cast_form_float(&base->frequency);
	ui->get_element_by_id("cmp_audio_source_ring_modulator_hco").cast_form_float(&base->highpass_cut_off);
	ui->get_element_by_id("cmp_audio_source_ring_modulator_wform").cast_form_int32(&base->waveform);
}
void effect_autowah(gui::context* ui, effects::autowah* base)
{
	ui->get_element_by_id("cmp_audio_source_autowah_at").cast_form_float(&base->attack_time);
	ui->get_element_by_id("cmp_audio_source_autowah_rt").cast_form_float(&base->release_time);
	ui->get_element_by_id("cmp_audio_source_autowah_r").cast_form_float(&base->resonance);
	ui->get_element_by_id("cmp_audio_source_autowah_pg").cast_form_float(&base->peak_gain);
}
void effect_equalizer(gui::context* ui, effects::equalizer* base)
{
	ui->get_element_by_id("cmp_audio_source_equalizer_lg").cast_form_float(&base->low_gain);
	ui->get_element_by_id("cmp_audio_source_equalizer_lco").cast_form_float(&base->low_cut_off);
	ui->get_element_by_id("cmp_audio_source_equalizer_hg").cast_form_float(&base->high_gain);
	ui->get_element_by_id("cmp_audio_source_equalizer_hco").cast_form_float(&base->high_cut_off);
	ui->get_element_by_id("cmp_audio_source_equalizer_m1g").cast_form_float(&base->mid1_gain);
	ui->get_element_by_id("cmp_audio_source_equalizer_m1c").cast_form_float(&base->mid1_center);
	ui->get_element_by_id("cmp_audio_source_equalizer_m1w").cast_form_float(&base->mid1_width);
	ui->get_element_by_id("cmp_audio_source_equalizer_m2g").cast_form_float(&base->mid2_gain);
	ui->get_element_by_id("cmp_audio_source_equalizer_m2c").cast_form_float(&base->mid2_center);
	ui->get_element_by_id("cmp_audio_source_equalizer_m2w").cast_form_float(&base->mid2_width);
}