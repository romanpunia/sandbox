#include "components.h"
#include "resolvers.h"
#include "../core/sandbox.h"

void ComponentModel(GUI::Context* UI, Components::Model* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	static Material* LastMaterial = nullptr;
	static MeshBuffer* LastMesh = nullptr;

	if (Changed)
	{
		LastMaterial = nullptr;
		LastMesh = nullptr;
	}

	App->State.System->SetBoolean("sl_cmp_model_source", Base->GetDrawable() && LastMesh != nullptr);
	App->State.System->SetBoolean("sl_cmp_model_assigned", Base->GetDrawable());

	ResolveModel(UI, "cmp_model_source", Base, Changed);
	UI->GetElementById("cmp_model_uv_x").CastFormFloat(&Base->TexCoord.X);
	UI->GetElementById("cmp_model_uv_y").CastFormFloat(&Base->TexCoord.Y);
	UI->GetElementById("cmp_model_static").CastFormBoolean(&Base->Static);

	if (UI->GetElementById("cmp_model_mesh").CastFormPointer((void**)&LastMesh))
		LastMaterial = (LastMesh ? Base->GetMaterial(LastMesh) : nullptr);

	if (LastMesh != nullptr && UI->GetElementById("cmp_model_material").CastFormPointer((void**)&LastMaterial))
		Base->SetMaterial(LastMesh, LastMaterial);

	bool Alpha = (Base->GetCategory() == GeoCategory::Transparent);
	if (UI->GetElementById("cmp_model_alpha").CastFormBoolean(&Alpha))
		Base->SetCategory(Alpha ? GeoCategory::Transparent : GeoCategory::Opaque);
}
void ComponentSkin(GUI::Context* UI, Components::Skin* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	static Material* LastMaterial = nullptr;
	static SkinMeshBuffer* LastMesh = nullptr;
	static int64_t LastJoint = -1;

	if (Changed)
	{
		LastMaterial = nullptr;
		LastMesh = nullptr;
		LastJoint = -1;
	}

	App->State.System->SetBoolean("sl_cmp_skin_source", Base->GetDrawable() && LastMesh != nullptr);
	App->State.System->SetBoolean("sl_cmp_skin_assigned", Base->GetDrawable());
	App->State.System->SetInteger("sl_cmp_skin_joint", LastJoint);

	if (Base->GetDrawable() != nullptr)
	{
		App->State.System->SetInteger("sl_cmp_skin_joints", (int64_t)Base->Skeleton.Offsets.size() - 1);
		UI->GetElementById("cmp_skin_joint").CastFormInt64(&LastJoint);

		if (LastJoint != -1)
		{
			Joint* Current = nullptr;
			if (Base->GetDrawable()->FindJoint(LastJoint, Current))
			{
				String Name = Current->Name + (" (" + ToString(LastJoint) + ")");
				auto& Pose = Base->Skeleton.Offsets[Current->Index];

				UI->GetElementById("cmp_skin_jname").CastFormString(&Name);
				UI->GetElementById("cmp_skin_jp_x").CastFormFloat(&Pose.Offset.Position.X);
				UI->GetElementById("cmp_skin_jp_y").CastFormFloat(&Pose.Offset.Position.Y);
				UI->GetElementById("cmp_skin_jp_z").CastFormFloat(&Pose.Offset.Position.Z);
				UI->GetElementById("cmp_skin_jr_x").CastFormFloat(&Pose.Offset.Rotation.X, Mathf::Rad2Deg());
				UI->GetElementById("cmp_skin_jr_y").CastFormFloat(&Pose.Offset.Rotation.Y, Mathf::Rad2Deg());
				UI->GetElementById("cmp_skin_jr_z").CastFormFloat(&Pose.Offset.Rotation.Z, Mathf::Rad2Deg());
			}
		}
	}
	else
		App->State.System->SetInteger("sl_cmp_skin_joints", 0);

	ResolveSkin(UI, "cmp_skin_source", Base, Changed);
	UI->GetElementById("cmp_skin_uv_x").CastFormFloat(&Base->TexCoord.X);
	UI->GetElementById("cmp_skin_uv_y").CastFormFloat(&Base->TexCoord.Y);
	UI->GetElementById("cmp_skin_static").CastFormBoolean(&Base->Static);

	if (UI->GetElementById("cmp_skin_mesh").CastFormPointer((void**)&LastMesh))
		LastMaterial = (LastMesh ? Base->GetMaterial(LastMesh) : nullptr);

	if (LastMesh != nullptr && UI->GetElementById("cmp_skin_material").CastFormPointer((void**)&LastMaterial))
		Base->SetMaterial(LastMesh, LastMaterial);

	bool Alpha = (Base->GetCategory() == GeoCategory::Transparent);
	if (UI->GetElementById("cmp_skin_alpha").CastFormBoolean(&Alpha))
		Base->SetCategory(Alpha ? GeoCategory::Transparent : GeoCategory::Opaque);
}
void ComponentEmitter(GUI::Context* UI, Components::Emitter* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	static Material* LastMaterial = nullptr;

	if (Changed)
		LastMaterial = nullptr;

	int Count = (int)Base->GetBuffer()->GetArray().size();
	UI->GetElementById("cmp_emitter_min_x").CastFormFloat(&Base->Min.X);
	UI->GetElementById("cmp_emitter_min_y").CastFormFloat(&Base->Min.Y);
	UI->GetElementById("cmp_emitter_min_z").CastFormFloat(&Base->Min.Z);
	UI->GetElementById("cmp_emitter_max_x").CastFormFloat(&Base->Max.X);
	UI->GetElementById("cmp_emitter_max_y").CastFormFloat(&Base->Max.Y);
	UI->GetElementById("cmp_emitter_max_z").CastFormFloat(&Base->Max.Z);
	UI->GetElementById("cmp_emitter_elem_limit").CastFormInt32(&App->State.ElementsLimit);
	UI->GetElementById("cmp_emitter_elem_count").CastFormInt32(&Count);
	UI->GetElementById("cmp_emitter_quad_based").CastFormBoolean(&Base->QuadBased);
	UI->GetElementById("cmp_emitter_connected").CastFormBoolean(&Base->Connected);
	UI->GetElementById("cmp_emitter_static").CastFormBoolean(&Base->Static);

	if (UI->GetElementById("cmp_emitter_refill").IsActive())
	{
		if (App->State.ElementsLimit < 1)
			App->State.ElementsLimit = 1;

		if (App->State.ElementsLimit > 5000000)
			App->State.ElementsLimit = 5000000;

		App->Renderer->UpdateBufferSize(Base->GetBuffer(), App->State.ElementsLimit);
	}

	if (UI->GetElementById("cmp_emitter_material").CastFormPointer((void**)&LastMaterial))
		Base->SetMaterial(nullptr, LastMaterial);

	bool Alpha = (Base->GetCategory() == GeoCategory::Transparent);
	if (UI->GetElementById("cmp_emitter_alpha").CastFormBoolean(&Alpha))
		Base->SetCategory(Alpha ? GeoCategory::Transparent : GeoCategory::Opaque);

	bool Additive = (Base->GetCategory() == GeoCategory::Additive);
	if (UI->GetElementById("cmp_emitter_additive").CastFormBoolean(&Additive))
		Base->SetCategory(Additive ? GeoCategory::Additive : GeoCategory::Opaque);
}
void ComponentDecal(GUI::Context* UI, Components::Decal* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	static Material* LastMaterial = nullptr;

	if (Changed)
		LastMaterial = nullptr;

	UI->GetElementById("cmp_decal_uv_x").CastFormFloat(&Base->TexCoord.X);
	UI->GetElementById("cmp_decal_uv_y").CastFormFloat(&Base->TexCoord.Y);

	if (UI->GetElementById("cmp_decal_material").CastFormPointer((void**)&LastMaterial))
		Base->SetMaterial(nullptr, LastMaterial);

	bool Alpha = (Base->GetCategory() == GeoCategory::Transparent);
	if (UI->GetElementById("cmp_decal_alpha").CastFormBoolean(&Alpha))
		Base->SetCategory(Alpha ? GeoCategory::Transparent : GeoCategory::Opaque);
}
void ComponentSoftBody(GUI::Context* UI, Components::SoftBody* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	static Material* LastMaterial = nullptr;

	if (Changed)
		LastMaterial = nullptr;

	SoftBody* Body = Base->GetBody();
	if (Body != nullptr)
	{
		SoftBody::Desc& Desc = Body->GetInitialState();
		UI->GetElementById("cmp_soft_body_vcf").CastFormFloat(&Desc.Config.VCF);
		UI->GetElementById("cmp_soft_body_dp").CastFormFloat(&Desc.Config.DP);
		UI->GetElementById("cmp_soft_body_dg").CastFormFloat(&Desc.Config.DG);
		UI->GetElementById("cmp_soft_body_lf").CastFormFloat(&Desc.Config.LF);
		UI->GetElementById("cmp_soft_body_pr").CastFormFloat(&Desc.Config.PR);
		UI->GetElementById("cmp_soft_body_vc").CastFormFloat(&Desc.Config.VC);
		UI->GetElementById("cmp_soft_body_df").CastFormFloat(&Desc.Config.DF);
		UI->GetElementById("cmp_soft_body_mt").CastFormFloat(&Desc.Config.MT);
		UI->GetElementById("cmp_soft_body_chr").CastFormFloat(&Desc.Config.CHR);
		UI->GetElementById("cmp_soft_body_khr").CastFormFloat(&Desc.Config.KHR);
		UI->GetElementById("cmp_soft_body_shr").CastFormFloat(&Desc.Config.SHR);
		UI->GetElementById("cmp_soft_body_ahr").CastFormFloat(&Desc.Config.AHR);
		UI->GetElementById("cmp_soft_body_srhr_cl").CastFormFloat(&Desc.Config.SRHR_CL);
		UI->GetElementById("cmp_soft_body_skhr_cl").CastFormFloat(&Desc.Config.SKHR_CL);
		UI->GetElementById("cmp_soft_body_sshr_cl").CastFormFloat(&Desc.Config.SSHR_CL);
		UI->GetElementById("cmp_soft_body_sr_splt_cl").CastFormFloat(&Desc.Config.SR_SPLT_CL);
		UI->GetElementById("cmp_soft_body_sk_splt_cl").CastFormFloat(&Desc.Config.SK_SPLT_CL);
		UI->GetElementById("cmp_soft_body_ss_splt_cl").CastFormFloat(&Desc.Config.SS_SPLT_CL);
		UI->GetElementById("cmp_soft_body_mx_vol").CastFormFloat(&Desc.Config.MaxVolume);
		UI->GetElementById("cmp_soft_body_ts").CastFormFloat(&Desc.Config.TimeScale);
		UI->GetElementById("cmp_soft_body_drag").CastFormFloat(&Desc.Config.Drag);
		UI->GetElementById("cmp_soft_body_mx_strs").CastFormFloat(&Desc.Config.MaxStress);
		UI->GetElementById("cmp_soft_body_cltrs").CastFormInt32(&Desc.Config.Clusters);
		UI->GetElementById("cmp_soft_body_constrs").CastFormInt32(&Desc.Config.Constraints);
		UI->GetElementById("cmp_soft_body_vit").CastFormInt32(&Desc.Config.VIterations);
		UI->GetElementById("cmp_soft_body_pit").CastFormInt32(&Desc.Config.PIterations);
		UI->GetElementById("cmp_soft_body_dit").CastFormInt32(&Desc.Config.DIterations);
		UI->GetElementById("cmp_soft_body_cit").CastFormInt32(&Desc.Config.CIterations);
		UI->GetElementById("cmp_soft_body_kinemat").CastFormBoolean(&Base->Kinematic);

		Vector3 Offset = Body->GetAnisotropicFriction();
		if (UI->GetElementById("cmp_soft_body_af_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_soft_body_af_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_soft_body_af_z").CastFormFloat(&Offset.Z))
			Body->SetAnisotropicFriction(Offset);

		Offset = Body->GetWindVelocity();
		if (UI->GetElementById("cmp_soft_body_wv_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_soft_body_wv_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_soft_body_wv_z").CastFormFloat(&Offset.Z))
			Body->SetWindVelocity(Offset);

		float Value = Body->GetTotalMass();
		if (UI->GetElementById("cmp_soft_body_mass").CastFormFloat(&Value))
			Body->SetTotalMass(Value);

		Value = Body->GetRestitution();
		if (UI->GetElementById("cmp_soft_body_rest").CastFormFloat(&Value))
			Body->SetRestitution(Value);

		Value = Body->GetSpinningFriction();
		if (UI->GetElementById("cmp_soft_body_spinf").CastFormFloat(&Value))
			Body->SetSpinningFriction(Value);

		Value = Body->GetContactDamping();
		if (UI->GetElementById("cmp_soft_body_contdamp").CastFormFloat(&Value))
			Body->SetContactDamping(Value);

		Value = Body->GetContactStiffness();
		if (UI->GetElementById("cmp_soft_body_contstf").CastFormFloat(&Value, 1.0f / 1000000000000.0f))
			Body->SetContactStiffness(Value);

		Value = Body->GetFriction();
		if (UI->GetElementById("cmp_soft_body_fric").CastFormFloat(&Value))
			Body->SetFriction(Value);

		Value = Body->GetHitFraction();
		if (UI->GetElementById("cmp_soft_body_frac").CastFormFloat(&Value))
			Body->SetHitFraction(Value);

		Value = Body->GetCcdMotionThreshold();
		if (UI->GetElementById("cmp_soft_body_mtt").CastFormFloat(&Value))
			Body->SetCcdMotionThreshold(Value);

		Value = Body->GetCcdSweptSphereRadius();
		if (UI->GetElementById("cmp_soft_body_srad").CastFormFloat(&Value))
			Body->SetCcdSweptSphereRadius(Value);

		Value = Body->GetDeactivationTime();
		if (UI->GetElementById("cmp_soft_body_dtime").CastFormFloat(&Value))
			Body->SetDeactivationTime(Value);

		Value = Body->GetRollingFriction();
		if (UI->GetElementById("cmp_soft_body_rfric").CastFormFloat(&Value))
			Body->SetRollingFriction(Value);

		bool Option = Body->IsActive();
		if (UI->GetElementById("cmp_soft_body_active").CastFormBoolean(&Option))
			Body->SetActivity(Option);

		Option = (Body->GetActivationState() != MotionState::Disable_Deactivation);
		if (UI->GetElementById("cmp_soft_body_deact").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetActivationState(MotionState::Active);
			else
				Body->SetActivationState(MotionState::Disable_Deactivation);
		}

		Option = Body->IsGhost();
		if (UI->GetElementById("cmp_soft_body_ghost").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetAsGhost();
			else
				Body->SetAsNormal();
		}

		App->State.System->SetBoolean("sl_cmp_soft_body_source", true);
	}
	else
		App->State.System->SetBoolean("sl_cmp_soft_body_source", false);

	ResolveSoftBody(UI, "cmp_soft_body_source", Base, Changed);
	UI->GetElementById("cmp_soft_body_uv_x").CastFormFloat(&Base->TexCoord.X);
	UI->GetElementById("cmp_soft_body_uv_y").CastFormFloat(&Base->TexCoord.Y);

	if (UI->GetElementById("cmp_soft_body_regen").IsActive())
		Base->Regenerate();

	if (UI->GetElementById("cmp_soft_body_material").CastFormPointer((void**)&LastMaterial))
		Base->SetMaterial(nullptr, LastMaterial);

	bool Alpha = (Base->GetCategory() == GeoCategory::Transparent);
	if (UI->GetElementById("cmp_soft_body_alpha").CastFormBoolean(&Alpha))
		Base->SetCategory(Alpha ? GeoCategory::Transparent : GeoCategory::Opaque);
}
void ComponentSkinAnimator(GUI::Context* UI, Components::SkinAnimator* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	static Components::SkinAnimator* LastBase = nullptr;
	static int64_t Frame = -1, Clip = -1;

	if (LastBase != Base)
	{
		Frame = Clip = -1;
		LastBase = Base;
	}

	SkinAnimation* Animation = Base->GetAnimation();
	App->State.System->SetInteger("sl_cmp_skin_animator_clips", Animation ? (int64_t)Animation->GetClips().size() - 1 : -1);
	App->State.System->SetInteger("sl_cmp_skin_animator_clip", Clip);
	App->State.System->SetInteger("sl_cmp_skin_animator_frame", Frame);
	UI->GetElementById("cmp_skin_animator_clip").CastFormInt64(&Clip);

	String Path = Base->GetPath();
	ResolveSkinAnimator(UI, "cmp_skin_animator_source", Base, Changed);
	if (Path != Base->GetPath())
		LastBase = nullptr;

	UI->GetElementById("cmp_skin_animator_loop").CastFormBoolean(&Base->State.Looped);
	if (UI->GetElementById("cmp_skin_animator_play").IsActive())
		Base->Play(Clip, Frame);

	if (UI->GetElementById("cmp_skin_animator_pause").IsActive())
		Base->Pause();

	if (UI->GetElementById("cmp_skin_animator_stop").IsActive())
		Base->Stop();
}
void ComponentKeyAnimator(GUI::Context* UI, Components::KeyAnimator* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	static Components::KeyAnimator* LastBase = nullptr;
	static int64_t Frame = -1, Clip = -1;

	if (LastBase != Base)
	{
		Frame = Clip = -1;
		LastBase = Base;
	}

	App->State.System->SetInteger("sl_cmp_key_animator_clips", (int64_t)Base->Clips.size() - 1);
	App->State.System->SetInteger("sl_cmp_key_animator_clip", Clip);
	App->State.System->SetInteger("sl_cmp_key_animator_frame", Frame);
	UI->GetElementById("cmp_key_animator_clip").CastFormInt64(&Clip);

	if (Clip >= 0 && Clip < (int64_t)Base->Clips.size())
	{
		auto& IClip = Base->Clips[(size_t)Clip];
		App->State.System->SetInteger("sl_cmp_key_animator_frames", (int64_t)IClip.Keys.size() - 1);
		UI->GetElementById("cmp_key_animator_cname").CastFormString(&IClip.Name);
		UI->GetElementById("cmp_key_animator_cd").CastFormFloat(&IClip.Duration);
		UI->GetElementById("cmp_key_animator_cr").CastFormFloat(&IClip.Rate);
		UI->GetElementById("cmp_key_animator_frame").CastFormInt64(&Frame);

		if (Frame >= 0 && Frame < (int64_t)IClip.Keys.size())
		{
			auto& Current = IClip.Keys[(size_t)Frame];
			UI->GetElementById("cmp_key_animator_fname").CastFormInt64(&Frame);
			UI->GetElementById("cmp_key_animator_fp_x").CastFormFloat(&Current.Position.X);
			UI->GetElementById("cmp_key_animator_fp_y").CastFormFloat(&Current.Position.Y);
			UI->GetElementById("cmp_key_animator_fp_z").CastFormFloat(&Current.Position.Z);
			UI->GetElementById("cmp_key_animator_fr_x").CastFormFloat(&Current.Rotation.X, Mathf::Rad2Deg());
			UI->GetElementById("cmp_key_animator_fr_y").CastFormFloat(&Current.Rotation.Y, Mathf::Rad2Deg());
			UI->GetElementById("cmp_key_animator_fr_z").CastFormFloat(&Current.Rotation.Z, Mathf::Rad2Deg());
			UI->GetElementById("cmp_key_animator_fs_x").CastFormFloat(&Current.Scale.X);
			UI->GetElementById("cmp_key_animator_fs_y").CastFormFloat(&Current.Scale.Y);
			UI->GetElementById("cmp_key_animator_fs_z").CastFormFloat(&Current.Scale.Z);
			UI->GetElementById("cmp_key_animator_ft").CastFormFloat(&Current.Time);

			if (UI->GetElementById("cmp_key_animator_frem").IsActive())
			{
				IClip.Keys.erase(IClip.Keys.begin() + (size_t)Frame);
				Frame = -1;
			}
		}

		if (UI->GetElementById("cmp_key_animator_cnorm").IsActive())
		{
			for (size_t i = 0; i < IClip.Keys.size(); i++)
				IClip.Keys[i].Rotation = IClip.Keys[i].Rotation;
		}

		if (UI->GetElementById("cmp_key_animator_cadde").IsActive())
		{
			AnimatorKey Key;
			Key.Scale = 1.0f;
			Key.Position = 0.0f;
			Key.Rotation = Quaternion();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById("cmp_key_animator_caddm").IsActive())
		{
			auto* fTransform = Base->GetEntity()->GetTransform();
			AnimatorKey Key;
			Key.Scale = fTransform->GetScale();
			Key.Position = fTransform->GetPosition();
			Key.Rotation = fTransform->GetRotation().rLerp();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById("cmp_key_animator_caddm").IsActive())
		{
			auto* fTransform = App->Scene->GetCamera()->GetEntity()->GetTransform();
			AnimatorKey Key;
			Key.Scale = fTransform->GetScale();
			Key.Position = fTransform->GetPosition();
			Key.Rotation = fTransform->GetRotation().rLerp();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById("cmp_key_animator_caddm").IsActive())
		{
			auto* fTransform = App->Scene->GetCamera()->GetEntity()->GetTransform();
			AnimatorKey Key;
			Key.Scale = fTransform->GetScale();
			Key.Position = fTransform->GetPosition();
			Key.Rotation = Quaternion();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById("cmp_key_animator_crem").IsActive())
		{
			IClip.Keys.clear();
			Clip = Frame = -1;
		}
	}
	else
		App->State.System->SetInteger("sl_cmp_key_animator_frames", -1);

	if (UI->GetElementById("cmp_key_animator_cap").IsActive())
		Base->Clips.emplace_back();

	String Path = Base->GetPath();
	ResolveKeyAnimator(UI, "cmp_key_animator_source", Base, Changed);
	if (Path != Base->GetPath())
		LastBase = nullptr;

	UI->GetElementById("cmp_key_animator_loop").CastFormBoolean(&Base->State.Looped);
	UI->GetElementById("cmp_key_animator_vpaths").CastFormBoolean(&App->State.IsPathTracked);

	if (UI->GetElementById("cmp_key_animator_play").IsActive())
		Base->Play(Clip, Frame);

	if (UI->GetElementById("cmp_key_animator_pause").IsActive())
		Base->Pause();

	if (UI->GetElementById("cmp_key_animator_stop").IsActive())
		Base->Stop();
}
void ComponentEmitterAnimator(GUI::Context* UI, Components::EmitterAnimator* Base, bool Changed)
{
	UI->GetElementById("cmp_emitter_animator_ddx").CastFormFloat(&Base->Diffuse.X);
	UI->GetElementById("cmp_emitter_animator_ddy").CastFormFloat(&Base->Diffuse.Y);
	UI->GetElementById("cmp_emitter_animator_ddz").CastFormFloat(&Base->Diffuse.Z);
	UI->GetElementById("cmp_emitter_animator_ddw").CastFormFloat(&Base->Diffuse.W);
	UI->GetElementById("cmp_emitter_animator_dpx").CastFormFloat(&Base->Position.X);
	UI->GetElementById("cmp_emitter_animator_dpy").CastFormFloat(&Base->Position.Y);
	UI->GetElementById("cmp_emitter_animator_dpz").CastFormFloat(&Base->Position.Z);
	UI->GetElementById("cmp_emitter_animator_dvx").CastFormFloat(&Base->Velocity.X);
	UI->GetElementById("cmp_emitter_animator_dvy").CastFormFloat(&Base->Velocity.Y);
	UI->GetElementById("cmp_emitter_animator_dvz").CastFormFloat(&Base->Velocity.Z);
	UI->GetElementById("cmp_emitter_animator_drs").CastFormFloat(&Base->RotationSpeed, Mathf::Rad2Deg());
	UI->GetElementById("cmp_emitter_animator_dss").CastFormFloat(&Base->ScaleSpeed);
	UI->GetElementById("cmp_emitter_animator_dns").CastFormFloat(&Base->Noise);
	UI->GetElementById("cmp_emitter_animator_it").CastFormInt32(&Base->Spawner.Iterations);
	UI->GetElementById("cmp_emitter_animator_active").CastFormBoolean(&Base->Simulate);
	UI->GetElementById("cmp_emitter_animator_fmin_dx").CastFormFloat(&Base->Spawner.Diffusion.Min.X);
	UI->GetElementById("cmp_emitter_animator_fmin_dy").CastFormFloat(&Base->Spawner.Diffusion.Min.Y);
	UI->GetElementById("cmp_emitter_animator_fmin_dz").CastFormFloat(&Base->Spawner.Diffusion.Min.Z);
	UI->GetElementById("cmp_emitter_animator_fmin_dw").CastFormFloat(&Base->Spawner.Diffusion.Min.W);
	UI->GetElementById("cmp_emitter_animator_fmax_dx").CastFormFloat(&Base->Spawner.Diffusion.Max.X);
	UI->GetElementById("cmp_emitter_animator_fmax_dy").CastFormFloat(&Base->Spawner.Diffusion.Max.Y);
	UI->GetElementById("cmp_emitter_animator_fmax_dz").CastFormFloat(&Base->Spawner.Diffusion.Max.Z);
	UI->GetElementById("cmp_emitter_animator_fmax_dw").CastFormFloat(&Base->Spawner.Diffusion.Max.W);
	UI->GetElementById("cmp_emitter_animator_facc_d").CastFormFloat(&Base->Spawner.Diffusion.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_d").CastFormBoolean(&Base->Spawner.Diffusion.Intensity);
	UI->GetElementById("cmp_emitter_animator_fmin_px").CastFormFloat(&Base->Spawner.Position.Min.X);
	UI->GetElementById("cmp_emitter_animator_fmin_py").CastFormFloat(&Base->Spawner.Position.Min.Y);
	UI->GetElementById("cmp_emitter_animator_fmin_pz").CastFormFloat(&Base->Spawner.Position.Min.Z);
	UI->GetElementById("cmp_emitter_animator_fmax_px").CastFormFloat(&Base->Spawner.Position.Max.X);
	UI->GetElementById("cmp_emitter_animator_fmax_py").CastFormFloat(&Base->Spawner.Position.Max.Y);
	UI->GetElementById("cmp_emitter_animator_fmax_pz").CastFormFloat(&Base->Spawner.Position.Max.Z);
	UI->GetElementById("cmp_emitter_animator_facc_p").CastFormFloat(&Base->Spawner.Position.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_p").CastFormBoolean(&Base->Spawner.Position.Intensity);
	UI->GetElementById("cmp_emitter_animator_fmin_vx").CastFormFloat(&Base->Spawner.Velocity.Min.X);
	UI->GetElementById("cmp_emitter_animator_fmin_vy").CastFormFloat(&Base->Spawner.Velocity.Min.Y);
	UI->GetElementById("cmp_emitter_animator_fmin_vz").CastFormFloat(&Base->Spawner.Velocity.Min.Z);
	UI->GetElementById("cmp_emitter_animator_fmax_vx").CastFormFloat(&Base->Spawner.Velocity.Max.X);
	UI->GetElementById("cmp_emitter_animator_fmax_vy").CastFormFloat(&Base->Spawner.Velocity.Max.Y);
	UI->GetElementById("cmp_emitter_animator_fmax_vz").CastFormFloat(&Base->Spawner.Velocity.Max.Z);
	UI->GetElementById("cmp_emitter_animator_facc_v").CastFormFloat(&Base->Spawner.Velocity.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_v").CastFormBoolean(&Base->Spawner.Velocity.Intensity);
	UI->GetElementById("cmp_emitter_animator_fmin_nx").CastFormFloat(&Base->Spawner.Noise.Min.X);
	UI->GetElementById("cmp_emitter_animator_fmin_ny").CastFormFloat(&Base->Spawner.Noise.Min.Y);
	UI->GetElementById("cmp_emitter_animator_fmin_nz").CastFormFloat(&Base->Spawner.Noise.Min.Z);
	UI->GetElementById("cmp_emitter_animator_fmax_nx").CastFormFloat(&Base->Spawner.Noise.Max.X);
	UI->GetElementById("cmp_emitter_animator_fmax_ny").CastFormFloat(&Base->Spawner.Noise.Max.Y);
	UI->GetElementById("cmp_emitter_animator_fmax_nz").CastFormFloat(&Base->Spawner.Noise.Max.Z);
	UI->GetElementById("cmp_emitter_animator_facc_n").CastFormFloat(&Base->Spawner.Noise.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_n").CastFormBoolean(&Base->Spawner.Noise.Intensity);
	UI->GetElementById("cmp_emitter_animator_fmin_r").CastFormFloat(&Base->Spawner.Rotation.Min, Mathf::Rad2Deg());
	UI->GetElementById("cmp_emitter_animator_fmax_r").CastFormFloat(&Base->Spawner.Rotation.Max, Mathf::Rad2Deg());
	UI->GetElementById("cmp_emitter_animator_facc_r").CastFormFloat(&Base->Spawner.Rotation.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_r").CastFormBoolean(&Base->Spawner.Rotation.Intensity);
	UI->GetElementById("cmp_emitter_animator_fmin_a").CastFormFloat(&Base->Spawner.Angular.Min, Mathf::Rad2Deg());
	UI->GetElementById("cmp_emitter_animator_fmax_a").CastFormFloat(&Base->Spawner.Angular.Max, Mathf::Rad2Deg());
	UI->GetElementById("cmp_emitter_animator_facc_a").CastFormFloat(&Base->Spawner.Angular.Accuracy);
	UI->GetElementById("cmp_emitter_animator_fint_a").CastFormBoolean(&Base->Spawner.Angular.Intensity);
}
void ComponentRigidBody(GUI::Context* UI, Components::RigidBody* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	static Components::RigidBody* LastBase = nullptr;
	if (LastBase != Base)
	{
		Shape Type = (Base->GetBody() ? Base->GetBody()->GetCollisionShapeType() : Shape::Invalid);
		if (Type == Shape::Box)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("box");
		else if (Type == Shape::Sphere)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("sphere");
		else if (Type == Shape::Capsule)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("capsule");
		else if (Type == Shape::Cone)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("cone");
		else if (Type == Shape::Cylinder)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("cylinder");
		else if (Type == Shape::Convex_Hull)
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("other");
		else
			UI->GetElementById("cmp_rigid_body_shape").SetFormValue("none");

		LastBase = Base;
	}

	String Shape = UI->GetElementById("cmp_rigid_body_shape").GetFormValue();
	if (Shape == "other")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", true);
		ResolveRigidBody(UI, "cmp_rigid_body_source", Base, Changed);

		if (Base->GetBody() && Base->GetBody()->GetCollisionShapeType() != Shape::Convex_Hull)
			Base->Clear();
	}
	else if (Shape == "none")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (Base->GetBody() && Base->GetBody()->GetCollisionShapeType() != Shape::Invalid)
			Base->Clear();
	}
	else if (Shape == "box")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape::Box)
			Base->Load(App->Scene->GetSimulator()->CreateCube(), 0, 0);
	}
	else if (Shape == "sphere")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape::Sphere)
			Base->Load(App->Scene->GetSimulator()->CreateSphere(), 0, 0);
	}
	else if (Shape == "capsule")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape::Capsule)
			Base->Load(App->Scene->GetSimulator()->CreateCapsule(), 0, 0);
	}
	else if (Shape == "cone")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape::Cone)
			Base->Load(App->Scene->GetSimulator()->CreateCone(), 0, 0);
	}
	else if (Shape == "cylinder")
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape::Cylinder)
			Base->Load(App->Scene->GetSimulator()->CreateCylinder(), 0, 0);
	}

	RigidBody* Body = Base->GetBody();
	if (Body != nullptr)
	{
		App->State.System->SetBoolean("sl_cmp_rigid_body_source", true);
		UI->GetElementById("cmp_soft_body_kinemat").CastFormBoolean(&Base->Kinematic);

		Vector3 Offset = Body->GetLinearVelocity();
		if (UI->GetElementById("cmp_rigid_body_v_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_v_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_v_z").CastFormFloat(&Offset.Z))
			Body->SetLinearVelocity(Offset);

		Offset = Body->GetAngularVelocity();
		if (UI->GetElementById("cmp_rigid_body_t_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_t_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_t_z").CastFormFloat(&Offset.Z))
			Body->SetAngularVelocity(Offset);

		Offset = Body->GetGravity();
		if (UI->GetElementById("cmp_rigid_body_g_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_g_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_g_z").CastFormFloat(&Offset.Z))
			Body->SetGravity(Offset);

		Offset = Body->GetAngularFactor();
		if (UI->GetElementById("cmp_rigid_body_af_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_af_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_af_z").CastFormFloat(&Offset.Z))
			Body->SetAngularFactor(Offset);

		Offset = Body->GetLinearFactor();
		if (UI->GetElementById("cmp_rigid_body_lf_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_lf_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_lf_z").CastFormFloat(&Offset.Z))
			Body->SetLinearFactor(Offset);

		Offset = Body->GetAnisotropicFriction();
		if (UI->GetElementById("cmp_rigid_body_afric_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById("cmp_rigid_body_afric_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById("cmp_rigid_body_afric_z").CastFormFloat(&Offset.Z))
			Body->SetAnisotropicFriction(Offset);

		float Value = Body->GetMass();
		if (UI->GetElementById("cmp_rigid_body_mass").CastFormFloat(&Value))
			Body->SetMass(Value);

		Value = Body->GetAngularDamping();
		if (UI->GetElementById("cmp_rigid_body_ad").CastFormFloat(&Value))
			Body->SetAngularDamping(Value);

		Value = Body->GetLinearDamping();
		if (UI->GetElementById("cmp_rigid_body_ld").CastFormFloat(&Value))
			Body->SetLinearDamping(Value);

		Value = Body->GetContactDamping();
		if (UI->GetElementById("cmp_rigid_body_cd").CastFormFloat(&Value))
			Body->SetContactDamping(Value);

		Value = Body->GetHitFraction();
		if (UI->GetElementById("cmp_rigid_body_frac").CastFormFloat(&Value))
			Body->SetHitFraction(Value);

		Value = Body->GetRestitution();
		if (UI->GetElementById("cmp_rigid_body_rest").CastFormFloat(&Value))
			Body->SetRestitution(Value);

		Value = Body->GetFriction();
		if (UI->GetElementById("cmp_rigid_body_fric").CastFormFloat(&Value))
			Body->SetFriction(Value);

		Value = Body->GetSpinningFriction();
		if (UI->GetElementById("cmp_rigid_body_spinf").CastFormFloat(&Value))
			Body->SetSpinningFriction(Value);

		Value = Body->GetRollingFriction();
		if (UI->GetElementById("cmp_rigid_body_rollf").CastFormFloat(&Value))
			Body->SetRollingFriction(Value);

		Value = Body->GetContactStiffness();
		if (UI->GetElementById("cmp_rigid_body_stiff").CastFormFloat(&Value, 1.0f / 1000000000000.0f))
			Body->SetContactStiffness(Value);

		Value = Body->GetAngularSleepingThreshold();
		if (UI->GetElementById("cmp_rigid_body_ath").CastFormFloat(&Value))
			Body->SetAngularSleepingThreshold(Value);

		Value = Body->GetLinearSleepingThreshold();
		if (UI->GetElementById("cmp_rigid_body_lth").CastFormFloat(&Value))
			Body->SetLinearSleepingThreshold(Value);

		Value = Body->GetCcdMotionThreshold();
		if (UI->GetElementById("cmp_rigid_body_mth").CastFormFloat(&Value))
			Body->SetCcdMotionThreshold(Value);

		Value = Body->GetCcdSweptSphereRadius();
		if (UI->GetElementById("cmp_rigid_body_srad").CastFormFloat(&Value))
			Body->SetCcdSweptSphereRadius(Value);

		Value = Body->GetDeactivationTime();
		if (UI->GetElementById("cmp_rigid_body_dtime").CastFormFloat(&Value))
			Body->SetDeactivationTime(Value);

		bool Option = Body->IsActive();
		if (UI->GetElementById("cmp_rigid_body_active").CastFormBoolean(&Option))
			Body->SetActivity(Option);

		Option = (Body->GetActivationState() != MotionState::Disable_Deactivation);
		if (UI->GetElementById("cmp_rigid_body_deact").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetActivationState(MotionState::Active);
			else
				Body->SetActivationState(MotionState::Disable_Deactivation);
		}

		Option = Body->IsGhost();
		if (UI->GetElementById("cmp_rigid_body_ghost").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetAsGhost();
			else
				Body->SetAsNormal();
		}
	}
	else
		App->State.System->SetBoolean("sl_cmp_rigid_body_source", false);
}
void ComponentAcceleration(GUI::Context* UI, Components::Acceleration* Base, bool Changed)
{
	UI->GetElementById("cmp_acceleration_av_x").CastFormFloat(&Base->AmplitudeVelocity.X);
	UI->GetElementById("cmp_acceleration_av_y").CastFormFloat(&Base->AmplitudeVelocity.Y);
	UI->GetElementById("cmp_acceleration_av_z").CastFormFloat(&Base->AmplitudeVelocity.Z);
	UI->GetElementById("cmp_acceleration_v_x").CastFormFloat(&Base->ConstantVelocity.X);
	UI->GetElementById("cmp_acceleration_v_y").CastFormFloat(&Base->ConstantVelocity.Y);
	UI->GetElementById("cmp_acceleration_v_z").CastFormFloat(&Base->ConstantVelocity.Z);
	UI->GetElementById("cmp_acceleration_at_x").CastFormFloat(&Base->AmplitudeTorque.X);
	UI->GetElementById("cmp_acceleration_at_y").CastFormFloat(&Base->AmplitudeTorque.Y);
	UI->GetElementById("cmp_acceleration_at_z").CastFormFloat(&Base->AmplitudeTorque.Z);
	UI->GetElementById("cmp_acceleration_t_x").CastFormFloat(&Base->ConstantTorque.X);
	UI->GetElementById("cmp_acceleration_t_y").CastFormFloat(&Base->ConstantTorque.Y);
	UI->GetElementById("cmp_acceleration_t_z").CastFormFloat(&Base->ConstantTorque.Z);
	UI->GetElementById("cmp_acceleration_c_x").CastFormFloat(&Base->ConstantCenter.X);
	UI->GetElementById("cmp_acceleration_c_y").CastFormFloat(&Base->ConstantCenter.Y);
	UI->GetElementById("cmp_acceleration_c_z").CastFormFloat(&Base->ConstantCenter.Z);
	UI->GetElementById("cmp_acceleration_kinemat").CastFormBoolean(&Base->Kinematic);
}
void ComponentSliderConstraint(GUI::Context* UI, Components::SliderConstraint* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	static bool Ghost = true, Linear = true;
	ResolveSliderConstraint(UI, "cmp_slider_constraint_entity", Base, Ghost, Linear, Changed);

	SConstraint* Body = Base->GetConstraint();
	if (Body != nullptr)
	{
		App->State.System->SetBoolean("sl_cmp_slider_constraint_entity", true);

		float Value = Body->GetAngularMotorVelocity();
		if (UI->GetElementById("cmp_slider_constraint_amv").CastFormFloat(&Value))
			Body->SetAngularMotorVelocity(Value);

		Value = Body->GetLinearMotorVelocity();
		if (UI->GetElementById("cmp_slider_constraint_lmv").CastFormFloat(&Value))
			Body->SetLinearMotorVelocity(Value);

		Value = Body->GetUpperLinearLimit();
		if (UI->GetElementById("cmp_slider_constraint_ull").CastFormFloat(&Value))
			Body->SetUpperLinearLimit(Value);

		Value = Body->GetLowerLinearLimit();
		if (UI->GetElementById("cmp_slider_constraint_lll").CastFormFloat(&Value))
			Body->SetLowerLinearLimit(Value);

		Value = Body->GetAngularDampingDirection();
		if (UI->GetElementById("cmp_slider_constraint_add").CastFormFloat(&Value))
			Body->SetAngularDampingDirection(Value);

		Value = Body->GetLinearDampingDirection();
		if (UI->GetElementById("cmp_slider_constraint_ldd").CastFormFloat(&Value))
			Body->SetLinearDampingDirection(Value);

		Value = Body->GetAngularDampingLimit();
		if (UI->GetElementById("cmp_slider_constraint_adl").CastFormFloat(&Value))
			Body->SetAngularDampingLimit(Value);

		Value = Body->GetLinearDampingLimit();
		if (UI->GetElementById("cmp_slider_constraint_ldl").CastFormFloat(&Value))
			Body->SetLinearDampingLimit(Value);

		Value = Body->GetAngularDampingOrtho();
		if (UI->GetElementById("cmp_slider_constraint_ado").CastFormFloat(&Value))
			Body->SetAngularDampingOrtho(Value);

		Value = Body->GetLinearDampingOrtho();
		if (UI->GetElementById("cmp_slider_constraint_ldo").CastFormFloat(&Value))
			Body->SetLinearDampingOrtho(Value);

		Value = Body->GetUpperAngularLimit();
		if (UI->GetElementById("cmp_slider_constraint_ual").CastFormFloat(&Value))
			Body->SetUpperAngularLimit(Value);

		Value = Body->GetLowerAngularLimit();
		if (UI->GetElementById("cmp_slider_constraint_lal").CastFormFloat(&Value))
			Body->SetLowerAngularLimit(Value);

		Value = Body->GetMaxAngularMotorForce();
		if (UI->GetElementById("cmp_slider_constraint_mamf").CastFormFloat(&Value))
			Body->SetMaxAngularMotorForce(Value);

		Value = Body->GetMaxLinearMotorForce();
		if (UI->GetElementById("cmp_slider_constraint_mlmf").CastFormFloat(&Value))
			Body->SetMaxLinearMotorForce(Value);

		Value = Body->GetAngularRestitutionDirection();
		if (UI->GetElementById("cmp_slider_constraint_ard").CastFormFloat(&Value))
			Body->SetAngularRestitutionDirection(Value);

		Value = Body->GetLinearRestitutionDirection();
		if (UI->GetElementById("cmp_slider_constraint_lrd").CastFormFloat(&Value))
			Body->SetLinearRestitutionDirection(Value);

		Value = Body->GetAngularSoftnessDirection();
		if (UI->GetElementById("cmp_slider_constraint_asd").CastFormFloat(&Value))
			Body->SetAngularSoftnessDirection(Value);

		Value = Body->GetLinearSoftnessDirection();
		if (UI->GetElementById("cmp_slider_constraint_lsd").CastFormFloat(&Value))
			Body->SetLinearSoftnessDirection(Value);

		Value = Body->GetAngularSoftnessLimit();
		if (UI->GetElementById("cmp_slider_constraint_asl").CastFormFloat(&Value))
			Body->SetAngularSoftnessLimit(Value);

		Value = Body->GetLinearSoftnessLimit();
		if (UI->GetElementById("cmp_slider_constraint_lsl").CastFormFloat(&Value))
			Body->SetLinearSoftnessLimit(Value);

		Value = Body->GetAngularSoftnessOrtho();
		if (UI->GetElementById("cmp_slider_constraint_aso").CastFormFloat(&Value))
			Body->SetAngularSoftnessOrtho(Value);

		Value = Body->GetLinearSoftnessOrtho();
		if (UI->GetElementById("cmp_slider_constraint_lso").CastFormFloat(&Value))
			Body->SetLinearSoftnessOrtho(Value);

		bool Option = Body->GetPoweredAngularMotor();
		if (UI->GetElementById("cmp_slider_constraint_amp").CastFormBoolean(&Option))
			Body->SetPoweredAngularMotor(Option);

		Option = Body->GetPoweredLinearMotor();
		if (UI->GetElementById("cmp_slider_constraint_lmp").CastFormBoolean(&Option))
			Body->SetPoweredLinearMotor(Option);

		Option = Body->IsEnabled();
		if (UI->GetElementById("cmp_slider_constraint_active").CastFormBoolean(&Option))
			Body->SetEnabled(Option);
	}
	else
	{
		App->State.System->SetBoolean("sl_cmp_slider_constraint_entity", false);
		UI->GetElementById("cmp_slider_constraint_ghost").CastFormBoolean(&Ghost);
		UI->GetElementById("cmp_slider_constraint_linear").CastFormBoolean(&Linear);
	}
}
void ComponentFreeLook(GUI::Context* UI, Components::FreeLook* Base, bool Changed)
{
	ResolveKeyCode(UI, "cmp_free_look_key_rot", &Base->Rotate, Changed);
	UI->GetElementById("cmp_free_look_sens").CastFormFloat(&Base->Sensivity);
}
void ComponentFly(GUI::Context* UI, Components::Fly* Base, bool Changed)
{
	ResolveKeyCode(UI, "cmp_fly_key_forward", &Base->Forward, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_backward", &Base->Backward, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_right", &Base->Right, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_left", &Base->Left, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_up", &Base->Up, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_down", &Base->Down, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_slow", &Base->Slow, Changed);
	ResolveKeyCode(UI, "cmp_fly_key_fast", &Base->Fast, Changed);
	UI->GetElementById("cmp_fly_ax").CastFormFloat(&Base->Moving.Axis.X);
	UI->GetElementById("cmp_fly_ay").CastFormFloat(&Base->Moving.Axis.Y);
	UI->GetElementById("cmp_fly_az").CastFormFloat(&Base->Moving.Axis.Z);
	UI->GetElementById("cmp_fly_fad").CastFormFloat(&Base->Moving.Fading);
	UI->GetElementById("cmp_fly_sp_down").CastFormFloat(&Base->Moving.Slower);
	UI->GetElementById("cmp_fly_sp_normal").CastFormFloat(&Base->Moving.Normal);
	UI->GetElementById("cmp_fly_sp_up").CastFormFloat(&Base->Moving.Faster);
}
void ComponentAudioSource(GUI::Context* UI, Components::AudioSource* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	ResolveAudioSource(UI, "cmp_audio_source_clip", Base, Changed);

	AudioClip* Source = Base->GetSource()->GetClip();
	if (Source != nullptr)
	{
		auto& Sync = Base->GetSync();
		App->State.System->SetBoolean("sl_cmp_audio_source_clip", true);
		UI->GetElementById("cmp_audio_source_gain").CastFormFloat(&Sync.Gain);
		UI->GetElementById("cmp_audio_source_pitch").CastFormFloat(&Sync.Pitch);
		UI->GetElementById("cmp_audio_source_dx").CastFormFloat(&Sync.Direction.X);
		UI->GetElementById("cmp_audio_source_dy").CastFormFloat(&Sync.Direction.Y);
		UI->GetElementById("cmp_audio_source_dz").CastFormFloat(&Sync.Direction.Z);
		UI->GetElementById("cmp_audio_source_cia").CastFormFloat(&Sync.ConeInnerAngle);
		UI->GetElementById("cmp_audio_source_coa").CastFormFloat(&Sync.ConeOuterAngle);
		UI->GetElementById("cmp_audio_source_cog").CastFormFloat(&Sync.ConeOuterGain);
		UI->GetElementById("cmp_audio_source_aa").CastFormFloat(&Sync.AirAbsorption);
		UI->GetElementById("cmp_audio_source_rro").CastFormFloat(&Sync.RoomRollOff);
		UI->GetElementById("cmp_audio_source_rr").CastFormFloat(&Sync.Rolloff);
		UI->GetElementById("cmp_audio_source_dist").CastFormFloat(&Sync.Distance);
		UI->GetElementById("cmp_audio_source_rdist").CastFormFloat(&Sync.RefDistance);
		UI->GetElementById("cmp_audio_source_loop").CastFormBoolean(&Sync.IsLooped);
		UI->GetElementById("cmp_audio_source_3d").CastFormBoolean(&Sync.IsRelative);

		if (UI->GetElementById("cmp_audio_source_pos").CastFormFloat(&Sync.Position))
			Base->ApplyPlayingPosition();

		float Length = Source->Length();
		UI->GetElementById("cmp_audio_source_length").CastFormFloat(&Length);
		App->State.System->SetFloat("sl_cmp_audio_source_length", Length);

		unsigned int Buffer = Source->GetBuffer();
		UI->GetElementById("cmp_audio_source_buffer").CastFormUInt32(&Buffer);

		int Format = Source->GetFormat();
		UI->GetElementById("cmp_audio_source_format").CastFormInt32(&Format);

		bool Mono = Source->IsMono();
		UI->GetElementById("cmp_audio_source_mono").CastFormBoolean(&Mono);

		if (UI->GetElementById("cmp_audio_source_play").IsActive())
			Base->GetSource()->Play();

		if (UI->GetElementById("cmp_audio_source_pause").IsActive())
			Base->GetSource()->Pause();

		if (UI->GetElementById("cmp_audio_source_stop").IsActive())
			Base->GetSource()->Stop();
	}
	else
	{
		App->State.System->SetBoolean("sl_cmp_audio_source_clip", false);
		App->State.System->SetFloat("sl_cmp_audio_source_length", 0.0f);
	}
}
void ComponentAudioListener(GUI::Context* UI, Components::AudioListener* Base, bool Changed)
{
	UI->GetElementById("cmp_audio_listener_gain").CastFormFloat(&Base->Gain);
}
void ComponentPointLight(GUI::Context* UI, Components::PointLight* Base, bool Changed)
{
	ResolveColor3(UI, "cmp_point_light_diffuse", &Base->Diffuse);
	UI->GetElementById("cmp_point_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById("cmp_point_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById("cmp_point_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById("cmp_point_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById("cmp_point_light_sd_dist").CastFormFloat(&Base->Shadow.Distance);
	UI->GetElementById("cmp_point_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById("cmp_point_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);

	Attenuation Size = Base->GetSize();
	if (UI->GetElementById("cmp_point_light_srange").CastFormFloat(&Size.Radius) ||
		UI->GetElementById("cmp_point_light_sc1").CastFormFloat(&Size.C1) ||
		UI->GetElementById("cmp_point_light_sc2").CastFormFloat(&Size.C2))
		Base->SetSize(Size);
}
void ComponentSpotLight(GUI::Context* UI, Components::SpotLight* Base, bool Changed)
{
	ResolveColor3(UI, "cmp_spot_light_diffuse", &Base->Diffuse);
	UI->GetElementById("cmp_spot_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById("cmp_spot_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById("cmp_spot_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById("cmp_spot_light_cutoff").CastFormFloat(&Base->Cutoff);
	UI->GetElementById("cmp_spot_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById("cmp_spot_light_sd_dist").CastFormFloat(&Base->Shadow.Distance);
	UI->GetElementById("cmp_spot_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById("cmp_spot_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);

	Attenuation Size = Base->GetSize();
	if (UI->GetElementById("cmp_spot_light_srange").CastFormFloat(&Size.Radius) ||
		UI->GetElementById("cmp_spot_light_sc1").CastFormFloat(&Size.C1) ||
		UI->GetElementById("cmp_spot_light_sc2").CastFormFloat(&Size.C2))
		Base->SetSize(Size);
}
void ComponentLineLight(GUI::Context* UI, Components::LineLight* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	App->State.System->SetInteger("sl_cmp_line_light_cascades", Base->Shadow.Cascades);
	if (UI->GetElementById("cmp_line_light_sd_casc").CastFormUInt32(&Base->Shadow.Cascades))
		Base->Shadow.Cascades = Math<uint32_t>::Clamp(Base->Shadow.Cascades, 0, 6);

	if (UI->GetElementById("cmp_line_light_sd_dist0").CastFormFloat(&Base->Shadow.Distance[0]))
		Base->Shadow.Distance[0] = Mathf::Max(0, Base->Shadow.Distance[0]);

	if (UI->GetElementById("cmp_line_light_sd_dist1").CastFormFloat(&Base->Shadow.Distance[1]))
		Base->Shadow.Distance[1] = Mathf::Max(Base->Shadow.Distance[0] + 1.0f, Base->Shadow.Distance[1]);

	if (UI->GetElementById("cmp_line_light_sd_dist2").CastFormFloat(&Base->Shadow.Distance[2]))
		Base->Shadow.Distance[2] = Mathf::Max(Base->Shadow.Distance[1] + 1.0f, Base->Shadow.Distance[2]);

	if (UI->GetElementById("cmp_line_light_sd_dist3").CastFormFloat(&Base->Shadow.Distance[3]))
		Base->Shadow.Distance[3] = Mathf::Max(Base->Shadow.Distance[2] + 1.0f, Base->Shadow.Distance[3]);

	if (UI->GetElementById("cmp_line_light_sd_dist4").CastFormFloat(&Base->Shadow.Distance[4]))
		Base->Shadow.Distance[4] = Mathf::Max(Base->Shadow.Distance[3] + 1.0f, Base->Shadow.Distance[4]);

	if (UI->GetElementById("cmp_line_light_sd_dist5").CastFormFloat(&Base->Shadow.Distance[5]))
		Base->Shadow.Distance[5] = Mathf::Max(Base->Shadow.Distance[4] + 1.0f, Base->Shadow.Distance[5]);

	ResolveColor3(UI, "cmp_line_light_diffuse", &Base->Diffuse);
	ResolveColor3(UI, "cmp_line_light_rlh", &Base->Sky.RlhEmission);
	ResolveColor3(UI, "cmp_line_light_mie", &Base->Sky.MieEmission);
	UI->GetElementById("cmp_line_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById("cmp_line_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById("cmp_line_light_rlhh").CastFormFloat(&Base->Sky.RlhHeight);
	UI->GetElementById("cmp_line_light_mieh").CastFormFloat(&Base->Sky.MieHeight);
	UI->GetElementById("cmp_line_light_mied").CastFormFloat(&Base->Sky.MieDirection);
	UI->GetElementById("cmp_line_light_scatter").CastFormFloat(&Base->Sky.Intensity);
	UI->GetElementById("cmp_line_light_inner").CastFormFloat(&Base->Sky.InnerRadius);
	UI->GetElementById("cmp_line_light_outer").CastFormFloat(&Base->Sky.OuterRadius);
	UI->GetElementById("cmp_line_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById("cmp_line_light_sd_far").CastFormFloat(&Base->Shadow.Far);
	UI->GetElementById("cmp_line_light_sd_near").CastFormFloat(&Base->Shadow.Near);
	UI->GetElementById("cmp_line_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById("cmp_line_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById("cmp_line_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);
}
void ComponentSurfaceLight(GUI::Context* UI, Components::SurfaceLight* Base, bool Changed)
{
	ResolveColor3(UI, "cmp_surface_light_refl", &Base->Diffuse);
	ResolveTexture2D(UI, "cmp_surface_light_source", Base->GetDiffuseMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetDiffuseMap(New);
		Memory::Release(New);
	}, Changed);

	UI->GetElementById("cmp_surface_light_vo_x").CastFormFloat(&Base->Offset.X);
	UI->GetElementById("cmp_surface_light_vo_y").CastFormFloat(&Base->Offset.Y);
	UI->GetElementById("cmp_surface_light_vo_z").CastFormFloat(&Base->Offset.Z);
	UI->GetElementById("cmp_surface_light_rd").CastFormFloat(&Base->Tick.Delay);
	UI->GetElementById("cmp_surface_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById("cmp_surface_light_inf").CastFormFloat(&Base->Infinity);
	UI->GetElementById("cmp_surface_light_px").CastFormBoolean(&Base->Parallax);
	UI->GetElementById("cmp_surface_light_static").CastFormBoolean(&Base->StaticMask);

	Attenuation Size = Base->GetSize();
	if (UI->GetElementById("cmp_surface_light_srange").CastFormFloat(&Size.Radius) ||
		UI->GetElementById("cmp_surface_light_sc1").CastFormFloat(&Size.C1) ||
		UI->GetElementById("cmp_surface_light_sc2").CastFormFloat(&Size.C2))
		Base->SetSize(Size);
}
void ComponentIlluminator(GUI::Context* UI, Components::Illuminator* Base, bool Changed)
{
	UI->GetElementById("cmp_illuminator_id").CastFormFloat(&Base->Inside.Delay);
	UI->GetElementById("cmp_illuminator_od").CastFormFloat(&Base->Outside.Delay);
	UI->GetElementById("cmp_illuminator_rs").CastFormFloat(&Base->RayStep);
	UI->GetElementById("cmp_illuminator_ms").CastFormFloat(&Base->MaxSteps);
	UI->GetElementById("cmp_illuminator_d").CastFormFloat(&Base->Distance);
	UI->GetElementById("cmp_illuminator_i").CastFormFloat(&Base->Radiance);
	UI->GetElementById("cmp_illuminator_l").CastFormFloat(&Base->Length);
	UI->GetElementById("cmp_illuminator_m").CastFormFloat(&Base->Margin);
	UI->GetElementById("cmp_illuminator_off").CastFormFloat(&Base->Offset);
	UI->GetElementById("cmp_illuminator_a").CastFormFloat(&Base->Angle);
	UI->GetElementById("cmp_illuminator_o").CastFormFloat(&Base->Occlusion);
	UI->GetElementById("cmp_illuminator_s").CastFormFloat(&Base->Specular);
	UI->GetElementById("cmp_illuminator_b").CastFormFloat(&Base->Bleeding);
}
void ComponentCamera(GUI::Context* UI, Components::Camera* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	auto* fRenderer = Base->GetRenderer();
	bool Preview = !App->State.IsCameraActive;

	UI->GetElementById("cmp_camera_oc").CastFormBoolean(&fRenderer->OcclusionCulling);
	UI->GetElementById("cmp_camera_fov").CastFormFloat(&Base->FieldOfView);
	UI->GetElementById("cmp_camera_w").CastFormFloat(&Base->Width);
	UI->GetElementById("cmp_camera_h").CastFormFloat(&Base->Height);
	UI->GetElementById("cmp_camera_np").CastFormFloat(&Base->NearPlane);
	UI->GetElementById("cmp_camera_fp").CastFormFloat(&Base->FarPlane);
	UI->GetElementById("cmp_camera_thd").CastFormFloat(&fRenderer->Threshold);
	UI->GetElementById("cmp_camera_oc_sp1").CastFormSize(&fRenderer->OcclusionSkips);
	UI->GetElementById("cmp_camera_oc_sp2").CastFormSize(&fRenderer->OccluderSkips);
	UI->GetElementById("cmp_camera_oc_sp3").CastFormSize(&fRenderer->OccludeeSkips);
	UI->GetElementById("cmp_camera_oc_sp4").CastFormFloat(&fRenderer->OccludeeScaling);
	UI->GetElementById("cmp_camera_oc_mq").CastFormSize(&fRenderer->MaxQueries);
	UI->GetElementById("cmp_camera_oc_thrs").CastFormFloat(&fRenderer->Threshold);
	UI->GetElementById("cmp_camera_oc_ov").CastFormFloat(&fRenderer->OverflowVisibility);

	if (UI->GetElementById("cmp_camera_perspective").IsActive())
		Base->Mode = Components::Camera::ProjectionMode::Perspective;
	else if (UI->GetElementById("cmp_camera_ortho").IsActive())
		Base->Mode = Components::Camera::ProjectionMode::Orthographic;

	if (UI->GetElementById("cmp_camera_preview").CastFormBoolean(&Preview))
	{
		if (App->State.Camera != nullptr && (!Base->IsActive() || !Preview))
		{
			auto* Main = App->State.Camera->GetComponent<Components::Camera>();
			App->Scene->SetCamera(App->State.Camera);

			auto* fRenderer = Main->GetRenderer();
			if (App->Scene->IsActive())
				fRenderer->OcclusionCulling = false;
			else
				App->Scene->ClearCulling();
		}
		else
			App->Scene->SetCamera(Base->GetEntity());

		App->State.IsCameraActive = !Preview && App->State.Camera != nullptr;
		if (!Base->IsActive())
			App->State.IsCameraActive = true;
	}
	else if (Preview && !Base->IsActive())
	{
		App->Scene->SetCamera(App->State.Camera);
		App->State.IsCameraActive = true;
	}
}
void ComponentScriptable(GUI::Context* UI, Components::Scriptable* Base, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	App->State.System->SetBoolean("sl_cmp_scriptable_source", !Base->GetSource().empty());
	ResolveScriptable(UI, "cmp_scriptable_source", Base, Changed);

	bool Typeless = (Base->GetInvokeType() == Components::Scriptable::InvokeType::Typeless);
	if (UI->GetElementById("cmp_scriptable_ti").CastFormBoolean(&Typeless))
		Base->SetInvocation(Typeless ? Components::Scriptable::InvokeType::Typeless : Components::Scriptable::InvokeType::Normal);

	if (UI->GetElementById("cmp_scriptable_recompile").IsActive())
	{
		App->VM->ClearCache();
		Base->LoadSource();
	}
}