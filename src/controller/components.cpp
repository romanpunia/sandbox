#include "components.h"
#include "resolvers.h"
#include "../core/sandbox.h"

void component_model(gui::context* ui, components::model* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	static material* last_material = nullptr;
	static mesh_buffer* last_mesh = nullptr;

	if (changed)
	{
		last_material = nullptr;
		last_mesh = nullptr;
	}

	app->state.system->set_boolean("sl_cmp_model_source", base->get_drawable() && last_mesh != nullptr);
	app->state.system->set_boolean("sl_cmp_model_assigned", base->get_drawable());

	resolve_model(ui, "cmp_model_source", base, changed);
	ui->get_element_by_id("cmp_model_uv_x").cast_form_float(&base->texcoord.x);
	ui->get_element_by_id("cmp_model_uv_y").cast_form_float(&base->texcoord.y);
	ui->get_element_by_id("cmp_model_static").cast_form_boolean(&base->constant);

	if (ui->get_element_by_id("cmp_model_mesh").cast_form_pointer((void**)&last_mesh))
		last_material = (last_mesh ? base->get_material(last_mesh) : nullptr);

	if (last_mesh != nullptr && ui->get_element_by_id("cmp_model_material").cast_form_pointer((void**)&last_material))
		base->set_material(last_mesh, last_material);

	bool alpha = (base->get_category() == geo_category::transparent);
	if (ui->get_element_by_id("cmp_model_alpha").cast_form_boolean(&alpha))
		base->set_category(alpha ? geo_category::transparent : geo_category::opaque);
}
void component_skin(gui::context* ui, components::skin* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	static material* last_material = nullptr;
	static skin_mesh_buffer* last_mesh = nullptr;
	static int64_t last_joint = -1;

	if (changed)
	{
		last_material = nullptr;
		last_mesh = nullptr;
		last_joint = -1;
	}

	app->state.system->set_boolean("sl_cmp_skin_source", base->get_drawable() && last_mesh != nullptr);
	app->state.system->set_boolean("sl_cmp_skin_assigned", base->get_drawable());
	app->state.system->set_integer("sl_cmp_skin_joint", last_joint);

	if (base->get_drawable() != nullptr)
	{
		app->state.system->set_integer("sl_cmp_skin_joints", (int64_t)base->skeleton.offsets.size() - 1);
		ui->get_element_by_id("cmp_skin_joint").cast_form_int64(&last_joint);

		if (last_joint != -1)
		{
			joint* current = nullptr;
			if (base->get_drawable()->find_joint(last_joint, current))
			{
				string name = current->name + (" (" + to_string(last_joint) + ")");
				auto& pose = base->skeleton.offsets[current->index];

				ui->get_element_by_id("cmp_skin_jname").cast_form_string(&name);
				ui->get_element_by_id("cmp_skin_jp_x").cast_form_float(&pose.offset.position.x);
				ui->get_element_by_id("cmp_skin_jp_y").cast_form_float(&pose.offset.position.y);
				ui->get_element_by_id("cmp_skin_jp_z").cast_form_float(&pose.offset.position.z);
				ui->get_element_by_id("cmp_skin_jr_x").cast_form_float(&pose.offset.rotation.x, mathf::rad2deg());
				ui->get_element_by_id("cmp_skin_jr_y").cast_form_float(&pose.offset.rotation.y, mathf::rad2deg());
				ui->get_element_by_id("cmp_skin_jr_z").cast_form_float(&pose.offset.rotation.z, mathf::rad2deg());
			}
		}
	}
	else
		app->state.system->set_integer("sl_cmp_skin_joints", 0);

	resolve_skin(ui, "cmp_skin_source", base, changed);
	ui->get_element_by_id("cmp_skin_uv_x").cast_form_float(&base->texcoord.x);
	ui->get_element_by_id("cmp_skin_uv_y").cast_form_float(&base->texcoord.y);
	ui->get_element_by_id("cmp_skin_static").cast_form_boolean(&base->constant);

	if (ui->get_element_by_id("cmp_skin_mesh").cast_form_pointer((void**)&last_mesh))
		last_material = (last_mesh ? base->get_material(last_mesh) : nullptr);

	if (last_mesh != nullptr && ui->get_element_by_id("cmp_skin_material").cast_form_pointer((void**)&last_material))
		base->set_material(last_mesh, last_material);

	bool alpha = (base->get_category() == geo_category::transparent);
	if (ui->get_element_by_id("cmp_skin_alpha").cast_form_boolean(&alpha))
		base->set_category(alpha ? geo_category::transparent : geo_category::opaque);
}
void component_emitter(gui::context* ui, components::emitter* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	static material* last_material = nullptr;

	if (changed)
		last_material = nullptr;

	int count = (int)base->get_buffer()->get_array().size();
	ui->get_element_by_id("cmp_emitter_min_x").cast_form_float(&base->min.x);
	ui->get_element_by_id("cmp_emitter_min_y").cast_form_float(&base->min.y);
	ui->get_element_by_id("cmp_emitter_min_z").cast_form_float(&base->min.z);
	ui->get_element_by_id("cmp_emitter_max_x").cast_form_float(&base->max.x);
	ui->get_element_by_id("cmp_emitter_max_y").cast_form_float(&base->max.y);
	ui->get_element_by_id("cmp_emitter_max_z").cast_form_float(&base->max.z);
	ui->get_element_by_id("cmp_emitter_elem_limit").cast_form_int32(&app->state.elements_limit);
	ui->get_element_by_id("cmp_emitter_elem_count").cast_form_int32(&count);
	ui->get_element_by_id("cmp_emitter_quad_based").cast_form_boolean(&base->quad_based);
	ui->get_element_by_id("cmp_emitter_connected").cast_form_boolean(&base->connected);
	ui->get_element_by_id("cmp_emitter_static").cast_form_boolean(&base->constant);

	if (ui->get_element_by_id("cmp_emitter_refill").is_active())
	{
		if (app->state.elements_limit < 1)
			app->state.elements_limit = 1;

		if (app->state.elements_limit > 5000000)
			app->state.elements_limit = 5000000;

		app->renderer->update_buffer_size(base->get_buffer(), app->state.elements_limit);
	}

	if (ui->get_element_by_id("cmp_emitter_material").cast_form_pointer((void**)&last_material))
		base->set_material(nullptr, last_material);

	bool alpha = (base->get_category() == geo_category::transparent);
	if (ui->get_element_by_id("cmp_emitter_alpha").cast_form_boolean(&alpha))
		base->set_category(alpha ? geo_category::transparent : geo_category::opaque);

	bool additive = (base->get_category() == geo_category::additive);
	if (ui->get_element_by_id("cmp_emitter_additive").cast_form_boolean(&additive))
		base->set_category(additive ? geo_category::additive : geo_category::opaque);
}
void component_decal(gui::context* ui, components::decal* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	static material* last_material = nullptr;

	if (changed)
		last_material = nullptr;

	ui->get_element_by_id("cmp_decal_uv_x").cast_form_float(&base->texcoord.x);
	ui->get_element_by_id("cmp_decal_uv_y").cast_form_float(&base->texcoord.y);

	if (ui->get_element_by_id("cmp_decal_material").cast_form_pointer((void**)&last_material))
		base->set_material(nullptr, last_material);

	bool alpha = (base->get_category() == geo_category::transparent);
	if (ui->get_element_by_id("cmp_decal_alpha").cast_form_boolean(&alpha))
		base->set_category(alpha ? geo_category::transparent : geo_category::opaque);
}
void component_soft_body(gui::context* ui, components::soft_body* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	static material* last_material = nullptr;

	if (changed)
		last_material = nullptr;

	soft_body* body = base->get_body();
	if (body != nullptr)
	{
		soft_body::desc& desc = body->get_initial_state();
		ui->get_element_by_id("cmp_soft_body_vcf").cast_form_float(&desc.config.vcf);
		ui->get_element_by_id("cmp_soft_body_dp").cast_form_float(&desc.config.dp);
		ui->get_element_by_id("cmp_soft_body_dg").cast_form_float(&desc.config.dg);
		ui->get_element_by_id("cmp_soft_body_lf").cast_form_float(&desc.config.lf);
		ui->get_element_by_id("cmp_soft_body_pr").cast_form_float(&desc.config.pr);
		ui->get_element_by_id("cmp_soft_body_vc").cast_form_float(&desc.config.vc);
		ui->get_element_by_id("cmp_soft_body_df").cast_form_float(&desc.config.df);
		ui->get_element_by_id("cmp_soft_body_mt").cast_form_float(&desc.config.mt);
		ui->get_element_by_id("cmp_soft_body_chr").cast_form_float(&desc.config.chr);
		ui->get_element_by_id("cmp_soft_body_khr").cast_form_float(&desc.config.khr);
		ui->get_element_by_id("cmp_soft_body_shr").cast_form_float(&desc.config.shr);
		ui->get_element_by_id("cmp_soft_body_ahr").cast_form_float(&desc.config.ahr);
		ui->get_element_by_id("cmp_soft_body_srhr_cl").cast_form_float(&desc.config.srhr_cl);
		ui->get_element_by_id("cmp_soft_body_skhr_cl").cast_form_float(&desc.config.skhr_cl);
		ui->get_element_by_id("cmp_soft_body_sshr_cl").cast_form_float(&desc.config.sshr_cl);
		ui->get_element_by_id("cmp_soft_body_sr_splt_cl").cast_form_float(&desc.config.sr_splt_cl);
		ui->get_element_by_id("cmp_soft_body_sk_splt_cl").cast_form_float(&desc.config.sk_splt_cl);
		ui->get_element_by_id("cmp_soft_body_ss_splt_cl").cast_form_float(&desc.config.ss_splt_cl);
		ui->get_element_by_id("cmp_soft_body_mx_vol").cast_form_float(&desc.config.max_volume);
		ui->get_element_by_id("cmp_soft_body_ts").cast_form_float(&desc.config.time_scale);
		ui->get_element_by_id("cmp_soft_body_drag").cast_form_float(&desc.config.drag);
		ui->get_element_by_id("cmp_soft_body_mx_strs").cast_form_float(&desc.config.max_stress);
		ui->get_element_by_id("cmp_soft_body_cltrs").cast_form_int32(&desc.config.clusters);
		ui->get_element_by_id("cmp_soft_body_constrs").cast_form_int32(&desc.config.constraints);
		ui->get_element_by_id("cmp_soft_body_vit").cast_form_int32(&desc.config.viterations);
		ui->get_element_by_id("cmp_soft_body_pit").cast_form_int32(&desc.config.piterations);
		ui->get_element_by_id("cmp_soft_body_dit").cast_form_int32(&desc.config.diterations);
		ui->get_element_by_id("cmp_soft_body_cit").cast_form_int32(&desc.config.citerations);
		ui->get_element_by_id("cmp_soft_body_kinemat").cast_form_boolean(&base->kinematic);

		vector3 offset = body->get_anisotropic_friction();
		if (ui->get_element_by_id("cmp_soft_body_af_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_soft_body_af_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_soft_body_af_z").cast_form_float(&offset.z))
			body->set_anisotropic_friction(offset);

		offset = body->get_wind_velocity();
		if (ui->get_element_by_id("cmp_soft_body_wv_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_soft_body_wv_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_soft_body_wv_z").cast_form_float(&offset.z))
			body->set_wind_velocity(offset);

		float value = body->get_total_mass();
		if (ui->get_element_by_id("cmp_soft_body_mass").cast_form_float(&value))
			body->set_total_mass(value);

		value = body->get_restitution();
		if (ui->get_element_by_id("cmp_soft_body_rest").cast_form_float(&value))
			body->set_restitution(value);

		value = body->get_spinning_friction();
		if (ui->get_element_by_id("cmp_soft_body_spinf").cast_form_float(&value))
			body->set_spinning_friction(value);

		value = body->get_contact_damping();
		if (ui->get_element_by_id("cmp_soft_body_contdamp").cast_form_float(&value))
			body->set_contact_damping(value);

		value = body->get_contact_stiffness();
		if (ui->get_element_by_id("cmp_soft_body_contstf").cast_form_float(&value, 1.0f / 1000000000000.0f))
			body->set_contact_stiffness(value);

		value = body->get_friction();
		if (ui->get_element_by_id("cmp_soft_body_fric").cast_form_float(&value))
			body->set_friction(value);

		value = body->get_hit_fraction();
		if (ui->get_element_by_id("cmp_soft_body_frac").cast_form_float(&value))
			body->set_hit_fraction(value);

		value = body->get_ccd_motion_threshold();
		if (ui->get_element_by_id("cmp_soft_body_mtt").cast_form_float(&value))
			body->set_ccd_motion_threshold(value);

		value = body->get_ccd_swept_sphere_radius();
		if (ui->get_element_by_id("cmp_soft_body_srad").cast_form_float(&value))
			body->set_ccd_swept_sphere_radius(value);

		value = body->get_deactivation_time();
		if (ui->get_element_by_id("cmp_soft_body_dtime").cast_form_float(&value))
			body->set_deactivation_time(value);

		value = body->get_rolling_friction();
		if (ui->get_element_by_id("cmp_soft_body_rfric").cast_form_float(&value))
			body->set_rolling_friction(value);

		bool option = body->is_active();
		if (ui->get_element_by_id("cmp_soft_body_active").cast_form_boolean(&option))
			body->set_activity(option);

		option = (body->get_activation_state() != motion_state::disable_deactivation);
		if (ui->get_element_by_id("cmp_soft_body_deact").cast_form_boolean(&option))
		{
			if (option)
				body->set_activation_state(motion_state::active);
			else
				body->set_activation_state(motion_state::disable_deactivation);
		}

		option = body->is_ghost();
		if (ui->get_element_by_id("cmp_soft_body_ghost").cast_form_boolean(&option))
		{
			if (option)
				body->set_as_ghost();
			else
				body->set_as_normal();
		}

		app->state.system->set_boolean("sl_cmp_soft_body_source", true);
	}
	else
		app->state.system->set_boolean("sl_cmp_soft_body_source", false);

	resolve_soft_body(ui, "cmp_soft_body_source", base, changed);
	ui->get_element_by_id("cmp_soft_body_uv_x").cast_form_float(&base->texcoord.x);
	ui->get_element_by_id("cmp_soft_body_uv_y").cast_form_float(&base->texcoord.y);

	if (ui->get_element_by_id("cmp_soft_body_regen").is_active())
		base->regenerate();

	if (ui->get_element_by_id("cmp_soft_body_material").cast_form_pointer((void**)&last_material))
		base->set_material(nullptr, last_material);

	bool alpha = (base->get_category() == geo_category::transparent);
	if (ui->get_element_by_id("cmp_soft_body_alpha").cast_form_boolean(&alpha))
		base->set_category(alpha ? geo_category::transparent : geo_category::opaque);
}
void component_skin_animator(gui::context* ui, components::skin_animator* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	static components::skin_animator* last_base = nullptr;
	static int64_t frame = -1, clip = -1;

	if (last_base != base)
	{
		frame = clip = -1;
		last_base = base;
	}

	skin_animation* animation = base->get_animation();
	app->state.system->set_integer("sl_cmp_skin_animator_clips", animation ? (int64_t)animation->get_clips().size() - 1 : -1);
	app->state.system->set_integer("sl_cmp_skin_animator_clip", clip);
	app->state.system->set_integer("sl_cmp_skin_animator_frame", frame);
	ui->get_element_by_id("cmp_skin_animator_clip").cast_form_int64(&clip);

	string path = base->get_path();
	resolve_skin_animator(ui, "cmp_skin_animator_source", base, changed);
	if (path != base->get_path())
		last_base = nullptr;

	ui->get_element_by_id("cmp_skin_animator_loop").cast_form_boolean(&base->state.looped);
	if (ui->get_element_by_id("cmp_skin_animator_play").is_active())
		base->play(clip, frame);

	if (ui->get_element_by_id("cmp_skin_animator_pause").is_active())
		base->pause();

	if (ui->get_element_by_id("cmp_skin_animator_stop").is_active())
		base->stop();
}
void component_key_animator(gui::context* ui, components::key_animator* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	static components::key_animator* last_base = nullptr;
	static int64_t frame = -1, clip = -1;

	if (last_base != base)
	{
		frame = clip = -1;
		last_base = base;
	}

	app->state.system->set_integer("sl_cmp_key_animator_clips", (int64_t)base->clips.size() - 1);
	app->state.system->set_integer("sl_cmp_key_animator_clip", clip);
	app->state.system->set_integer("sl_cmp_key_animator_frame", frame);
	ui->get_element_by_id("cmp_key_animator_clip").cast_form_int64(&clip);

	if (clip >= 0 && clip < (int64_t)base->clips.size())
	{
		auto& iclip = base->clips[(size_t)clip];
		app->state.system->set_integer("sl_cmp_key_animator_frames", (int64_t)iclip.keys.size() - 1);
		ui->get_element_by_id("cmp_key_animator_cname").cast_form_string(&iclip.name);
		ui->get_element_by_id("cmp_key_animator_cd").cast_form_float(&iclip.duration);
		ui->get_element_by_id("cmp_key_animator_cr").cast_form_float(&iclip.rate);
		ui->get_element_by_id("cmp_key_animator_frame").cast_form_int64(&frame);

		if (frame >= 0 && frame < (int64_t)iclip.keys.size())
		{
			auto& current = iclip.keys[(size_t)frame];
			ui->get_element_by_id("cmp_key_animator_fname").cast_form_int64(&frame);
			ui->get_element_by_id("cmp_key_animator_fp_x").cast_form_float(&current.position.x);
			ui->get_element_by_id("cmp_key_animator_fp_y").cast_form_float(&current.position.y);
			ui->get_element_by_id("cmp_key_animator_fp_z").cast_form_float(&current.position.z);
			ui->get_element_by_id("cmp_key_animator_fr_x").cast_form_float(&current.rotation.x, mathf::rad2deg());
			ui->get_element_by_id("cmp_key_animator_fr_y").cast_form_float(&current.rotation.y, mathf::rad2deg());
			ui->get_element_by_id("cmp_key_animator_fr_z").cast_form_float(&current.rotation.z, mathf::rad2deg());
			ui->get_element_by_id("cmp_key_animator_fs_x").cast_form_float(&current.scale.x);
			ui->get_element_by_id("cmp_key_animator_fs_y").cast_form_float(&current.scale.y);
			ui->get_element_by_id("cmp_key_animator_fs_z").cast_form_float(&current.scale.z);
			ui->get_element_by_id("cmp_key_animator_ft").cast_form_float(&current.time);

			if (ui->get_element_by_id("cmp_key_animator_frem").is_active())
			{
				iclip.keys.erase(iclip.keys.begin() + (size_t)frame);
				frame = -1;
			}
		}

		if (ui->get_element_by_id("cmp_key_animator_cnorm").is_active())
		{
			for (size_t i = 0; i < iclip.keys.size(); i++)
				iclip.keys[i].rotation = iclip.keys[i].rotation;
		}

		if (ui->get_element_by_id("cmp_key_animator_cadde").is_active())
		{
			animator_key key;
			key.scale = 1.0f;
			key.position = 0.0f;
			key.rotation = quaternion();

			iclip.keys.push_back(key);
		}

		if (ui->get_element_by_id("cmp_key_animator_caddm").is_active())
		{
			auto* fTransform = base->get_entity()->get_transform();
			animator_key key;
			key.scale = fTransform->get_scale();
			key.position = fTransform->get_position();
			key.rotation = fTransform->get_rotation().rlerp();

			iclip.keys.push_back(key);
		}

		if (ui->get_element_by_id("cmp_key_animator_caddm").is_active())
		{
			auto* fTransform = app->scene->get_camera()->get_entity()->get_transform();
			animator_key key;
			key.scale = fTransform->get_scale();
			key.position = fTransform->get_position();
			key.rotation = fTransform->get_rotation().rlerp();

			iclip.keys.push_back(key);
		}

		if (ui->get_element_by_id("cmp_key_animator_caddm").is_active())
		{
			auto* fTransform = app->scene->get_camera()->get_entity()->get_transform();
			animator_key key;
			key.scale = fTransform->get_scale();
			key.position = fTransform->get_position();
			key.rotation = quaternion();

			iclip.keys.push_back(key);
		}

		if (ui->get_element_by_id("cmp_key_animator_crem").is_active())
		{
			iclip.keys.clear();
			clip = frame = -1;
		}
	}
	else
		app->state.system->set_integer("sl_cmp_key_animator_frames", -1);

	if (ui->get_element_by_id("cmp_key_animator_cap").is_active())
		base->clips.emplace_back();

	string path = base->get_path();
	resolve_key_animator(ui, "cmp_key_animator_source", base, changed);
	if (path != base->get_path())
		last_base = nullptr;

	ui->get_element_by_id("cmp_key_animator_loop").cast_form_boolean(&base->state.looped);
	ui->get_element_by_id("cmp_key_animator_vpaths").cast_form_boolean(&app->state.is_path_tracked);

	if (ui->get_element_by_id("cmp_key_animator_play").is_active())
		base->play(clip, frame);

	if (ui->get_element_by_id("cmp_key_animator_pause").is_active())
		base->pause();

	if (ui->get_element_by_id("cmp_key_animator_stop").is_active())
		base->stop();
}
void component_emitter_animator(gui::context* ui, components::emitter_animator* base, bool changed)
{
	ui->get_element_by_id("cmp_emitter_animator_ddx").cast_form_float(&base->diffuse.x);
	ui->get_element_by_id("cmp_emitter_animator_ddy").cast_form_float(&base->diffuse.y);
	ui->get_element_by_id("cmp_emitter_animator_ddz").cast_form_float(&base->diffuse.z);
	ui->get_element_by_id("cmp_emitter_animator_ddw").cast_form_float(&base->diffuse.w);
	ui->get_element_by_id("cmp_emitter_animator_dpx").cast_form_float(&base->position.x);
	ui->get_element_by_id("cmp_emitter_animator_dpy").cast_form_float(&base->position.y);
	ui->get_element_by_id("cmp_emitter_animator_dpz").cast_form_float(&base->position.z);
	ui->get_element_by_id("cmp_emitter_animator_dvx").cast_form_float(&base->velocity.x);
	ui->get_element_by_id("cmp_emitter_animator_dvy").cast_form_float(&base->velocity.y);
	ui->get_element_by_id("cmp_emitter_animator_dvz").cast_form_float(&base->velocity.z);
	ui->get_element_by_id("cmp_emitter_animator_drs").cast_form_float(&base->rotation_speed, mathf::rad2deg());
	ui->get_element_by_id("cmp_emitter_animator_dss").cast_form_float(&base->scale_speed);
	ui->get_element_by_id("cmp_emitter_animator_dns").cast_form_float(&base->noise);
	ui->get_element_by_id("cmp_emitter_animator_it").cast_form_int32(&base->spawner.iterations);
	ui->get_element_by_id("cmp_emitter_animator_active").cast_form_boolean(&base->simulate);
	ui->get_element_by_id("cmp_emitter_animator_fmin_dx").cast_form_float(&base->spawner.diffusion.min.x);
	ui->get_element_by_id("cmp_emitter_animator_fmin_dy").cast_form_float(&base->spawner.diffusion.min.y);
	ui->get_element_by_id("cmp_emitter_animator_fmin_dz").cast_form_float(&base->spawner.diffusion.min.z);
	ui->get_element_by_id("cmp_emitter_animator_fmin_dw").cast_form_float(&base->spawner.diffusion.min.w);
	ui->get_element_by_id("cmp_emitter_animator_fmax_dx").cast_form_float(&base->spawner.diffusion.max.x);
	ui->get_element_by_id("cmp_emitter_animator_fmax_dy").cast_form_float(&base->spawner.diffusion.max.y);
	ui->get_element_by_id("cmp_emitter_animator_fmax_dz").cast_form_float(&base->spawner.diffusion.max.z);
	ui->get_element_by_id("cmp_emitter_animator_fmax_dw").cast_form_float(&base->spawner.diffusion.max.w);
	ui->get_element_by_id("cmp_emitter_animator_facc_d").cast_form_float(&base->spawner.diffusion.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_d").cast_form_boolean(&base->spawner.diffusion.intensity);
	ui->get_element_by_id("cmp_emitter_animator_fmin_px").cast_form_float(&base->spawner.position.min.x);
	ui->get_element_by_id("cmp_emitter_animator_fmin_py").cast_form_float(&base->spawner.position.min.y);
	ui->get_element_by_id("cmp_emitter_animator_fmin_pz").cast_form_float(&base->spawner.position.min.z);
	ui->get_element_by_id("cmp_emitter_animator_fmax_px").cast_form_float(&base->spawner.position.max.x);
	ui->get_element_by_id("cmp_emitter_animator_fmax_py").cast_form_float(&base->spawner.position.max.y);
	ui->get_element_by_id("cmp_emitter_animator_fmax_pz").cast_form_float(&base->spawner.position.max.z);
	ui->get_element_by_id("cmp_emitter_animator_facc_p").cast_form_float(&base->spawner.position.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_p").cast_form_boolean(&base->spawner.position.intensity);
	ui->get_element_by_id("cmp_emitter_animator_fmin_vx").cast_form_float(&base->spawner.velocity.min.x);
	ui->get_element_by_id("cmp_emitter_animator_fmin_vy").cast_form_float(&base->spawner.velocity.min.y);
	ui->get_element_by_id("cmp_emitter_animator_fmin_vz").cast_form_float(&base->spawner.velocity.min.z);
	ui->get_element_by_id("cmp_emitter_animator_fmax_vx").cast_form_float(&base->spawner.velocity.max.x);
	ui->get_element_by_id("cmp_emitter_animator_fmax_vy").cast_form_float(&base->spawner.velocity.max.y);
	ui->get_element_by_id("cmp_emitter_animator_fmax_vz").cast_form_float(&base->spawner.velocity.max.z);
	ui->get_element_by_id("cmp_emitter_animator_facc_v").cast_form_float(&base->spawner.velocity.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_v").cast_form_boolean(&base->spawner.velocity.intensity);
	ui->get_element_by_id("cmp_emitter_animator_fmin_nx").cast_form_float(&base->spawner.noise.min.x);
	ui->get_element_by_id("cmp_emitter_animator_fmin_ny").cast_form_float(&base->spawner.noise.min.y);
	ui->get_element_by_id("cmp_emitter_animator_fmin_nz").cast_form_float(&base->spawner.noise.min.z);
	ui->get_element_by_id("cmp_emitter_animator_fmax_nx").cast_form_float(&base->spawner.noise.max.x);
	ui->get_element_by_id("cmp_emitter_animator_fmax_ny").cast_form_float(&base->spawner.noise.max.y);
	ui->get_element_by_id("cmp_emitter_animator_fmax_nz").cast_form_float(&base->spawner.noise.max.z);
	ui->get_element_by_id("cmp_emitter_animator_facc_n").cast_form_float(&base->spawner.noise.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_n").cast_form_boolean(&base->spawner.noise.intensity);
	ui->get_element_by_id("cmp_emitter_animator_fmin_r").cast_form_float(&base->spawner.rotation.min, mathf::rad2deg());
	ui->get_element_by_id("cmp_emitter_animator_fmax_r").cast_form_float(&base->spawner.rotation.max, mathf::rad2deg());
	ui->get_element_by_id("cmp_emitter_animator_facc_r").cast_form_float(&base->spawner.rotation.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_r").cast_form_boolean(&base->spawner.rotation.intensity);
	ui->get_element_by_id("cmp_emitter_animator_fmin_a").cast_form_float(&base->spawner.angular.min, mathf::rad2deg());
	ui->get_element_by_id("cmp_emitter_animator_fmax_a").cast_form_float(&base->spawner.angular.max, mathf::rad2deg());
	ui->get_element_by_id("cmp_emitter_animator_facc_a").cast_form_float(&base->spawner.angular.accuracy);
	ui->get_element_by_id("cmp_emitter_animator_fint_a").cast_form_boolean(&base->spawner.angular.intensity);
}
void component_rigid_body(gui::context* ui, components::rigid_body* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	static components::rigid_body* last_base = nullptr;
	if (last_base != base)
	{
		shape type = (base->get_body() ? base->get_body()->get_collision_shape_type() : shape::invalid);
		if (type == shape::box)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("box");
		else if (type == shape::sphere)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("sphere");
		else if (type == shape::capsule)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("capsule");
		else if (type == shape::cone)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("cone");
		else if (type == shape::cylinder)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("cylinder");
		else if (type == shape::convex_hull)
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("other");
		else
			ui->get_element_by_id("cmp_rigid_body_shape").set_form_value("none");

		last_base = base;
	}

	string shape = ui->get_element_by_id("cmp_rigid_body_shape").get_form_value();
	if (shape == "other")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", true);
		resolve_rigid_body(ui, "cmp_rigid_body_source", base, changed);

		if (base->get_body() && base->get_body()->get_collision_shape_type() != shape::convex_hull)
			base->clear();
	}
	else if (shape == "none")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (base->get_body() && base->get_body()->get_collision_shape_type() != shape::invalid)
			base->clear();
	}
	else if (shape == "box")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (!base->get_body() || base->get_body()->get_collision_shape_type() != shape::box)
			base->load(app->scene->get_simulator()->create_cube(), 0, 0);
	}
	else if (shape == "sphere")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (!base->get_body() || base->get_body()->get_collision_shape_type() != shape::sphere)
			base->load(app->scene->get_simulator()->create_sphere(), 0, 0);
	}
	else if (shape == "capsule")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (!base->get_body() || base->get_body()->get_collision_shape_type() != shape::capsule)
			base->load(app->scene->get_simulator()->create_capsule(), 0, 0);
	}
	else if (shape == "cone")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (!base->get_body() || base->get_body()->get_collision_shape_type() != shape::cone)
			base->load(app->scene->get_simulator()->create_cone(), 0, 0);
	}
	else if (shape == "cylinder")
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
		if (!base->get_body() || base->get_body()->get_collision_shape_type() != shape::cylinder)
			base->load(app->scene->get_simulator()->create_cylinder(), 0, 0);
	}

	rigid_body* body = base->get_body();
	if (body != nullptr)
	{
		app->state.system->set_boolean("sl_cmp_rigid_body_source", true);
		ui->get_element_by_id("cmp_soft_body_kinemat").cast_form_boolean(&base->kinematic);

		vector3 offset = body->get_linear_velocity();
		if (ui->get_element_by_id("cmp_rigid_body_v_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_v_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_v_z").cast_form_float(&offset.z))
			body->set_linear_velocity(offset);

		offset = body->get_angular_velocity();
		if (ui->get_element_by_id("cmp_rigid_body_t_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_t_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_t_z").cast_form_float(&offset.z))
			body->set_angular_velocity(offset);

		offset = body->get_gravity();
		if (ui->get_element_by_id("cmp_rigid_body_g_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_g_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_g_z").cast_form_float(&offset.z))
			body->set_gravity(offset);

		offset = body->get_angular_factor();
		if (ui->get_element_by_id("cmp_rigid_body_af_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_af_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_af_z").cast_form_float(&offset.z))
			body->set_angular_factor(offset);

		offset = body->get_linear_factor();
		if (ui->get_element_by_id("cmp_rigid_body_lf_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_lf_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_lf_z").cast_form_float(&offset.z))
			body->set_linear_factor(offset);

		offset = body->get_anisotropic_friction();
		if (ui->get_element_by_id("cmp_rigid_body_afric_x").cast_form_float(&offset.x) ||
			ui->get_element_by_id("cmp_rigid_body_afric_y").cast_form_float(&offset.y) ||
			ui->get_element_by_id("cmp_rigid_body_afric_z").cast_form_float(&offset.z))
			body->set_anisotropic_friction(offset);

		float value = body->get_mass();
		if (ui->get_element_by_id("cmp_rigid_body_mass").cast_form_float(&value))
			body->set_mass(value);

		value = body->get_angular_damping();
		if (ui->get_element_by_id("cmp_rigid_body_ad").cast_form_float(&value))
			body->set_angular_damping(value);

		value = body->get_linear_damping();
		if (ui->get_element_by_id("cmp_rigid_body_ld").cast_form_float(&value))
			body->set_linear_damping(value);

		value = body->get_contact_damping();
		if (ui->get_element_by_id("cmp_rigid_body_cd").cast_form_float(&value))
			body->set_contact_damping(value);

		value = body->get_hit_fraction();
		if (ui->get_element_by_id("cmp_rigid_body_frac").cast_form_float(&value))
			body->set_hit_fraction(value);

		value = body->get_restitution();
		if (ui->get_element_by_id("cmp_rigid_body_rest").cast_form_float(&value))
			body->set_restitution(value);

		value = body->get_friction();
		if (ui->get_element_by_id("cmp_rigid_body_fric").cast_form_float(&value))
			body->set_friction(value);

		value = body->get_spinning_friction();
		if (ui->get_element_by_id("cmp_rigid_body_spinf").cast_form_float(&value))
			body->set_spinning_friction(value);

		value = body->get_rolling_friction();
		if (ui->get_element_by_id("cmp_rigid_body_rollf").cast_form_float(&value))
			body->set_rolling_friction(value);

		value = body->get_contact_stiffness();
		if (ui->get_element_by_id("cmp_rigid_body_stiff").cast_form_float(&value, 1.0f / 1000000000000.0f))
			body->set_contact_stiffness(value);

		value = body->get_angular_sleeping_threshold();
		if (ui->get_element_by_id("cmp_rigid_body_ath").cast_form_float(&value))
			body->set_angular_sleeping_threshold(value);

		value = body->get_linear_sleeping_threshold();
		if (ui->get_element_by_id("cmp_rigid_body_lth").cast_form_float(&value))
			body->set_linear_sleeping_threshold(value);

		value = body->get_ccd_motion_threshold();
		if (ui->get_element_by_id("cmp_rigid_body_mth").cast_form_float(&value))
			body->set_ccd_motion_threshold(value);

		value = body->get_ccd_swept_sphere_radius();
		if (ui->get_element_by_id("cmp_rigid_body_srad").cast_form_float(&value))
			body->set_ccd_swept_sphere_radius(value);

		value = body->get_deactivation_time();
		if (ui->get_element_by_id("cmp_rigid_body_dtime").cast_form_float(&value))
			body->set_deactivation_time(value);

		bool option = body->is_active();
		if (ui->get_element_by_id("cmp_rigid_body_active").cast_form_boolean(&option))
			body->set_activity(option);

		option = (body->get_activation_state() != motion_state::disable_deactivation);
		if (ui->get_element_by_id("cmp_rigid_body_deact").cast_form_boolean(&option))
		{
			if (option)
				body->set_activation_state(motion_state::active);
			else
				body->set_activation_state(motion_state::disable_deactivation);
		}

		option = body->is_ghost();
		if (ui->get_element_by_id("cmp_rigid_body_ghost").cast_form_boolean(&option))
		{
			if (option)
				body->set_as_ghost();
			else
				body->set_as_normal();
		}
	}
	else
		app->state.system->set_boolean("sl_cmp_rigid_body_source", false);
}
void component_acceleration(gui::context* ui, components::acceleration* base, bool changed)
{
	ui->get_element_by_id("cmp_acceleration_av_x").cast_form_float(&base->amplitude_velocity.x);
	ui->get_element_by_id("cmp_acceleration_av_y").cast_form_float(&base->amplitude_velocity.y);
	ui->get_element_by_id("cmp_acceleration_av_z").cast_form_float(&base->amplitude_velocity.z);
	ui->get_element_by_id("cmp_acceleration_v_x").cast_form_float(&base->constant_velocity.x);
	ui->get_element_by_id("cmp_acceleration_v_y").cast_form_float(&base->constant_velocity.y);
	ui->get_element_by_id("cmp_acceleration_v_z").cast_form_float(&base->constant_velocity.z);
	ui->get_element_by_id("cmp_acceleration_at_x").cast_form_float(&base->amplitude_torque.x);
	ui->get_element_by_id("cmp_acceleration_at_y").cast_form_float(&base->amplitude_torque.y);
	ui->get_element_by_id("cmp_acceleration_at_z").cast_form_float(&base->amplitude_torque.z);
	ui->get_element_by_id("cmp_acceleration_t_x").cast_form_float(&base->constant_torque.x);
	ui->get_element_by_id("cmp_acceleration_t_y").cast_form_float(&base->constant_torque.y);
	ui->get_element_by_id("cmp_acceleration_t_z").cast_form_float(&base->constant_torque.z);
	ui->get_element_by_id("cmp_acceleration_c_x").cast_form_float(&base->constant_center.x);
	ui->get_element_by_id("cmp_acceleration_c_y").cast_form_float(&base->constant_center.y);
	ui->get_element_by_id("cmp_acceleration_c_z").cast_form_float(&base->constant_center.z);
	ui->get_element_by_id("cmp_acceleration_kinemat").cast_form_boolean(&base->kinematic);
}
void component_slider_constraint(gui::context* ui, components::slider_constraint* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	static bool ghost = true, linear = true;
	resolve_slider_constraint(ui, "cmp_slider_constraint_entity", base, ghost, linear, changed);

	sconstraint* body = base->get_constraint();
	if (body != nullptr)
	{
		app->state.system->set_boolean("sl_cmp_slider_constraint_entity", true);

		float value = body->get_angular_motor_velocity();
		if (ui->get_element_by_id("cmp_slider_constraint_amv").cast_form_float(&value))
			body->set_angular_motor_velocity(value);

		value = body->get_linear_motor_velocity();
		if (ui->get_element_by_id("cmp_slider_constraint_lmv").cast_form_float(&value))
			body->set_linear_motor_velocity(value);

		value = body->get_upper_linear_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_ull").cast_form_float(&value))
			body->set_upper_linear_limit(value);

		value = body->get_lower_linear_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_lll").cast_form_float(&value))
			body->set_lower_linear_limit(value);

		value = body->get_angular_damping_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_add").cast_form_float(&value))
			body->set_angular_damping_direction(value);

		value = body->get_linear_damping_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_ldd").cast_form_float(&value))
			body->set_linear_damping_direction(value);

		value = body->get_angular_damping_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_adl").cast_form_float(&value))
			body->set_angular_damping_limit(value);

		value = body->get_linear_damping_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_ldl").cast_form_float(&value))
			body->set_linear_damping_limit(value);

		value = body->get_angular_damping_ortho();
		if (ui->get_element_by_id("cmp_slider_constraint_ado").cast_form_float(&value))
			body->set_angular_damping_ortho(value);

		value = body->get_linear_damping_ortho();
		if (ui->get_element_by_id("cmp_slider_constraint_ldo").cast_form_float(&value))
			body->set_linear_damping_ortho(value);

		value = body->get_upper_angular_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_ual").cast_form_float(&value))
			body->set_upper_angular_limit(value);

		value = body->get_lower_angular_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_lal").cast_form_float(&value))
			body->set_lower_angular_limit(value);

		value = body->get_max_angular_motor_force();
		if (ui->get_element_by_id("cmp_slider_constraint_mamf").cast_form_float(&value))
			body->set_max_angular_motor_force(value);

		value = body->get_max_linear_motor_force();
		if (ui->get_element_by_id("cmp_slider_constraint_mlmf").cast_form_float(&value))
			body->set_max_linear_motor_force(value);

		value = body->get_angular_restitution_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_ard").cast_form_float(&value))
			body->set_angular_restitution_direction(value);

		value = body->get_linear_restitution_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_lrd").cast_form_float(&value))
			body->set_linear_restitution_direction(value);

		value = body->get_angular_softness_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_asd").cast_form_float(&value))
			body->set_angular_softness_direction(value);

		value = body->get_linear_softness_direction();
		if (ui->get_element_by_id("cmp_slider_constraint_lsd").cast_form_float(&value))
			body->set_linear_softness_direction(value);

		value = body->get_angular_softness_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_asl").cast_form_float(&value))
			body->set_angular_softness_limit(value);

		value = body->get_linear_softness_limit();
		if (ui->get_element_by_id("cmp_slider_constraint_lsl").cast_form_float(&value))
			body->set_linear_softness_limit(value);

		value = body->get_angular_softness_ortho();
		if (ui->get_element_by_id("cmp_slider_constraint_aso").cast_form_float(&value))
			body->set_angular_softness_ortho(value);

		value = body->get_linear_softness_ortho();
		if (ui->get_element_by_id("cmp_slider_constraint_lso").cast_form_float(&value))
			body->set_linear_softness_ortho(value);

		bool option = body->get_powered_angular_motor();
		if (ui->get_element_by_id("cmp_slider_constraint_amp").cast_form_boolean(&option))
			body->set_powered_angular_motor(option);

		option = body->get_powered_linear_motor();
		if (ui->get_element_by_id("cmp_slider_constraint_lmp").cast_form_boolean(&option))
			body->set_powered_linear_motor(option);

		option = body->is_enabled();
		if (ui->get_element_by_id("cmp_slider_constraint_active").cast_form_boolean(&option))
			body->set_enabled(option);
	}
	else
	{
		app->state.system->set_boolean("sl_cmp_slider_constraint_entity", false);
		ui->get_element_by_id("cmp_slider_constraint_ghost").cast_form_boolean(&ghost);
		ui->get_element_by_id("cmp_slider_constraint_linear").cast_form_boolean(&linear);
	}
}
void component_free_look(gui::context* ui, components::free_look* base, bool changed)
{
	resolve_key_code(ui, "cmp_free_look_key_rot", &base->rotate, changed);
	ui->get_element_by_id("cmp_free_look_sens").cast_form_float(&base->sensivity);
}
void component_fly(gui::context* ui, components::fly* base, bool changed)
{
	resolve_key_code(ui, "cmp_fly_key_forward", &base->forward, changed);
	resolve_key_code(ui, "cmp_fly_key_backward", &base->backward, changed);
	resolve_key_code(ui, "cmp_fly_key_right", &base->right, changed);
	resolve_key_code(ui, "cmp_fly_key_left", &base->left, changed);
	resolve_key_code(ui, "cmp_fly_key_up", &base->up, changed);
	resolve_key_code(ui, "cmp_fly_key_down", &base->down, changed);
	resolve_key_code(ui, "cmp_fly_key_slow", &base->slow, changed);
	resolve_key_code(ui, "cmp_fly_key_fast", &base->fast, changed);
	ui->get_element_by_id("cmp_fly_ax").cast_form_float(&base->moving.axis.x);
	ui->get_element_by_id("cmp_fly_ay").cast_form_float(&base->moving.axis.y);
	ui->get_element_by_id("cmp_fly_az").cast_form_float(&base->moving.axis.z);
	ui->get_element_by_id("cmp_fly_fad").cast_form_float(&base->moving.fading);
	ui->get_element_by_id("cmp_fly_sp_down").cast_form_float(&base->moving.slower);
	ui->get_element_by_id("cmp_fly_sp_normal").cast_form_float(&base->moving.normal);
	ui->get_element_by_id("cmp_fly_sp_up").cast_form_float(&base->moving.faster);
}
void component_audio_source(gui::context* ui, components::audio_source* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	resolve_audio_source(ui, "cmp_audio_source_clip", base, changed);

	audio_clip* source = base->get_source()->get_clip();
	if (source != nullptr)
	{
		auto& sync = base->get_sync();
		app->state.system->set_boolean("sl_cmp_audio_source_clip", true);
		ui->get_element_by_id("cmp_audio_source_gain").cast_form_float(&sync.gain);
		ui->get_element_by_id("cmp_audio_source_pitch").cast_form_float(&sync.pitch);
		ui->get_element_by_id("cmp_audio_source_dx").cast_form_float(&sync.direction.x);
		ui->get_element_by_id("cmp_audio_source_dy").cast_form_float(&sync.direction.y);
		ui->get_element_by_id("cmp_audio_source_dz").cast_form_float(&sync.direction.z);
		ui->get_element_by_id("cmp_audio_source_cia").cast_form_float(&sync.cone_inner_angle);
		ui->get_element_by_id("cmp_audio_source_coa").cast_form_float(&sync.cone_outer_angle);
		ui->get_element_by_id("cmp_audio_source_cog").cast_form_float(&sync.cone_outer_gain);
		ui->get_element_by_id("cmp_audio_source_aa").cast_form_float(&sync.air_absorption);
		ui->get_element_by_id("cmp_audio_source_rro").cast_form_float(&sync.room_roll_off);
		ui->get_element_by_id("cmp_audio_source_rr").cast_form_float(&sync.rolloff);
		ui->get_element_by_id("cmp_audio_source_dist").cast_form_float(&sync.distance);
		ui->get_element_by_id("cmp_audio_source_rdist").cast_form_float(&sync.ref_distance);
		ui->get_element_by_id("cmp_audio_source_loop").cast_form_boolean(&sync.is_looped);
		ui->get_element_by_id("cmp_audio_source_3d").cast_form_boolean(&sync.is_relative);

		if (ui->get_element_by_id("cmp_audio_source_pos").cast_form_float(&sync.position))
			base->apply_playing_position();

		float length = source->length();
		ui->get_element_by_id("cmp_audio_source_length").cast_form_float(&length);
		app->state.system->set_float("sl_cmp_audio_source_length", length);

		unsigned int buffer = source->get_buffer();
		ui->get_element_by_id("cmp_audio_source_buffer").cast_form_uint32(&buffer);

		int format = source->get_format();
		ui->get_element_by_id("cmp_audio_source_format").cast_form_int32(&format);

		bool mono = source->is_mono();
		ui->get_element_by_id("cmp_audio_source_mono").cast_form_boolean(&mono);

		if (ui->get_element_by_id("cmp_audio_source_play").is_active())
			base->get_source()->play();

		if (ui->get_element_by_id("cmp_audio_source_pause").is_active())
			base->get_source()->pause();

		if (ui->get_element_by_id("cmp_audio_source_stop").is_active())
			base->get_source()->stop();
	}
	else
	{
		app->state.system->set_boolean("sl_cmp_audio_source_clip", false);
		app->state.system->set_float("sl_cmp_audio_source_length", 0.0f);
	}
}
void component_audio_listener(gui::context* ui, components::audio_listener* base, bool changed)
{
	ui->get_element_by_id("cmp_audio_listener_gain").cast_form_float(&base->gain);
}
void component_point_light(gui::context* ui, components::point_light* base, bool changed)
{
	resolve_color3(ui, "cmp_point_light_diffuse", &base->diffuse);
	ui->get_element_by_id("cmp_point_light_pcf").cast_form_uint32(&base->shadow.iterations);
	ui->get_element_by_id("cmp_point_light_emission").cast_form_float(&base->emission);
	ui->get_element_by_id("cmp_point_light_disp").cast_form_float(&base->disperse);
	ui->get_element_by_id("cmp_point_light_sd_bias").cast_form_float(&base->shadow.bias);
	ui->get_element_by_id("cmp_point_light_sd_dist").cast_form_float(&base->shadow.distance);
	ui->get_element_by_id("cmp_point_light_sd_soft").cast_form_float(&base->shadow.softness);
	ui->get_element_by_id("cmp_point_light_sd_active").cast_form_boolean(&base->shadow.enabled);

	attenuation size = base->get_size();
	if (ui->get_element_by_id("cmp_point_light_srange").cast_form_float(&size.radius) ||
		ui->get_element_by_id("cmp_point_light_sc1").cast_form_float(&size.C1) ||
		ui->get_element_by_id("cmp_point_light_sc2").cast_form_float(&size.C2))
		base->set_size(size);
}
void component_spot_light(gui::context* ui, components::spot_light* base, bool changed)
{
	resolve_color3(ui, "cmp_spot_light_diffuse", &base->diffuse);
	ui->get_element_by_id("cmp_spot_light_pcf").cast_form_uint32(&base->shadow.iterations);
	ui->get_element_by_id("cmp_spot_light_emission").cast_form_float(&base->emission);
	ui->get_element_by_id("cmp_spot_light_disp").cast_form_float(&base->disperse);
	ui->get_element_by_id("cmp_spot_light_cutoff").cast_form_float(&base->cutoff);
	ui->get_element_by_id("cmp_spot_light_sd_bias").cast_form_float(&base->shadow.bias);
	ui->get_element_by_id("cmp_spot_light_sd_dist").cast_form_float(&base->shadow.distance);
	ui->get_element_by_id("cmp_spot_light_sd_soft").cast_form_float(&base->shadow.softness);
	ui->get_element_by_id("cmp_spot_light_sd_active").cast_form_boolean(&base->shadow.enabled);

	attenuation size = base->get_size();
	if (ui->get_element_by_id("cmp_spot_light_srange").cast_form_float(&size.radius) ||
		ui->get_element_by_id("cmp_spot_light_sc1").cast_form_float(&size.C1) ||
		ui->get_element_by_id("cmp_spot_light_sc2").cast_form_float(&size.C2))
		base->set_size(size);
}
void component_line_light(gui::context* ui, components::line_light* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	app->state.system->set_integer("sl_cmp_line_light_cascades", base->shadow.cascades);
	if (ui->get_element_by_id("cmp_line_light_sd_casc").cast_form_uint32(&base->shadow.cascades))
		base->shadow.cascades = math<uint32_t>::clamp(base->shadow.cascades, 0, 6);

	if (ui->get_element_by_id("cmp_line_light_sd_dist0").cast_form_float(&base->shadow.distance[0]))
		base->shadow.distance[0] = mathf::max(0, base->shadow.distance[0]);

	if (ui->get_element_by_id("cmp_line_light_sd_dist1").cast_form_float(&base->shadow.distance[1]))
		base->shadow.distance[1] = mathf::max(base->shadow.distance[0] + 1.0f, base->shadow.distance[1]);

	if (ui->get_element_by_id("cmp_line_light_sd_dist2").cast_form_float(&base->shadow.distance[2]))
		base->shadow.distance[2] = mathf::max(base->shadow.distance[1] + 1.0f, base->shadow.distance[2]);

	if (ui->get_element_by_id("cmp_line_light_sd_dist3").cast_form_float(&base->shadow.distance[3]))
		base->shadow.distance[3] = mathf::max(base->shadow.distance[2] + 1.0f, base->shadow.distance[3]);

	if (ui->get_element_by_id("cmp_line_light_sd_dist4").cast_form_float(&base->shadow.distance[4]))
		base->shadow.distance[4] = mathf::max(base->shadow.distance[3] + 1.0f, base->shadow.distance[4]);

	if (ui->get_element_by_id("cmp_line_light_sd_dist5").cast_form_float(&base->shadow.distance[5]))
		base->shadow.distance[5] = mathf::max(base->shadow.distance[4] + 1.0f, base->shadow.distance[5]);

	resolve_color3(ui, "cmp_line_light_diffuse", &base->diffuse);
	resolve_color3(ui, "cmp_line_light_rlh", &base->sky.rlh_emission);
	resolve_color3(ui, "cmp_line_light_mie", &base->sky.mie_emission);
	ui->get_element_by_id("cmp_line_light_disp").cast_form_float(&base->disperse);
	ui->get_element_by_id("cmp_line_light_emission").cast_form_float(&base->emission);
	ui->get_element_by_id("cmp_line_light_rlhh").cast_form_float(&base->sky.rlh_height);
	ui->get_element_by_id("cmp_line_light_mieh").cast_form_float(&base->sky.mie_height);
	ui->get_element_by_id("cmp_line_light_mied").cast_form_float(&base->sky.mie_direction);
	ui->get_element_by_id("cmp_line_light_scatter").cast_form_float(&base->sky.intensity);
	ui->get_element_by_id("cmp_line_light_inner").cast_form_float(&base->sky.inner_radius);
	ui->get_element_by_id("cmp_line_light_outer").cast_form_float(&base->sky.outer_radius);
	ui->get_element_by_id("cmp_line_light_pcf").cast_form_uint32(&base->shadow.iterations);
	ui->get_element_by_id("cmp_line_light_sd_far").cast_form_float(&base->shadow.far);
	ui->get_element_by_id("cmp_line_light_sd_near").cast_form_float(&base->shadow.near);
	ui->get_element_by_id("cmp_line_light_sd_bias").cast_form_float(&base->shadow.bias);
	ui->get_element_by_id("cmp_line_light_sd_soft").cast_form_float(&base->shadow.softness);
	ui->get_element_by_id("cmp_line_light_sd_active").cast_form_boolean(&base->shadow.enabled);
}
void component_surface_light(gui::context* ui, components::surface_light* base, bool changed)
{
	resolve_color3(ui, "cmp_surface_light_refl", &base->diffuse);
	resolve_texture_2d(ui, "cmp_surface_light_source", base->get_diffuse_map() != nullptr, [base](texture_2d* init)
	{
		base->set_diffuse_map(init);
		memory::release(init);
	}, changed);

	ui->get_element_by_id("cmp_surface_light_vo_x").cast_form_float(&base->offset.x);
	ui->get_element_by_id("cmp_surface_light_vo_y").cast_form_float(&base->offset.y);
	ui->get_element_by_id("cmp_surface_light_vo_z").cast_form_float(&base->offset.z);
	ui->get_element_by_id("cmp_surface_light_rd").cast_form_float(&base->tick.delay);
	ui->get_element_by_id("cmp_surface_light_emission").cast_form_float(&base->emission);
	ui->get_element_by_id("cmp_surface_light_inf").cast_form_float(&base->infinity);
	ui->get_element_by_id("cmp_surface_light_px").cast_form_boolean(&base->parallax);
	ui->get_element_by_id("cmp_surface_light_static").cast_form_boolean(&base->static_mask);

	attenuation size = base->get_size();
	if (ui->get_element_by_id("cmp_surface_light_srange").cast_form_float(&size.radius) ||
		ui->get_element_by_id("cmp_surface_light_sc1").cast_form_float(&size.C1) ||
		ui->get_element_by_id("cmp_surface_light_sc2").cast_form_float(&size.C2))
		base->set_size(size);
}
void component_illuminator(gui::context* ui, components::illuminator* base, bool changed)
{
	ui->get_element_by_id("cmp_illuminator_id").cast_form_float(&base->inside.delay);
	ui->get_element_by_id("cmp_illuminator_od").cast_form_float(&base->outside.delay);
	ui->get_element_by_id("cmp_illuminator_rs").cast_form_float(&base->ray_step);
	ui->get_element_by_id("cmp_illuminator_ms").cast_form_float(&base->max_steps);
	ui->get_element_by_id("cmp_illuminator_d").cast_form_float(&base->distance);
	ui->get_element_by_id("cmp_illuminator_i").cast_form_float(&base->radiance);
	ui->get_element_by_id("cmp_illuminator_l").cast_form_float(&base->length);
	ui->get_element_by_id("cmp_illuminator_m").cast_form_float(&base->margin);
	ui->get_element_by_id("cmp_illuminator_off").cast_form_float(&base->offset);
	ui->get_element_by_id("cmp_illuminator_a").cast_form_float(&base->angle);
	ui->get_element_by_id("cmp_illuminator_o").cast_form_float(&base->occlusion);
	ui->get_element_by_id("cmp_illuminator_s").cast_form_float(&base->specular);
	ui->get_element_by_id("cmp_illuminator_b").cast_form_float(&base->bleeding);
}
void component_camera(gui::context* ui, components::camera* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	auto* fRenderer = base->get_renderer();
	bool preview = !app->state.is_camera_active;

	ui->get_element_by_id("cmp_camera_oc").cast_form_boolean(&fRenderer->occlusion_culling);
	ui->get_element_by_id("cmp_camera_fov").cast_form_float(&base->field_of_view);
	ui->get_element_by_id("cmp_camera_w").cast_form_float(&base->width);
	ui->get_element_by_id("cmp_camera_h").cast_form_float(&base->height);
	ui->get_element_by_id("cmp_camera_np").cast_form_float(&base->near_plane);
	ui->get_element_by_id("cmp_camera_fp").cast_form_float(&base->far_plane);
	ui->get_element_by_id("cmp_camera_thd").cast_form_float(&fRenderer->threshold);
	ui->get_element_by_id("cmp_camera_oc_sp1").cast_form_size(&fRenderer->occlusion_skips);
	ui->get_element_by_id("cmp_camera_oc_sp2").cast_form_size(&fRenderer->occluder_skips);
	ui->get_element_by_id("cmp_camera_oc_sp3").cast_form_size(&fRenderer->occludee_skips);
	ui->get_element_by_id("cmp_camera_oc_sp4").cast_form_float(&fRenderer->occludee_scaling);
	ui->get_element_by_id("cmp_camera_oc_mq").cast_form_size(&fRenderer->max_queries);
	ui->get_element_by_id("cmp_camera_oc_thrs").cast_form_float(&fRenderer->threshold);
	ui->get_element_by_id("cmp_camera_oc_ov").cast_form_float(&fRenderer->overflow_visibility);

	if (ui->get_element_by_id("cmp_camera_perspective").is_active())
		base->mode = components::camera::projection_mode::perspective;
	else if (ui->get_element_by_id("cmp_camera_ortho").is_active())
		base->mode = components::camera::projection_mode::orthographic;

	if (ui->get_element_by_id("cmp_camera_preview").cast_form_boolean(&preview))
	{
		if (app->state.camera != nullptr && (!base->is_active() || !preview))
		{
			auto* main = app->state.camera->get_component<components::camera>();
			app->scene->set_camera(app->state.camera);

			auto* fRenderer = main->get_renderer();
			if (app->scene->is_active())
				fRenderer->occlusion_culling = false;
			else
				app->scene->clear_culling();
		}
		else
			app->scene->set_camera(base->get_entity());

		app->state.is_camera_active = !preview && app->state.camera != nullptr;
		if (!base->is_active())
			app->state.is_camera_active = true;
	}
	else if (preview && !base->is_active())
	{
		app->scene->set_camera(app->state.camera);
		app->state.is_camera_active = true;
	}
}
void component_scriptable(gui::context* ui, components::scriptable* base, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	app->state.system->set_boolean("sl_cmp_scriptable_source", !base->get_source().empty());
	resolve_scriptable(ui, "cmp_scriptable_source", base, changed);

	bool typeless = (base->get_invoke_type() == components::scriptable::invoke_type::typeless);
	if (ui->get_element_by_id("cmp_scriptable_ti").cast_form_boolean(&typeless))
		base->set_invocation(typeless ? components::scriptable::invoke_type::typeless : components::scriptable::invoke_type::normal);

	if (ui->get_element_by_id("cmp_scriptable_recompile").is_active())
	{
		app->vm->clear_cache();
		base->load_source();
	}
}