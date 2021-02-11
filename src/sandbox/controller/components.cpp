#include "components.h"
#include "resolvers.h"
#include "../core/sandbox.h"

void ComponentModel(GUI::Context* UI, Components::Model* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static Model* LastBase = nullptr;
	if (LastBase != Base->GetDrawable())
	{
		App->Models.Model->Update(nullptr);
		LastBase = Base->GetDrawable();
	}

	GUI::IElement Mesh = UI->GetElementById(0, "cmp_model_mesh");
	if (!Mesh.IsValid())
		return;

	std::string Ptr = Mesh.GetFormValue();
	App->Models.System->SetBoolean("sl_cmp_model_assigned", Base->GetDrawable());

	if (Base->GetDrawable() && !Ptr.empty() && Ptr != "0")
	{
		MeshBuffer* Buffer = (MeshBuffer*)(void*)(intptr_t)Stroke(&Ptr).ToInt64();
		App->Models.System->SetBoolean("sl_cmp_model_source", true);

		if (Buffer != nullptr)
			ResolveAppearance(UI, "cmp_model", Base->GetSurface(Buffer));
	}
	else
		App->Models.System->SetBoolean("sl_cmp_model_source", false);

	ResolveModel(UI, "cmp_model_source", Base);
	UI->GetElementById(0, "cmp_model_static").CastFormBoolean(&Base->Static);

	bool Alpha = Base->HasTransparency();
	if (UI->GetElementById(0, "cmp_model_alpha").CastFormBoolean(&Alpha))
		Base->SetTransparency(Alpha);
}
void ComponentSkin(GUI::Context* UI, Components::Skin* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static SkinModel* LastBase = nullptr;
	static int64_t Joint = -1;

	if (LastBase != Base->GetDrawable())
	{
		App->Models.Skin->Update(nullptr);
		LastBase = Base->GetDrawable();
		Joint = -1;
	}

	GUI::IElement Mesh = UI->GetElementById(0, "cmp_skin_mesh");
	if (!Mesh.IsValid())
		return;

	App->Models.System->SetInteger("sl_cmp_skin_joint", Joint);
	if (Base->GetDrawable() != nullptr)
	{
		App->Models.System->SetInteger("sl_cmp_skin_joints", (int64_t)Base->Skeleton.Pose.size() - 1);
		UI->GetElementById(0, "cmp_skin_joint").CastFormInt64(&Joint);

		if (Joint != -1)
		{
			auto Current = Base->Skeleton.Pose.find(Joint);
			if (Current != Base->Skeleton.Pose.end())
			{
				auto* Sub = Base->GetDrawable()->FindJoint(Joint);
				std::string Name = (Sub ? Sub->Name : "Unnamed") + (" (" + std::to_string(Joint) + ")");

				UI->GetElementById(0, "cmp_skin_jname").CastFormString(&Name);
				UI->GetElementById(0, "cmp_skin_jp_x").CastFormFloat(&Current->second.Position.X);
				UI->GetElementById(0, "cmp_skin_jp_y").CastFormFloat(&Current->second.Position.Y);
				UI->GetElementById(0, "cmp_skin_jp_z").CastFormFloat(&Current->second.Position.Z);
				UI->GetElementById(0, "cmp_skin_jr_x").CastFormFloat(&Current->second.Rotation.X, Mathf::Rad2Deg());
				UI->GetElementById(0, "cmp_skin_jr_y").CastFormFloat(&Current->second.Rotation.Y, Mathf::Rad2Deg());
				UI->GetElementById(0, "cmp_skin_jr_z").CastFormFloat(&Current->second.Rotation.Z, Mathf::Rad2Deg());
			}
		}
	}
	else
		App->Models.System->SetInteger("sl_cmp_skin_joints", 0);

	std::string Ptr = Mesh.GetFormValue();
	App->Models.System->SetBoolean("sl_cmp_skin_assigned", Base->GetDrawable());

	if (Base->GetDrawable() && !Ptr.empty() && Ptr != "0")
	{
		MeshBuffer* Buffer = (MeshBuffer*)(void*)(intptr_t)Stroke(&Ptr).ToInt64();
		App->Models.System->SetBoolean("sl_cmp_skin_source", true);

		if (Buffer != nullptr)
			ResolveAppearance(UI, "cmp_skin", Base->GetSurface(Buffer));
	}
	else
		App->Models.System->SetBoolean("sl_cmp_skin_source", false);

	ResolveSkin(UI, "cmp_skin_source", Base);
	UI->GetElementById(0, "cmp_skin_static").CastFormBoolean(&Base->Static);

	bool Alpha = Base->HasTransparency();
	if (UI->GetElementById(0, "cmp_skin_alpha").CastFormBoolean(&Alpha))
		Base->SetTransparency(Alpha);
}
void ComponentEmitter(GUI::Context* UI, Components::Emitter* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	int Count = (int)Base->GetBuffer()->GetArray()->Size();
	ResolveAppearance(UI, "cmp_emitter", Base->GetSurface());
	UI->GetElementById(0, "cmp_emitter_vol_x").CastFormFloat(&Base->Volume.X);
	UI->GetElementById(0, "cmp_emitter_vol_y").CastFormFloat(&Base->Volume.Y);
	UI->GetElementById(0, "cmp_emitter_vol_z").CastFormFloat(&Base->Volume.Z);
	UI->GetElementById(0, "cmp_emitter_elem_limit").CastFormInt32(&App->State.ElementsLimit);
	UI->GetElementById(0, "cmp_emitter_elem_count").CastFormInt32(&Count);
	UI->GetElementById(0, "cmp_emitter_quad_based").CastFormBoolean(&Base->QuadBased);
	UI->GetElementById(0, "cmp_emitter_connected").CastFormBoolean(&Base->Connected);
	UI->GetElementById(0, "cmp_emitter_static").CastFormBoolean(&Base->Static);

	if (UI->GetElementById(0, "cmp_emitter_refill").IsActive())
	{
		if (App->State.ElementsLimit < 1)
			App->State.ElementsLimit = 1;

		if (App->State.ElementsLimit > 5000000)
			App->State.ElementsLimit = 5000000;

		App->Renderer->UpdateBufferSize(Base->GetBuffer(), App->State.ElementsLimit);
	}

	bool Alpha = Base->HasTransparency();
	if (UI->GetElementById(0, "cmp_emitter_alpha").CastFormBoolean(&Alpha))
		Base->SetTransparency(Alpha);
}
void ComponentDecal(GUI::Context* UI, Components::Decal* Base)
{
	ResolveAppearance(UI, "cmp_decal", Base->GetSurface());
	UI->GetElementById(0, "cmp_decal_fov").CastFormFloat(&Base->FieldOfView);
	UI->GetElementById(0, "cmp_decal_dist").CastFormFloat(&Base->Distance);

	bool Alpha = Base->HasTransparency();
	if (UI->GetElementById(0, "cmp_decal_alpha").CastFormBoolean(&Alpha))
		Base->SetTransparency(Alpha);
}
void ComponentSoftBody(GUI::Context* UI, Components::SoftBody* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	SoftBody* Body = Base->GetBody();
	if (Body != nullptr)
	{
		SoftBody::Desc& Desc = Body->GetInitialState();
		UI->GetElementById(0, "cmp_soft_body_vcf").CastFormFloat(&Desc.Config.VCF);
		UI->GetElementById(0, "cmp_soft_body_dp").CastFormFloat(&Desc.Config.DP);
		UI->GetElementById(0, "cmp_soft_body_dg").CastFormFloat(&Desc.Config.DG);
		UI->GetElementById(0, "cmp_soft_body_lf").CastFormFloat(&Desc.Config.LF);
		UI->GetElementById(0, "cmp_soft_body_pr").CastFormFloat(&Desc.Config.PR);
		UI->GetElementById(0, "cmp_soft_body_vc").CastFormFloat(&Desc.Config.VC);
		UI->GetElementById(0, "cmp_soft_body_df").CastFormFloat(&Desc.Config.DF);
		UI->GetElementById(0, "cmp_soft_body_mt").CastFormFloat(&Desc.Config.MT);
		UI->GetElementById(0, "cmp_soft_body_chr").CastFormFloat(&Desc.Config.CHR);
		UI->GetElementById(0, "cmp_soft_body_khr").CastFormFloat(&Desc.Config.KHR);
		UI->GetElementById(0, "cmp_soft_body_shr").CastFormFloat(&Desc.Config.SHR);
		UI->GetElementById(0, "cmp_soft_body_ahr").CastFormFloat(&Desc.Config.AHR);
		UI->GetElementById(0, "cmp_soft_body_srhr_cl").CastFormFloat(&Desc.Config.SRHR_CL);
		UI->GetElementById(0, "cmp_soft_body_skhr_cl").CastFormFloat(&Desc.Config.SKHR_CL);
		UI->GetElementById(0, "cmp_soft_body_sshr_cl").CastFormFloat(&Desc.Config.SSHR_CL);
		UI->GetElementById(0, "cmp_soft_body_sr_splt_cl").CastFormFloat(&Desc.Config.SR_SPLT_CL);
		UI->GetElementById(0, "cmp_soft_body_sk_splt_cl").CastFormFloat(&Desc.Config.SK_SPLT_CL);
		UI->GetElementById(0, "cmp_soft_body_ss_splt_cl").CastFormFloat(&Desc.Config.SS_SPLT_CL);
		UI->GetElementById(0, "cmp_soft_body_mx_vol").CastFormFloat(&Desc.Config.MaxVolume);
		UI->GetElementById(0, "cmp_soft_body_ts").CastFormFloat(&Desc.Config.TimeScale);
		UI->GetElementById(0, "cmp_soft_body_drag").CastFormFloat(&Desc.Config.Drag);
		UI->GetElementById(0, "cmp_soft_body_mx_strs").CastFormFloat(&Desc.Config.MaxStress);
		UI->GetElementById(0, "cmp_soft_body_cltrs").CastFormInt32(&Desc.Config.Clusters);
		UI->GetElementById(0, "cmp_soft_body_constrs").CastFormInt32(&Desc.Config.Constraints);
		UI->GetElementById(0, "cmp_soft_body_vit").CastFormInt32(&Desc.Config.VIterations);
		UI->GetElementById(0, "cmp_soft_body_pit").CastFormInt32(&Desc.Config.PIterations);
		UI->GetElementById(0, "cmp_soft_body_dit").CastFormInt32(&Desc.Config.DIterations);
		UI->GetElementById(0, "cmp_soft_body_cit").CastFormInt32(&Desc.Config.CIterations);
		UI->GetElementById(0, "cmp_soft_body_kinemat").CastFormBoolean(&Base->Kinematic);

		Vector3 Offset = Body->GetAnisotropicFriction();
		if (UI->GetElementById(0, "cmp_soft_body_af_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_soft_body_af_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_soft_body_af_z").CastFormFloat(&Offset.Z))
			Body->SetAnisotropicFriction(Offset);

		Offset = Body->GetWindVelocity();
		if (UI->GetElementById(0, "cmp_soft_body_wv_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_soft_body_wv_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_soft_body_wv_z").CastFormFloat(&Offset.Z))
			Body->SetWindVelocity(Offset);

		float Value = Body->GetTotalMass();
		if (UI->GetElementById(0, "cmp_soft_body_mass").CastFormFloat(&Value))
			Body->SetTotalMass(Value);

		Value = Body->GetRestitution();
		if (UI->GetElementById(0, "cmp_soft_body_rest").CastFormFloat(&Value))
			Body->SetRestitution(Value);

		Value = Body->GetSpinningFriction();
		if (UI->GetElementById(0, "cmp_soft_body_spinf").CastFormFloat(&Value))
			Body->SetSpinningFriction(Value);

		Value = Body->GetContactDamping();
		if (UI->GetElementById(0, "cmp_soft_body_contdamp").CastFormFloat(&Value))
			Body->SetContactDamping(Value);

		Value = Body->GetContactStiffness();
		if (UI->GetElementById(0, "cmp_soft_body_contstf").CastFormFloat(&Value, 1.0f / 1000000000000.0f))
			Body->SetContactStiffness(Value);

		Value = Body->GetFriction();
		if (UI->GetElementById(0, "cmp_soft_body_fric").CastFormFloat(&Value))
			Body->SetFriction(Value);

		Value = Body->GetHitFraction();
		if (UI->GetElementById(0, "cmp_soft_body_frac").CastFormFloat(&Value))
			Body->SetHitFraction(Value);

		Value = Body->GetCcdMotionThreshold();
		if (UI->GetElementById(0, "cmp_soft_body_mtt").CastFormFloat(&Value))
			Body->SetCcdMotionThreshold(Value);

		Value = Body->GetCcdSweptSphereRadius();
		if (UI->GetElementById(0, "cmp_soft_body_srad").CastFormFloat(&Value))
			Body->SetCcdSweptSphereRadius(Value);

		Value = Body->GetDeactivationTime();
		if (UI->GetElementById(0, "cmp_soft_body_dtime").CastFormFloat(&Value))
			Body->SetDeactivationTime(Value);

		Value = Body->GetRollingFriction();
		if (UI->GetElementById(0, "cmp_soft_body_rfric").CastFormFloat(&Value))
			Body->SetRollingFriction(Value);

		bool Option = Body->IsActive();
		if (UI->GetElementById(0, "cmp_soft_body_active").CastFormBoolean(&Option))
			Body->SetActivity(Option);

		Option = (Body->GetActivationState() != MotionState_Disable_Deactivation);
		if (UI->GetElementById(0, "cmp_soft_body_deact").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetActivationState(MotionState_Active);
			else
				Body->SetActivationState(MotionState_Disable_Deactivation);
		}

		Option = Body->IsGhost();
		if (UI->GetElementById(0, "cmp_soft_body_ghost").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetAsGhost();
			else
				Body->SetAsNormal();
		}

		App->Models.System->SetBoolean("sl_cmp_soft_body_source", true);
	}
	else
		App->Models.System->SetBoolean("sl_cmp_soft_body_source", false);

	ResolveAppearance(UI, "cmp_soft_body", Base->GetSurface());
	ResolveSoftBody(UI, "cmp_soft_body_source", Base);

	bool Alpha = Base->HasTransparency();
	if (UI->GetElementById(0, "cmp_soft_body_alpha").CastFormBoolean(&Alpha))
		Base->SetTransparency(Alpha);

	if (UI->GetElementById(0, "cmp_soft_body_regen").IsActive())
		Base->Regenerate();
}
void ComponentSkinAnimator(GUI::Context* UI, Components::SkinAnimator* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static Components::SkinAnimator* LastBase = nullptr;
	static int64_t Joint = -1, Frame = -1, Clip = -1;

	if (LastBase != Base)
	{
		Joint = Frame = Clip = -1;
		LastBase = Base;
	}

	App->Models.System->SetInteger("sl_cmp_skin_animator_clips", (int64_t)Base->Clips.size() - 1);
	App->Models.System->SetInteger("sl_cmp_skin_animator_clip", Clip);
	App->Models.System->SetInteger("sl_cmp_skin_animator_frame", Frame);
	App->Models.System->SetInteger("sl_cmp_skin_animator_joint", Joint);
	UI->GetElementById(0, "cmp_skin_animator_clip").CastFormInt64(&Clip);

	if (Clip >= 0 && Clip < Base->Clips.size())
	{
		auto& IClip = Base->Clips[Clip];
		App->Models.System->SetInteger("sl_cmp_skin_animator_frames", (int64_t)IClip.Keys.size() - 1);
		UI->GetElementById(0, "cmp_skin_animator_cname").CastFormString(&IClip.Name);
		UI->GetElementById(0, "cmp_skin_animator_cd").CastFormFloat(&IClip.Duration);
		UI->GetElementById(0, "cmp_skin_animator_cr").CastFormFloat(&IClip.Rate);
		UI->GetElementById(0, "cmp_skin_animator_frame").CastFormInt64(&Frame);

		if (Frame >= 0 && Frame < IClip.Keys.size())
		{
			auto& Array = IClip.Keys[Frame].Pose;
			App->Models.System->SetInteger("sl_cmp_skin_animator_joints", (int64_t)Array.size() - 1);
			UI->GetElementById(0, "cmp_skin_animator_joint").CastFormInt64(&Joint);
			UI->GetElementById(0, "cmp_skin_animator_fname").CastFormInt64(&Frame);

			if (Joint >= 0 && Joint < Array.size())
			{
				auto& Current = Array[Joint];
				auto* Skin = Base->GetSkin();
				auto* Sub = (Skin && Skin->GetDrawable() ? Skin->GetDrawable()->FindJoint(Joint) : nullptr);
				std::string Name = (Sub ? Sub->Name : "Unnamed") + (" (" + std::to_string(Joint) + ")");

				UI->GetElementById(0, "cmp_skin_animator_jname").CastFormString(&Name);
				UI->GetElementById(0, "cmp_skin_animator_jp_x").CastFormFloat(&Current.Position.X);
				UI->GetElementById(0, "cmp_skin_animator_jp_y").CastFormFloat(&Current.Position.Y);
				UI->GetElementById(0, "cmp_skin_animator_jp_z").CastFormFloat(&Current.Position.Z);
				UI->GetElementById(0, "cmp_skin_animator_jr_x").CastFormFloat(&Current.Rotation.X, Mathf::Rad2Deg());
				UI->GetElementById(0, "cmp_skin_animator_jr_y").CastFormFloat(&Current.Rotation.Y, Mathf::Rad2Deg());
				UI->GetElementById(0, "cmp_skin_animator_jr_z").CastFormFloat(&Current.Rotation.Z, Mathf::Rad2Deg());
				UI->GetElementById(0, "cmp_skin_animator_js_x").CastFormFloat(&Current.Scale.X);
				UI->GetElementById(0, "cmp_skin_animator_js_y").CastFormFloat(&Current.Scale.Y);
				UI->GetElementById(0, "cmp_skin_animator_js_z").CastFormFloat(&Current.Scale.Z);
				UI->GetElementById(0, "cmp_skin_animator_jt").CastFormFloat(&Current.Time);
			}

			if (UI->GetElementById(0, "cmp_skin_animator_frem").IsActive())
			{
				IClip.Keys.erase(IClip.Keys.begin() + Frame);
				Frame = Joint = -1;
			}
		}
		else
			App->Models.System->SetInteger("sl_cmp_skin_animator_joints", -1);

		if (UI->GetElementById(0, "cmp_skin_animator_cnorm").IsActive())
		{
			for (size_t i = 0; i < IClip.Keys.size(); i++)
			{
				for (size_t j = 0; j < IClip.Keys[i].Pose.size(); j++)
					IClip.Keys[i].Pose[j].Rotation = IClip.Keys[i].Pose[j].Rotation.SaturateRotation();
			}
		}

		if (UI->GetElementById(0, "cmp_skin_animator_cadde").IsActive())
		{
			SkinAnimatorKey Key;
			Key.Pose.resize(Base->Default.Pose.size());
			Key.Time = Base->Default.Time;

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_skin_animator_caddm").IsActive())
		{
			SkinAnimatorKey Key;
			Base->GetPose(&Key);

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_skin_animator_crem").IsActive())
		{
			Base->Clips.erase(Base->Clips.begin() + Clip);
			Clip = Frame = Joint = -1;
		}
	}
	else
	{
		App->Models.System->SetInteger("sl_cmp_skin_animator_frames", -1);
		App->Models.System->SetInteger("sl_cmp_skin_animator_joints", -1);
	}

	if (UI->GetElementById(0, "cmp_skin_animator_cap").IsActive())
		Base->Clips.push_back(SkinAnimatorClip());

	std::string Path = Base->GetPath();
	ResolveSkinAnimator(UI, "cmp_skin_animator_source", Base);
	if (Path != Base->GetPath())
		LastBase = nullptr;

	UI->GetElementById(0, "cmp_skin_animator_loop").CastFormBoolean(&Base->State.Looped);
	if (UI->GetElementById(0, "cmp_skin_animator_play").IsActive())
		Base->Play(Clip, Frame);

	if (UI->GetElementById(0, "cmp_skin_animator_pause").IsActive())
		Base->Pause();

	if (UI->GetElementById(0, "cmp_skin_animator_stop").IsActive())
		Base->Stop();
}
void ComponentKeyAnimator(GUI::Context* UI, Components::KeyAnimator* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static Components::KeyAnimator* LastBase = nullptr;
	static int64_t Frame = -1, Clip = -1;

	if (LastBase != Base)
	{
		Frame = Clip = -1;
		LastBase = Base;
	}

	App->Models.System->SetInteger("sl_cmp_key_animator_clips", (int64_t)Base->Clips.size() - 1);
	App->Models.System->SetInteger("sl_cmp_key_animator_clip", Clip);
	App->Models.System->SetInteger("sl_cmp_key_animator_frame", Frame);
	UI->GetElementById(0, "cmp_key_animator_clip").CastFormInt64(&Clip);

	if (Clip >= 0 && Clip < Base->Clips.size())
	{
		auto& IClip = Base->Clips[Clip];
		App->Models.System->SetInteger("sl_cmp_key_animator_frames", (int64_t)IClip.Keys.size() - 1);
		UI->GetElementById(0, "cmp_key_animator_cname").CastFormString(&IClip.Name);
		UI->GetElementById(0, "cmp_key_animator_cd").CastFormFloat(&IClip.Duration);
		UI->GetElementById(0, "cmp_key_animator_cr").CastFormFloat(&IClip.Rate);
		UI->GetElementById(0, "cmp_key_animator_frame").CastFormInt64(&Frame);

		if (Frame >= 0 && Frame < IClip.Keys.size())
		{
			auto& Current = IClip.Keys[Frame];
			UI->GetElementById(0, "cmp_key_animator_fname").CastFormInt64(&Frame);
			UI->GetElementById(0, "cmp_key_animator_fp_x").CastFormFloat(&Current.Position.X);
			UI->GetElementById(0, "cmp_key_animator_fp_y").CastFormFloat(&Current.Position.Y);
			UI->GetElementById(0, "cmp_key_animator_fp_z").CastFormFloat(&Current.Position.Z);
			UI->GetElementById(0, "cmp_key_animator_fr_x").CastFormFloat(&Current.Rotation.X, Mathf::Rad2Deg());
			UI->GetElementById(0, "cmp_key_animator_fr_y").CastFormFloat(&Current.Rotation.Y, Mathf::Rad2Deg());
			UI->GetElementById(0, "cmp_key_animator_fr_z").CastFormFloat(&Current.Rotation.Z, Mathf::Rad2Deg());
			UI->GetElementById(0, "cmp_key_animator_fs_x").CastFormFloat(&Current.Scale.X);
			UI->GetElementById(0, "cmp_key_animator_fs_y").CastFormFloat(&Current.Scale.Y);
			UI->GetElementById(0, "cmp_key_animator_fs_z").CastFormFloat(&Current.Scale.Z);
			UI->GetElementById(0, "cmp_key_animator_ft").CastFormFloat(&Current.Time);

			if (UI->GetElementById(0, "cmp_key_animator_frem").IsActive())
			{
				IClip.Keys.erase(IClip.Keys.begin() + Frame);
				Frame = -1;
			}
		}

		if (UI->GetElementById(0, "cmp_key_animator_cnorm").IsActive())
		{
			for (size_t i = 0; i < IClip.Keys.size(); i++)
				IClip.Keys[i].Rotation = IClip.Keys[i].Rotation.SaturateRotation();
		}

		if (UI->GetElementById(0, "cmp_key_animator_cadde").IsActive())
		{
			AnimatorKey Key;
			Key.Scale = 1.0f;
			Key.Position = 0.0f;
			Key.Rotation = 0.0f;

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_key_animator_caddm").IsActive())
		{
			AnimatorKey Key;
			Key.Scale = Base->GetEntity()->Transform->Scale;
			Key.Position = Base->GetEntity()->Transform->Position;
			Key.Rotation = Base->GetEntity()->Transform->Rotation.SaturateRotation();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_key_animator_caddm").IsActive())
		{
			AnimatorKey Key;
			Key.Scale = App->Scene->GetCamera()->GetEntity()->Transform->Scale;
			Key.Position = App->Scene->GetCamera()->GetEntity()->Transform->Position;
			Key.Rotation = App->Scene->GetCamera()->GetEntity()->Transform->Rotation.SaturateRotation();

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_key_animator_caddm").IsActive())
		{
			AnimatorKey Key;
			Key.Scale = App->Scene->GetCamera()->GetEntity()->Transform->Scale;
			Key.Position = App->Scene->GetCamera()->GetEntity()->Transform->Position;
			Key.Rotation = 0;

			IClip.Keys.push_back(Key);
		}

		if (UI->GetElementById(0, "cmp_key_animator_crem").IsActive())
		{
			IClip.Keys.clear();
			Clip = Frame = -1;
		}
	}
	else
		App->Models.System->SetInteger("sl_cmp_key_animator_frames", -1);

	if (UI->GetElementById(0, "cmp_key_animator_cap").IsActive())
		Base->Clips.push_back(KeyAnimatorClip());

	std::string Path = Base->GetPath();
	ResolveKeyAnimator(UI, "cmp_key_animator_source", Base);
	if (Path != Base->GetPath())
		LastBase = nullptr;

	UI->GetElementById(0, "cmp_key_animator_loop").CastFormBoolean(&Base->State.Looped);
	UI->GetElementById(0, "cmp_key_animator_vpaths").CastFormBoolean(&App->State.IsPathTracked);

	if (UI->GetElementById(0, "cmp_key_animator_play").IsActive())
		Base->Play(Clip, Frame);

	if (UI->GetElementById(0, "cmp_key_animator_pause").IsActive())
		Base->Pause();

	if (UI->GetElementById(0, "cmp_key_animator_stop").IsActive())
		Base->Stop();
}
void ComponentEmitterAnimator(GUI::Context* UI, Components::EmitterAnimator* Base)
{
	UI->GetElementById(0, "cmp_emitter_animator_ddx").CastFormFloat(&Base->Diffuse.X);
	UI->GetElementById(0, "cmp_emitter_animator_ddy").CastFormFloat(&Base->Diffuse.Y);
	UI->GetElementById(0, "cmp_emitter_animator_ddz").CastFormFloat(&Base->Diffuse.Z);
	UI->GetElementById(0, "cmp_emitter_animator_ddw").CastFormFloat(&Base->Diffuse.W);
	UI->GetElementById(0, "cmp_emitter_animator_dpx").CastFormFloat(&Base->Position.X);
	UI->GetElementById(0, "cmp_emitter_animator_dpy").CastFormFloat(&Base->Position.Y);
	UI->GetElementById(0, "cmp_emitter_animator_dpz").CastFormFloat(&Base->Position.Z);
	UI->GetElementById(0, "cmp_emitter_animator_dvx").CastFormFloat(&Base->Velocity.X);
	UI->GetElementById(0, "cmp_emitter_animator_dvy").CastFormFloat(&Base->Velocity.Y);
	UI->GetElementById(0, "cmp_emitter_animator_dvz").CastFormFloat(&Base->Velocity.Z);
	UI->GetElementById(0, "cmp_emitter_animator_drs").CastFormFloat(&Base->RotationSpeed, Mathf::Rad2Deg());
	UI->GetElementById(0, "cmp_emitter_animator_dss").CastFormFloat(&Base->ScaleSpeed);
	UI->GetElementById(0, "cmp_emitter_animator_dns").CastFormFloat(&Base->Noise);
	UI->GetElementById(0, "cmp_emitter_animator_it").CastFormInt32(&Base->Spawner.Iterations);
	UI->GetElementById(0, "cmp_emitter_animator_active").CastFormBoolean(&Base->Simulate);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_dx").CastFormFloat(&Base->Spawner.Diffusion.Min.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_dy").CastFormFloat(&Base->Spawner.Diffusion.Min.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_dz").CastFormFloat(&Base->Spawner.Diffusion.Min.Z);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_dw").CastFormFloat(&Base->Spawner.Diffusion.Min.W);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_dx").CastFormFloat(&Base->Spawner.Diffusion.Max.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_dy").CastFormFloat(&Base->Spawner.Diffusion.Max.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_dz").CastFormFloat(&Base->Spawner.Diffusion.Max.Z);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_dw").CastFormFloat(&Base->Spawner.Diffusion.Max.W);
	UI->GetElementById(0, "cmp_emitter_animator_facc_d").CastFormFloat(&Base->Spawner.Diffusion.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_d").CastFormBoolean(&Base->Spawner.Diffusion.Intensity);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_px").CastFormFloat(&Base->Spawner.Position.Min.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_py").CastFormFloat(&Base->Spawner.Position.Min.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_pz").CastFormFloat(&Base->Spawner.Position.Min.Z);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_px").CastFormFloat(&Base->Spawner.Position.Max.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_py").CastFormFloat(&Base->Spawner.Position.Max.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_pz").CastFormFloat(&Base->Spawner.Position.Max.Z);
	UI->GetElementById(0, "cmp_emitter_animator_facc_p").CastFormFloat(&Base->Spawner.Position.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_p").CastFormBoolean(&Base->Spawner.Position.Intensity);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_vx").CastFormFloat(&Base->Spawner.Velocity.Min.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_vy").CastFormFloat(&Base->Spawner.Velocity.Min.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_vz").CastFormFloat(&Base->Spawner.Velocity.Min.Z);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_vx").CastFormFloat(&Base->Spawner.Velocity.Max.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_vy").CastFormFloat(&Base->Spawner.Velocity.Max.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_vz").CastFormFloat(&Base->Spawner.Velocity.Max.Z);
	UI->GetElementById(0, "cmp_emitter_animator_facc_v").CastFormFloat(&Base->Spawner.Velocity.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_v").CastFormBoolean(&Base->Spawner.Velocity.Intensity);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_nx").CastFormFloat(&Base->Spawner.Noise.Min.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_ny").CastFormFloat(&Base->Spawner.Noise.Min.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_nz").CastFormFloat(&Base->Spawner.Noise.Min.Z);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_nx").CastFormFloat(&Base->Spawner.Noise.Max.X);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_ny").CastFormFloat(&Base->Spawner.Noise.Max.Y);
	UI->GetElementById(0, "cmp_emitter_animator_fmax_nz").CastFormFloat(&Base->Spawner.Noise.Max.Z);
	UI->GetElementById(0, "cmp_emitter_animator_facc_n").CastFormFloat(&Base->Spawner.Noise.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_n").CastFormBoolean(&Base->Spawner.Noise.Intensity);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_r").CastFormFloat(&Base->Spawner.Rotation.Min, Mathf::Rad2Deg());
	UI->GetElementById(0, "cmp_emitter_animator_fmax_r").CastFormFloat(&Base->Spawner.Rotation.Max, Mathf::Rad2Deg());
	UI->GetElementById(0, "cmp_emitter_animator_facc_r").CastFormFloat(&Base->Spawner.Rotation.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_r").CastFormBoolean(&Base->Spawner.Rotation.Intensity);
	UI->GetElementById(0, "cmp_emitter_animator_fmin_a").CastFormFloat(&Base->Spawner.Angular.Min, Mathf::Rad2Deg());
	UI->GetElementById(0, "cmp_emitter_animator_fmax_a").CastFormFloat(&Base->Spawner.Angular.Max, Mathf::Rad2Deg());
	UI->GetElementById(0, "cmp_emitter_animator_facc_a").CastFormFloat(&Base->Spawner.Angular.Accuracy);
	UI->GetElementById(0, "cmp_emitter_animator_fint_a").CastFormBoolean(&Base->Spawner.Angular.Intensity);
}
void ComponentRigidBody(GUI::Context* UI, Components::RigidBody* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static Components::RigidBody* LastBase = nullptr;
	if (LastBase != Base)
	{
		int Type = (Base->GetBody() ? Base->GetBody()->GetCollisionShapeType() : Shape_Invalid);
		if (Type == Shape_Box)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("box");
		else if (Type == Shape_Sphere)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("sphere");
		else if (Type == Shape_Capsule)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("capsule");
		else if (Type == Shape_Cone)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("cone");
		else if (Type == Shape_Cylinder)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("cylinder");
		else if (Type == Shape_Convex_Hull)
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("other");
		else
			UI->GetElementById(0, "cmp_rigid_body_shape").SetFormValue("none");

		LastBase = Base;
	}

	std::string Shape = UI->GetElementById(0, "cmp_rigid_body_shape").GetFormValue();
	if (Shape == "other")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", true);
		ResolveRigidBody(UI, "cmp_rigid_body_source", Base);

		if (Base->GetBody() && Base->GetBody()->GetCollisionShapeType() != Shape_Convex_Hull)
			Base->Clear();
	}
	else if (Shape == "none")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (Base->GetBody() && Base->GetBody()->GetCollisionShapeType() != Shape_Invalid)
			Base->Clear();
	}
	else if (Shape == "box")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape_Box)
			Base->Create(App->Scene->GetSimulator()->CreateCube(), 0, 0);
	}
	else if (Shape == "sphere")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape_Sphere)
			Base->Create(App->Scene->GetSimulator()->CreateSphere(), 0, 0);
	}
	else if (Shape == "capsule")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape_Capsule)
			Base->Create(App->Scene->GetSimulator()->CreateCapsule(), 0, 0);
	}
	else if (Shape == "cone")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape_Cone)
			Base->Create(App->Scene->GetSimulator()->CreateCone(), 0, 0);
	}
	else if (Shape == "cylinder")
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
		if (!Base->GetBody() || Base->GetBody()->GetCollisionShapeType() != Shape_Cylinder)
			Base->Create(App->Scene->GetSimulator()->CreateCylinder(), 0, 0);
	}

	RigidBody* Body = Base->GetBody();
	if (Body != nullptr)
	{
		App->Models.System->SetBoolean("sl_cmp_rigid_body_source", true);
		UI->GetElementById(0, "cmp_soft_body_kinemat").CastFormBoolean(&Base->Kinematic);

		Vector3 Offset = Body->GetLinearVelocity();
		if (UI->GetElementById(0, "cmp_rigid_body_v_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_v_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_v_z").CastFormFloat(&Offset.Z))
			Body->SetLinearVelocity(Offset);

		Offset = Body->GetAngularVelocity();
		if (UI->GetElementById(0, "cmp_rigid_body_t_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_t_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_t_z").CastFormFloat(&Offset.Z))
			Body->SetAngularVelocity(Offset);

		Offset = Body->GetGravity();
		if (UI->GetElementById(0, "cmp_rigid_body_g_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_g_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_g_z").CastFormFloat(&Offset.Z))
			Body->SetGravity(Offset);

		Offset = Body->GetAngularFactor();
		if (UI->GetElementById(0, "cmp_rigid_body_af_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_af_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_af_z").CastFormFloat(&Offset.Z))
			Body->SetAngularFactor(Offset);

		Offset = Body->GetLinearFactor();
		if (UI->GetElementById(0, "cmp_rigid_body_lf_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_lf_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_lf_z").CastFormFloat(&Offset.Z))
			Body->SetLinearFactor(Offset);

		Offset = Body->GetAnisotropicFriction();
		if (UI->GetElementById(0, "cmp_rigid_body_afric_x").CastFormFloat(&Offset.X) ||
			UI->GetElementById(0, "cmp_rigid_body_afric_y").CastFormFloat(&Offset.Y) ||
			UI->GetElementById(0, "cmp_rigid_body_afric_z").CastFormFloat(&Offset.Z))
			Body->SetAnisotropicFriction(Offset);

		float Value = Body->GetMass();
		if (UI->GetElementById(0, "cmp_rigid_body_mass").CastFormFloat(&Value))
			Body->SetMass(Value);

		Value = Body->GetAngularDamping();
		if (UI->GetElementById(0, "cmp_rigid_body_ad").CastFormFloat(&Value))
			Body->SetAngularDamping(Value);

		Value = Body->GetLinearDamping();
		if (UI->GetElementById(0, "cmp_rigid_body_ld").CastFormFloat(&Value))
			Body->SetLinearDamping(Value);

		Value = Body->GetContactDamping();
		if (UI->GetElementById(0, "cmp_rigid_body_cd").CastFormFloat(&Value))
			Body->SetContactDamping(Value);

		Value = Body->GetHitFraction();
		if (UI->GetElementById(0, "cmp_rigid_body_frac").CastFormFloat(&Value))
			Body->SetHitFraction(Value);

		Value = Body->GetRestitution();
		if (UI->GetElementById(0, "cmp_rigid_body_rest").CastFormFloat(&Value))
			Body->SetRestitution(Value);

		Value = Body->GetFriction();
		if (UI->GetElementById(0, "cmp_rigid_body_fric").CastFormFloat(&Value))
			Body->SetFriction(Value);

		Value = Body->GetSpinningFriction();
		if (UI->GetElementById(0, "cmp_rigid_body_spinf").CastFormFloat(&Value))
			Body->SetSpinningFriction(Value);

		Value = Body->GetRollingFriction();
		if (UI->GetElementById(0, "cmp_rigid_body_rollf").CastFormFloat(&Value))
			Body->SetRollingFriction(Value);

		Value = Body->GetContactStiffness();
		if (UI->GetElementById(0, "cmp_rigid_body_stiff").CastFormFloat(&Value, 1.0f / 1000000000000.0f))
			Body->SetContactStiffness(Value);

		Value = Body->GetAngularSleepingThreshold();
		if (UI->GetElementById(0, "cmp_rigid_body_ath").CastFormFloat(&Value))
			Body->SetAngularSleepingThreshold(Value);

		Value = Body->GetLinearSleepingThreshold();
		if (UI->GetElementById(0, "cmp_rigid_body_lth").CastFormFloat(&Value))
			Body->SetLinearSleepingThreshold(Value);

		Value = Body->GetCcdMotionThreshold();
		if (UI->GetElementById(0, "cmp_rigid_body_mth").CastFormFloat(&Value))
			Body->SetCcdMotionThreshold(Value);

		Value = Body->GetCcdSweptSphereRadius();
		if (UI->GetElementById(0, "cmp_rigid_body_srad").CastFormFloat(&Value))
			Body->SetCcdSweptSphereRadius(Value);

		Value = Body->GetDeactivationTime();
		if (UI->GetElementById(0, "cmp_rigid_body_dtime").CastFormFloat(&Value))
			Body->SetDeactivationTime(Value);

		bool Option = Body->IsActive();
		if (UI->GetElementById(0, "cmp_rigid_body_active").CastFormBoolean(&Option))
			Body->SetActivity(Option);

		Option = (Body->GetActivationState() != MotionState_Disable_Deactivation);
		if (UI->GetElementById(0, "cmp_rigid_body_deact").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetActivationState(MotionState_Active);
			else
				Body->SetActivationState(MotionState_Disable_Deactivation);
		}

		Option = Body->IsGhost();
		if (UI->GetElementById(0, "cmp_rigid_body_ghost").CastFormBoolean(&Option))
		{
			if (Option)
				Body->SetAsGhost();
			else
				Body->SetAsNormal();
		}
	}
	else
		App->Models.System->SetBoolean("sl_cmp_rigid_body_source", false);
}
void ComponentAcceleration(GUI::Context* UI, Components::Acceleration* Base)
{
	UI->GetElementById(0, "cmp_acceleration_av_x").CastFormFloat(&Base->AmplitudeVelocity.X);
	UI->GetElementById(0, "cmp_acceleration_av_y").CastFormFloat(&Base->AmplitudeVelocity.Y);
	UI->GetElementById(0, "cmp_acceleration_av_z").CastFormFloat(&Base->AmplitudeVelocity.Z);
	UI->GetElementById(0, "cmp_acceleration_v_x").CastFormFloat(&Base->ConstantVelocity.X);
	UI->GetElementById(0, "cmp_acceleration_v_y").CastFormFloat(&Base->ConstantVelocity.Y);
	UI->GetElementById(0, "cmp_acceleration_v_z").CastFormFloat(&Base->ConstantVelocity.Z);
	UI->GetElementById(0, "cmp_acceleration_at_x").CastFormFloat(&Base->AmplitudeTorque.X);
	UI->GetElementById(0, "cmp_acceleration_at_y").CastFormFloat(&Base->AmplitudeTorque.Y);
	UI->GetElementById(0, "cmp_acceleration_at_z").CastFormFloat(&Base->AmplitudeTorque.Z);
	UI->GetElementById(0, "cmp_acceleration_t_x").CastFormFloat(&Base->ConstantTorque.X);
	UI->GetElementById(0, "cmp_acceleration_t_y").CastFormFloat(&Base->ConstantTorque.Y);
	UI->GetElementById(0, "cmp_acceleration_t_z").CastFormFloat(&Base->ConstantTorque.Z);
	UI->GetElementById(0, "cmp_acceleration_c_x").CastFormFloat(&Base->ConstantCenter.X);
	UI->GetElementById(0, "cmp_acceleration_c_y").CastFormFloat(&Base->ConstantCenter.Y);
	UI->GetElementById(0, "cmp_acceleration_c_z").CastFormFloat(&Base->ConstantCenter.Z);
	UI->GetElementById(0, "cmp_acceleration_kinemat").CastFormBoolean(&Base->Kinematic);
}
void ComponentSliderConstraint(GUI::Context* UI, Components::SliderConstraint* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	static bool Ghost = true, Linear = true;
	ResolveSliderConstraint(UI, "cmp_slider_constraint_entity", Base, Ghost, Linear);

	SliderConstraint* Body = Base->GetConstraint();
	if (Body != nullptr)
	{
		App->Models.System->SetBoolean("sl_cmp_slider_constraint_entity", true);

		float Value = Body->GetAngularMotorVelocity();
		if (UI->GetElementById(0, "cmp_slider_constraint_amv").CastFormFloat(&Value))
			Body->SetAngularMotorVelocity(Value);

		Value = Body->GetLinearMotorVelocity();
		if (UI->GetElementById(0, "cmp_slider_constraint_lmv").CastFormFloat(&Value))
			Body->SetLinearMotorVelocity(Value);

		Value = Body->GetUpperLinearLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_ull").CastFormFloat(&Value))
			Body->SetUpperLinearLimit(Value);

		Value = Body->GetLowerLinearLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_lll").CastFormFloat(&Value))
			Body->SetLowerLinearLimit(Value);

		Value = Body->GetAngularDampingDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_add").CastFormFloat(&Value))
			Body->SetAngularDampingDirection(Value);

		Value = Body->GetLinearDampingDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_ldd").CastFormFloat(&Value))
			Body->SetLinearDampingDirection(Value);

		Value = Body->GetAngularDampingLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_adl").CastFormFloat(&Value))
			Body->SetAngularDampingLimit(Value);

		Value = Body->GetLinearDampingLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_ldl").CastFormFloat(&Value))
			Body->SetLinearDampingLimit(Value);

		Value = Body->GetAngularDampingOrtho();
		if (UI->GetElementById(0, "cmp_slider_constraint_ado").CastFormFloat(&Value))
			Body->SetAngularDampingOrtho(Value);

		Value = Body->GetLinearDampingOrtho();
		if (UI->GetElementById(0, "cmp_slider_constraint_ldo").CastFormFloat(&Value))
			Body->SetLinearDampingOrtho(Value);

		Value = Body->GetUpperAngularLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_ual").CastFormFloat(&Value))
			Body->SetUpperAngularLimit(Value);

		Value = Body->GetLowerAngularLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_lal").CastFormFloat(&Value))
			Body->SetLowerAngularLimit(Value);

		Value = Body->GetMaxAngularMotorForce();
		if (UI->GetElementById(0, "cmp_slider_constraint_mamf").CastFormFloat(&Value))
			Body->SetMaxAngularMotorForce(Value);

		Value = Body->GetMaxLinearMotorForce();
		if (UI->GetElementById(0, "cmp_slider_constraint_mlmf").CastFormFloat(&Value))
			Body->SetMaxLinearMotorForce(Value);

		Value = Body->GetAngularRestitutionDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_ard").CastFormFloat(&Value))
			Body->SetAngularRestitutionDirection(Value);

		Value = Body->GetLinearRestitutionDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_lrd").CastFormFloat(&Value))
			Body->SetLinearRestitutionDirection(Value);

		Value = Body->GetAngularSoftnessDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_asd").CastFormFloat(&Value))
			Body->SetAngularSoftnessDirection(Value);

		Value = Body->GetLinearSoftnessDirection();
		if (UI->GetElementById(0, "cmp_slider_constraint_lsd").CastFormFloat(&Value))
			Body->SetLinearSoftnessDirection(Value);

		Value = Body->GetAngularSoftnessLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_asl").CastFormFloat(&Value))
			Body->SetAngularSoftnessLimit(Value);

		Value = Body->GetLinearSoftnessLimit();
		if (UI->GetElementById(0, "cmp_slider_constraint_lsl").CastFormFloat(&Value))
			Body->SetLinearSoftnessLimit(Value);

		Value = Body->GetAngularSoftnessOrtho();
		if (UI->GetElementById(0, "cmp_slider_constraint_aso").CastFormFloat(&Value))
			Body->SetAngularSoftnessOrtho(Value);

		Value = Body->GetLinearSoftnessOrtho();
		if (UI->GetElementById(0, "cmp_slider_constraint_lso").CastFormFloat(&Value))
			Body->SetLinearSoftnessOrtho(Value);

		bool Option = Body->GetPoweredAngularMotor();
		if (UI->GetElementById(0, "cmp_slider_constraint_amp").CastFormBoolean(&Option))
			Body->SetPoweredAngularMotor(Option);

		Option = Body->GetPoweredLinearMotor();
		if (UI->GetElementById(0, "cmp_slider_constraint_lmp").CastFormBoolean(&Option))
			Body->SetPoweredLinearMotor(Option);

		Option = Body->IsEnabled();
		if (UI->GetElementById(0, "cmp_slider_constraint_active").CastFormBoolean(&Option))
			Body->SetEnabled(Option);
	}
	else
	{
		App->Models.System->SetBoolean("sl_cmp_slider_constraint_entity", false);
		UI->GetElementById(0, "cmp_slider_constraint_ghost").CastFormBoolean(&Ghost);
		UI->GetElementById(0, "cmp_slider_constraint_linear").CastFormBoolean(&Linear);
	}
}
void ComponentFreeLook(GUI::Context* UI, Components::FreeLook* Base)
{
	ResolveKeyCode(UI, "cmp_free_look_key_rot", &Base->Rotate);
	UI->GetElementById(0, "cmp_free_look_sens").CastFormFloat(&Base->Sensivity);
}
void ComponentFly(GUI::Context* UI, Components::Fly* Base)
{
	ResolveKeyCode(UI, "cmp_fly_key_forward", &Base->Forward);
	ResolveKeyCode(UI, "cmp_fly_key_backward", &Base->Backward);
	ResolveKeyCode(UI, "cmp_fly_key_right", &Base->Right);
	ResolveKeyCode(UI, "cmp_fly_key_left", &Base->Left);
	ResolveKeyCode(UI, "cmp_fly_key_up", &Base->Up);
	ResolveKeyCode(UI, "cmp_fly_key_down", &Base->Down);
	ResolveKeyCode(UI, "cmp_fly_key_slow", &Base->Slow);
	ResolveKeyCode(UI, "cmp_fly_key_fast", &Base->Fast);
	UI->GetElementById(0, "cmp_fly_ax").CastFormFloat(&Base->Axis.X);
	UI->GetElementById(0, "cmp_fly_ay").CastFormFloat(&Base->Axis.Y);
	UI->GetElementById(0, "cmp_fly_az").CastFormFloat(&Base->Axis.Z);
	UI->GetElementById(0, "cmp_fly_sp_down").CastFormFloat(&Base->SpeedDown);
	UI->GetElementById(0, "cmp_fly_sp_normal").CastFormFloat(&Base->SpeedNormal);
	UI->GetElementById(0, "cmp_fly_sp_up").CastFormFloat(&Base->SpeedUp);
}
void ComponentAudioSource(GUI::Context* UI, Components::AudioSource* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	ResolveAudioSource(UI, "cmp_audio_source_clip", Base);

	AudioClip* Source = Base->GetSource()->GetClip();
	if (Source != nullptr)
	{
		auto& Sync = Base->GetSync();
		App->Models.System->SetBoolean("sl_cmp_audio_source_clip", true);
		UI->GetElementById(0, "cmp_audio_source_gain").CastFormFloat(&Sync.Gain);
		UI->GetElementById(0, "cmp_audio_source_pitch").CastFormFloat(&Sync.Pitch);
		UI->GetElementById(0, "cmp_audio_source_dx").CastFormFloat(&Sync.Direction.X);
		UI->GetElementById(0, "cmp_audio_source_dy").CastFormFloat(&Sync.Direction.Y);
		UI->GetElementById(0, "cmp_audio_source_dz").CastFormFloat(&Sync.Direction.Z);
		UI->GetElementById(0, "cmp_audio_source_cia").CastFormFloat(&Sync.ConeInnerAngle);
		UI->GetElementById(0, "cmp_audio_source_coa").CastFormFloat(&Sync.ConeOuterAngle);
		UI->GetElementById(0, "cmp_audio_source_cog").CastFormFloat(&Sync.ConeOuterGain);
		UI->GetElementById(0, "cmp_audio_source_aa").CastFormFloat(&Sync.AirAbsorption);
		UI->GetElementById(0, "cmp_audio_source_rro").CastFormFloat(&Sync.RoomRollOff);
		UI->GetElementById(0, "cmp_audio_source_rr").CastFormFloat(&Sync.Rolloff);
		UI->GetElementById(0, "cmp_audio_source_dist").CastFormFloat(&Sync.Distance);
		UI->GetElementById(0, "cmp_audio_source_rdist").CastFormFloat(&Sync.RefDistance);
		UI->GetElementById(0, "cmp_audio_source_loop").CastFormBoolean(&Sync.IsLooped);
		UI->GetElementById(0, "cmp_audio_source_3d").CastFormBoolean(&Sync.IsRelative);

		if (UI->GetElementById(0, "cmp_audio_source_pos").CastFormFloat(&Sync.Position))
			Base->ApplyPlayingPosition();

		float Length = Source->Length();
		UI->GetElementById(0, "cmp_audio_source_length").CastFormFloat(&Length);
		App->Models.System->SetFloat("sl_cmp_audio_source_length", Length);

		unsigned int Buffer = Source->GetBuffer();
		UI->GetElementById(0, "cmp_audio_source_buffer").CastFormUInt32(&Buffer);

		int Format = Source->GetFormat();
		UI->GetElementById(0, "cmp_audio_source_format").CastFormInt32(&Format);

		bool Mono = Source->IsMono();
		UI->GetElementById(0, "cmp_audio_source_mono").CastFormBoolean(&Mono);

		if (UI->GetElementById(0, "cmp_audio_source_play").IsActive())
			Base->GetSource()->Play();

		if (UI->GetElementById(0, "cmp_audio_source_pause").IsActive())
			Base->GetSource()->Pause();

		if (UI->GetElementById(0, "cmp_audio_source_stop").IsActive())
			Base->GetSource()->Stop();
	}
	else
	{
		App->Models.System->SetBoolean("sl_cmp_audio_source_clip", false);
		App->Models.System->SetFloat("sl_cmp_audio_source_length", 0.0f);
	}
}
void ComponentAudioListener(GUI::Context* UI, Components::AudioListener* Base)
{
	UI->GetElementById(0, "cmp_audio_listener_gain").CastFormFloat(&Base->Gain);
}
void ComponentPointLight(GUI::Context* UI, Components::PointLight* Base)
{
	ResolveColor3(UI, "cmp_point_light_diffuse", &Base->Diffuse);
	UI->GetElementById(0, "cmp_point_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById(0, "cmp_point_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById(0, "cmp_point_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById(0, "cmp_point_light_srange").CastFormFloat(&Base->Size.Range);
	UI->GetElementById(0, "cmp_point_light_sc1").CastFormFloat(&Base->Size.C1);
	UI->GetElementById(0, "cmp_point_light_sc2").CastFormFloat(&Base->Size.C2);
	UI->GetElementById(0, "cmp_point_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById(0, "cmp_point_light_sd_dist").CastFormFloat(&Base->Shadow.Distance);
	UI->GetElementById(0, "cmp_point_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById(0, "cmp_point_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);
}
void ComponentSpotLight(GUI::Context* UI, Components::SpotLight* Base)
{
	ResolveColor3(UI, "cmp_spot_light_diffuse", &Base->Diffuse);
	UI->GetElementById(0, "cmp_spot_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById(0, "cmp_spot_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById(0, "cmp_spot_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById(0, "cmp_spot_light_cutoff").CastFormFloat(&Base->Cutoff);
	UI->GetElementById(0, "cmp_spot_light_srange").CastFormFloat(&Base->Size.Range);
	UI->GetElementById(0, "cmp_spot_light_sc1").CastFormFloat(&Base->Size.C1);
	UI->GetElementById(0, "cmp_spot_light_sc2").CastFormFloat(&Base->Size.C2);
	UI->GetElementById(0, "cmp_spot_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById(0, "cmp_spot_light_sd_dist").CastFormFloat(&Base->Shadow.Distance);
	UI->GetElementById(0, "cmp_spot_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById(0, "cmp_spot_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);
}
void ComponentLineLight(GUI::Context* UI, Components::LineLight* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	App->Models.System->SetInteger("sl_cmp_line_light_cascades", Base->Shadow.Cascades);
	if (UI->GetElementById(0, "cmp_line_light_sd_casc").CastFormUInt32(&Base->Shadow.Cascades))
		Base->Shadow.Cascades = Math<uint32_t>::Clamp(Base->Shadow.Cascades, 0, 6);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist0").CastFormFloat(&Base->Shadow.Distance[0]))
		Base->Shadow.Distance[0] = Mathf::Max(0, Base->Shadow.Distance[0]);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist1").CastFormFloat(&Base->Shadow.Distance[1]))
		Base->Shadow.Distance[1] = Mathf::Max(Base->Shadow.Distance[0] + 1.0f, Base->Shadow.Distance[1]);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist2").CastFormFloat(&Base->Shadow.Distance[2]))
		Base->Shadow.Distance[2] = Mathf::Max(Base->Shadow.Distance[1] + 1.0f, Base->Shadow.Distance[2]);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist3").CastFormFloat(&Base->Shadow.Distance[3]))
		Base->Shadow.Distance[3] = Mathf::Max(Base->Shadow.Distance[2] + 1.0f, Base->Shadow.Distance[3]);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist4").CastFormFloat(&Base->Shadow.Distance[4]))
		Base->Shadow.Distance[4] = Mathf::Max(Base->Shadow.Distance[3] + 1.0f, Base->Shadow.Distance[4]);

	if (UI->GetElementById(0, "cmp_line_light_sd_dist5").CastFormFloat(&Base->Shadow.Distance[5]))
		Base->Shadow.Distance[5] = Mathf::Max(Base->Shadow.Distance[4] + 1.0f, Base->Shadow.Distance[5]);

	ResolveColor3(UI, "cmp_line_light_diffuse", &Base->Diffuse);
	ResolveColor3(UI, "cmp_line_light_rlh", &Base->Sky.RlhEmission);
	ResolveColor3(UI, "cmp_line_light_mie", &Base->Sky.MieEmission);
	UI->GetElementById(0, "cmp_line_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById(0, "cmp_line_light_disp").CastFormFloat(&Base->Disperse);
	UI->GetElementById(0, "cmp_line_light_rlhh").CastFormFloat(&Base->Sky.RlhHeight);
	UI->GetElementById(0, "cmp_line_light_mieh").CastFormFloat(&Base->Sky.MieHeight);
	UI->GetElementById(0, "cmp_line_light_mied").CastFormFloat(&Base->Sky.MieDirection);
	UI->GetElementById(0, "cmp_line_light_scatter").CastFormFloat(&Base->Sky.Intensity);
	UI->GetElementById(0, "cmp_line_light_inner").CastFormFloat(&Base->Sky.InnerRadius);
	UI->GetElementById(0, "cmp_line_light_outer").CastFormFloat(&Base->Sky.OuterRadius);
	UI->GetElementById(0, "cmp_line_light_pcf").CastFormUInt32(&Base->Shadow.Iterations);
	UI->GetElementById(0, "cmp_line_light_sd_bias").CastFormFloat(&Base->Shadow.Bias);
	UI->GetElementById(0, "cmp_line_light_sd_soft").CastFormFloat(&Base->Shadow.Softness);
	UI->GetElementById(0, "cmp_line_light_sd_off").CastFormFloat(&Base->Shadow.Offset);
	UI->GetElementById(0, "cmp_line_light_sd_active").CastFormBoolean(&Base->Shadow.Enabled);
}
void ComponentSurfaceLight(GUI::Context* UI, Components::SurfaceLight* Base)
{
	ResolveColor3(UI, "cmp_surface_light_refl", &Base->Diffuse);
	ResolveTexture2D(UI, "cmp_surface_light_source", Base->GetDiffuseMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetDiffuseMap(New);
	});

	UI->GetElementById(0, "cmp_surface_light_vo_x").CastFormFloat(&Base->Offset.X);
	UI->GetElementById(0, "cmp_surface_light_vo_y").CastFormFloat(&Base->Offset.Y);
	UI->GetElementById(0, "cmp_surface_light_vo_z").CastFormFloat(&Base->Offset.Z);
	UI->GetElementById(0, "cmp_surface_light_rd").CastFormDouble(&Base->Tick.Delay);
	UI->GetElementById(0, "cmp_surface_light_srange").CastFormFloat(&Base->Size.Range);
	UI->GetElementById(0, "cmp_surface_light_sc1").CastFormFloat(&Base->Size.C1);
	UI->GetElementById(0, "cmp_surface_light_sc2").CastFormFloat(&Base->Size.C2);
	UI->GetElementById(0, "cmp_surface_light_emission").CastFormFloat(&Base->Emission);
	UI->GetElementById(0, "cmp_surface_light_inf").CastFormFloat(&Base->Infinity);
	UI->GetElementById(0, "cmp_surface_light_px").CastFormBoolean(&Base->Parallax);
	UI->GetElementById(0, "cmp_surface_light_static").CastFormBoolean(&Base->StaticMask);
}
void ComponentIlluminator(GUI::Context* UI, Components::Illuminator* Base)
{
	UI->GetElementById(0, "cmp_illuminator_rd").CastFormDouble(&Base->Tick.Delay);
	UI->GetElementById(0, "cmp_illuminator_rs").CastFormFloat(&Base->RayStep);
	UI->GetElementById(0, "cmp_illuminator_ms").CastFormFloat(&Base->MaxSteps);
	UI->GetElementById(0, "cmp_illuminator_d").CastFormFloat(&Base->Distance);
	UI->GetElementById(0, "cmp_illuminator_i").CastFormFloat(&Base->Intensity);
	UI->GetElementById(0, "cmp_illuminator_o").CastFormFloat(&Base->Occlusion);
	UI->GetElementById(0, "cmp_illuminator_s").CastFormFloat(&Base->Shadows);
	UI->GetElementById(0, "cmp_illuminator_b").CastFormFloat(&Base->Bleeding);

	uint32 Size = (uint32)Base->GetBufferSize();
	if (UI->GetElementById(0, "cmp_illuminator_sz").CastFormUInt32(&Size) && Size != Base->GetBufferSize())
		Base->SetBufferSize(Size);
}
void ComponentCamera(GUI::Context* UI, Components::Camera* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	uint64_t Size = Base->GetRenderer()->GetDepthSize();
	uint64_t Stalls = Base->GetRenderer()->StallFrames;
	bool Preview = !App->State.IsCameraActive;
	bool FC = Base->GetRenderer()->HasFrustumCulling();
	bool OC = Base->GetRenderer()->HasOcclusionCulling();

	if (UI->GetElementById(0, "cmp_camera_fc").CastFormBoolean(&FC))
		Base->GetRenderer()->SetFrustumCulling(FC);

	if (UI->GetElementById(0, "cmp_camera_oc").CastFormBoolean(&OC))
		Base->GetRenderer()->SetOcclusionCulling(FC);

	UI->GetElementById(0, "cmp_camera_fov").CastFormFloat(&Base->FieldOfView);
	UI->GetElementById(0, "cmp_camera_w").CastFormFloat(&Base->Width);
	UI->GetElementById(0, "cmp_camera_h").CastFormFloat(&Base->Height);
	UI->GetElementById(0, "cmp_camera_np").CastFormFloat(&Base->NearPlane);
	UI->GetElementById(0, "cmp_camera_fp").CastFormFloat(&Base->FarPlane);
	UI->GetElementById(0, "cmp_camera_oc_rd").CastFormDouble(&Base->GetRenderer()->Occlusion.Delay);
	UI->GetElementById(0, "cmp_camera_oc_sd").CastFormDouble(&Base->GetRenderer()->Sorting.Delay);
	UI->GetElementById(0, "cmp_camera_oc_sf").CastFormUInt64(&Stalls);
	UI->GetElementById(0, "cmp_camera_oc_db").CastFormUInt64(&Size);

	if (UI->GetElementById(0, "cmp_camera_perspective").IsActive())
		Base->Mode = Components::Camera::ProjectionMode_Perspective;
	else if (UI->GetElementById(0, "cmp_camera_ortho").IsActive())
		Base->Mode = Components::Camera::ProjectionMode_Orthographic;

	Base->GetRenderer()->StallFrames = Stalls;
	if (Size != Base->GetRenderer()->GetDepthSize())
		Base->GetRenderer()->SetDepthSize(Size);

	if (UI->GetElementById(0, "cmp_camera_preview").CastFormBoolean(&Preview))
	{
		if (!Base->IsActive() || !Preview)
		{
			auto* Main = App->State.Camera->GetComponent<Components::Camera>();
			App->Scene->SetCamera(App->State.Camera);

			if (App->Scene->IsActive())
			{
				Main->GetRenderer()->SetOcclusionCulling(false, true);
				Main->GetRenderer()->SetFrustumCulling(true, true);
			}
			else
				Main->GetRenderer()->ClearCull();
		}
		else
			App->Scene->SetCamera(Base->GetEntity());

		App->State.IsCameraActive = !Preview;
		if (!Base->IsActive())
			App->State.IsCameraActive = true;
	}
	else if (Preview && !Base->IsActive())
	{
		App->Scene->SetCamera(App->State.Camera);
		App->State.IsCameraActive = true;
	}
}
void ComponentScriptable(GUI::Context* UI, Components::Scriptable* Base)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	App->Models.System->SetBoolean("sl_cmp_scriptable_source", !Base->GetSource().empty());
	ResolveScriptable(UI, "cmp_scriptable_source", Base);

	bool Typeless = (Base->GetInvokeType() == Components::Scriptable::InvokeType_Typeless);
	if (UI->GetElementById(0, "cmp_scriptable_ti").CastFormBoolean(&Typeless))
		Base->SetInvocation(Typeless ? Components::Scriptable::InvokeType_Typeless : Components::Scriptable::InvokeType_Normal);

	if (UI->GetElementById(0, "cmp_scriptable_recompile").IsActive())
	{
		App->VM->ClearCache();
		Base->SetSource();
	}
}