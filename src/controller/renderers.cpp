#include "renderers.h"
#include "resolvers.h"

void renderer_lighting(gui::context* ui, renderers::lighting* base)
{
	resolve_texture_2d(ui, "cmp_camera_lighting_sm", base->get_sky_map() != nullptr, [base](texture_2d* init)
	{
		base->set_sky_map(init);
		memory::release(init);
	}, false);
	resolve_color3(ui, "cmp_camera_lighting_hc", &base->ambient_buffer.high_emission);
	resolve_color3(ui, "cmp_camera_lighting_lc", &base->ambient_buffer.low_emission);
	resolve_color3(ui, "cmp_camera_lighting_sc", &base->ambient_buffer.sky_color);
	resolve_color3(ui, "cmp_camera_lighting_fc", &base->ambient_buffer.fog_color);
	ui->get_element_by_id("cmp_camera_lighting_rd").cast_form_float(&base->shadows.tick.delay);
	ui->get_element_by_id("cmp_camera_lighting_sd").cast_form_float(&base->shadows.distance);
	ui->get_element_by_id("cmp_camera_lighting_ff_x").cast_form_float(&base->ambient_buffer.fog_far.x);
	ui->get_element_by_id("cmp_camera_lighting_ff_y").cast_form_float(&base->ambient_buffer.fog_far.y);
	ui->get_element_by_id("cmp_camera_lighting_ff_z").cast_form_float(&base->ambient_buffer.fog_far.z);
	ui->get_element_by_id("cmp_camera_lighting_fn_x").cast_form_float(&base->ambient_buffer.fog_near.x);
	ui->get_element_by_id("cmp_camera_lighting_fn_y").cast_form_float(&base->ambient_buffer.fog_near.y);
	ui->get_element_by_id("cmp_camera_lighting_fn_z").cast_form_float(&base->ambient_buffer.fog_near.z);
	ui->get_element_by_id("cmp_camera_lighting_fno").cast_form_float(&base->ambient_buffer.fog_near_off);
	ui->get_element_by_id("cmp_camera_lighting_ffo").cast_form_float(&base->ambient_buffer.fog_far_off);
	ui->get_element_by_id("cmp_camera_lighting_fa").cast_form_float(&base->ambient_buffer.fog_amount);
	ui->get_element_by_id("cmp_camera_lighting_se").cast_form_float(&base->ambient_buffer.sky_emission);
	ui->get_element_by_id("cmp_camera_lighting_le").cast_form_float(&base->ambient_buffer.light_emission);

	bool recursive = (base->ambient_buffer.recursive > 0.0f);
	if (ui->get_element_by_id("cmp_camera_lighting_rp").cast_form_boolean(&recursive))
		base->ambient_buffer.recursive = (recursive ? 1.0f : 0.0f);

	uint64_t size = base->surfaces.size;
	if (ui->get_element_by_id("cmp_camera_lighting_r").cast_form_uint64(&size) && size > 0)
		base->set_surface_buffer_size((size_t)size);
}
void renderer_sslr(gui::context* ui, renderers::local_reflections* base)
{
	ui->get_element_by_id("cmp_camera_sslr_sc").cast_form_float(&base->reflectance.samples);
	ui->get_element_by_id("cmp_camera_sslr_i").cast_form_float(&base->reflectance.intensity);
	ui->get_element_by_id("cmp_camera_sslr_d").cast_form_float(&base->reflectance.distance);
	ui->get_element_by_id("cmp_camera_sslr_bc").cast_form_float(&base->gloss.samples);
	ui->get_element_by_id("cmp_camera_sslr_b").cast_form_float(&base->gloss.blur);
	ui->get_element_by_id("cmp_camera_sslr_c").cast_form_float(&base->gloss.cutoff);
	ui->get_element_by_id("cmp_camera_sslr_dz").cast_form_float(&base->gloss.deadzone);
}
void renderer_ssli(gui::context* ui, renderers::local_illumination* base)
{
	ui->get_element_by_id("cmp_camera_ssli_is").cast_form_float(&base->indirection.samples);
	ui->get_element_by_id("cmp_camera_ssli_id").cast_form_float(&base->indirection.distance);
	ui->get_element_by_id("cmp_camera_ssli_ic").cast_form_float(&base->indirection.cutoff);
	ui->get_element_by_id("cmp_camera_ssli_ia").cast_form_float(&base->indirection.attenuation);
	ui->get_element_by_id("cmp_camera_ssli_isw").cast_form_float(&base->indirection.swing);
	ui->get_element_by_id("cmp_camera_ssli_ilbs").cast_form_float(&base->indirection.bias);
	ui->get_element_by_id("cmp_camera_ssli_ilb").cast_form_uint32(&base->bounces);
	ui->get_element_by_id("cmp_camera_ssli_dc").cast_form_float(&base->denoise.cutoff);
	ui->get_element_by_id("cmp_camera_ssli_ds").cast_form_float(&base->denoise.samples);
	ui->get_element_by_id("cmp_camera_ssli_db").cast_form_float(&base->denoise.blur);
}
void renderer_ssla(gui::context* ui, renderers::local_ambient* base)
{
	ui->get_element_by_id("cmp_camera_ssla_sc").cast_form_float(&base->shading.samples);
	ui->get_element_by_id("cmp_camera_ssla_d").cast_form_float(&base->shading.distance);
	ui->get_element_by_id("cmp_camera_ssla_fd").cast_form_float(&base->shading.fade);
	ui->get_element_by_id("cmp_camera_ssla_bs").cast_form_float(&base->shading.bias);
	ui->get_element_by_id("cmp_camera_ssla_i").cast_form_float(&base->shading.intensity);
	ui->get_element_by_id("cmp_camera_ssla_s").cast_form_float(&base->shading.scale);
	ui->get_element_by_id("cmp_camera_ssla_r").cast_form_float(&base->shading.radius);
	ui->get_element_by_id("cmp_camera_ssla_bc").cast_form_float(&base->fibo.samples);
	ui->get_element_by_id("cmp_camera_ssla_blr").cast_form_float(&base->fibo.blur);
	ui->get_element_by_id("cmp_camera_ssla_blp").cast_form_float(&base->fibo.power);
}
void renderer_motion_blur(gui::context* ui, renderers::motion_blur* base)
{
	ui->get_element_by_id("cmp_camera_motionblur_pw").cast_form_float(&base->velocity.power);
	ui->get_element_by_id("cmp_camera_motionblur_th").cast_form_float(&base->velocity.threshold);
	ui->get_element_by_id("cmp_camera_motionblur_sc").cast_form_float(&base->motion.samples);
	ui->get_element_by_id("cmp_camera_motionblur_mn").cast_form_float(&base->motion.motion);
}
void renderer_bloom(gui::context* ui, renderers::bloom* base)
{
	;
	ui->get_element_by_id("cmp_camera_bloom_i").cast_form_float(&base->extraction.intensity);
	ui->get_element_by_id("cmp_camera_bloom_t").cast_form_float(&base->extraction.threshold);
	ui->get_element_by_id("cmp_camera_bloom_bc").cast_form_float(&base->fibo.samples);
	ui->get_element_by_id("cmp_camera_bloom_blr").cast_form_float(&base->fibo.blur);
	ui->get_element_by_id("cmp_camera_bloom_blp").cast_form_float(&base->fibo.power);
}
void renderer_dof(gui::context* ui, renderers::depth_of_field* base)
{
	ui->get_element_by_id("cmp_camera_dof_afd").cast_form_float(&base->distance);
	ui->get_element_by_id("cmp_camera_dof_aft").cast_form_float(&base->time);
	ui->get_element_by_id("cmp_camera_dof_afr").cast_form_float(&base->radius);
	ui->get_element_by_id("cmp_camera_dof_r").cast_form_float(&base->focus.radius);
	ui->get_element_by_id("cmp_camera_dof_b").cast_form_float(&base->focus.bokeh);
	ui->get_element_by_id("cmp_camera_dof_s").cast_form_float(&base->focus.scale);
	ui->get_element_by_id("cmp_camera_dof_nd").cast_form_float(&base->focus.near_distance);
	ui->get_element_by_id("cmp_camera_dof_nr").cast_form_float(&base->focus.near_range);
	ui->get_element_by_id("cmp_camera_dof_fd").cast_form_float(&base->focus.far_distance);
	ui->get_element_by_id("cmp_camera_dof_fr").cast_form_float(&base->focus.far_range);
}
void renderer_tone(gui::context* ui, renderers::tone* base)
{
	ui->get_element_by_id("cmp_camera_tone_gs").cast_form_float(&base->mapping.grayscale);
	ui->get_element_by_id("cmp_camera_tone_aces").cast_form_float(&base->mapping.aces);
	ui->get_element_by_id("cmp_camera_tone_fm").cast_form_float(&base->mapping.filmic);
	ui->get_element_by_id("cmp_camera_tone_ls").cast_form_float(&base->mapping.lottes);
	ui->get_element_by_id("cmp_camera_tone_rh").cast_form_float(&base->mapping.reinhard);
	ui->get_element_by_id("cmp_camera_tone_rh2").cast_form_float(&base->mapping.reinhard2);
	ui->get_element_by_id("cmp_camera_tone_ul").cast_form_float(&base->mapping.unreal);
	ui->get_element_by_id("cmp_camera_tone_uc").cast_form_float(&base->mapping.uchimura);
	ui->get_element_by_id("cmp_camera_tone_ubr").cast_form_float(&base->mapping.ubrightness);
	ui->get_element_by_id("cmp_camera_tone_uct").cast_form_float(&base->mapping.ucontrast);
	ui->get_element_by_id("cmp_camera_tone_ust").cast_form_float(&base->mapping.ustart);
	ui->get_element_by_id("cmp_camera_tone_uln").cast_form_float(&base->mapping.ulength);
	ui->get_element_by_id("cmp_camera_tone_ubl").cast_form_float(&base->mapping.ublack);
	ui->get_element_by_id("cmp_camera_tone_upl").cast_form_float(&base->mapping.upedestal);
	ui->get_element_by_id("cmp_camera_tone_exp").cast_form_float(&base->mapping.exposure);
	ui->get_element_by_id("cmp_camera_tone_eint").cast_form_float(&base->mapping.eintensity);
	ui->get_element_by_id("cmp_camera_tone_egm").cast_form_float(&base->mapping.egamma);
	ui->get_element_by_id("cmp_camera_tone_aint").cast_form_float(&base->mapping.adaptation);
	ui->get_element_by_id("cmp_camera_tone_agr").cast_form_float(&base->mapping.agray);
	ui->get_element_by_id("cmp_camera_tone_awh").cast_form_float(&base->mapping.awhite);
	ui->get_element_by_id("cmp_camera_tone_abl").cast_form_float(&base->mapping.ablack);
	ui->get_element_by_id("cmp_camera_tone_asp").cast_form_float(&base->mapping.aspeed);
}
void renderer_glitch(gui::context* ui, renderers::glitch* base)
{
	ui->get_element_by_id("cmp_camera_glitch_cd").cast_form_float(&base->color_drift);
	ui->get_element_by_id("cmp_camera_glitch_hs").cast_form_float(&base->horizontal_shake);
	ui->get_element_by_id("cmp_camera_glitch_slj").cast_form_float(&base->scan_line_jitter);
	ui->get_element_by_id("cmp_camera_glitch_vj").cast_form_float(&base->vertical_jump);
}