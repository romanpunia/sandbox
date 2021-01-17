#include "renderers.h"
#include "resolvers.h"

void RendererLighting(GUI::Context* UI, Renderers::Lighting* Base)
{
	ResolveTexture2D(UI, "cmp_camera_lighting_sm", Base->GetSkyMap() != nullptr, [Base](Texture2D* New) { Base->SetSkyMap(New); });
	ResolveColor3(UI, "cmp_camera_lighting_hc", &Base->AmbientLight.HighEmission);
	ResolveColor3(UI, "cmp_camera_lighting_lc", &Base->AmbientLight.LowEmission);
	ResolveColor3(UI, "cmp_camera_lighting_sc", &Base->AmbientLight.SkyColor);
	ResolveColor3(UI, "cmp_camera_lighting_fc", &Base->AmbientLight.FogColor);
	UI->GetElementById(0, "cmp_camera_lighting_plr").CastFormUInt64(&Base->Shadows.PointLightResolution);
	UI->GetElementById(0, "cmp_camera_lighting_pll").CastFormUInt64(&Base->Shadows.PointLightLimits);
	UI->GetElementById(0, "cmp_camera_lighting_slr").CastFormUInt64(&Base->Shadows.SpotLightResolution);
	UI->GetElementById(0, "cmp_camera_lighting_sll").CastFormUInt64(&Base->Shadows.SpotLightLimits);
	UI->GetElementById(0, "cmp_camera_lighting_llr").CastFormUInt64(&Base->Shadows.LineLightResolution);
	UI->GetElementById(0, "cmp_camera_lighting_lll").CastFormUInt64(&Base->Shadows.LineLightLimits);
	UI->GetElementById(0, "cmp_camera_lighting_rd").CastFormDouble(&Base->Shadows.Tick.Delay);
	UI->GetElementById(0, "cmp_camera_lighting_sd").CastFormFloat(&Base->Shadows.Distance);
	UI->GetElementById(0, "cmp_camera_lighting_ff_x").CastFormFloat(&Base->AmbientLight.FogFar.X);
	UI->GetElementById(0, "cmp_camera_lighting_ff_y").CastFormFloat(&Base->AmbientLight.FogFar.Y);
	UI->GetElementById(0, "cmp_camera_lighting_ff_z").CastFormFloat(&Base->AmbientLight.FogFar.Z);
	UI->GetElementById(0, "cmp_camera_lighting_fn_x").CastFormFloat(&Base->AmbientLight.FogNear.X);
	UI->GetElementById(0, "cmp_camera_lighting_fn_y").CastFormFloat(&Base->AmbientLight.FogNear.Y);
	UI->GetElementById(0, "cmp_camera_lighting_fn_z").CastFormFloat(&Base->AmbientLight.FogNear.Z);
	UI->GetElementById(0, "cmp_camera_lighting_fno").CastFormFloat(&Base->AmbientLight.FogNearOff);
	UI->GetElementById(0, "cmp_camera_lighting_ffo").CastFormFloat(&Base->AmbientLight.FogFarOff);
	UI->GetElementById(0, "cmp_camera_lighting_fa").CastFormFloat(&Base->AmbientLight.FogAmount);
	UI->GetElementById(0, "cmp_camera_lighting_se").CastFormFloat(&Base->AmbientLight.SkyEmission);
	UI->GetElementById(0, "cmp_camera_lighting_le").CastFormFloat(&Base->AmbientLight.LightEmission);

	bool Recursive = (Base->AmbientLight.Recursive > 0.0f);
	if (UI->GetElementById(0, "cmp_camera_lighting_rp").CastFormBoolean(&Recursive))
		Base->AmbientLight.Recursive = (Recursive ? 1.0f : 0.0f);

	if (UI->GetElementById(0, "cmp_camera_lighting_r").CastFormUInt64(&Base->Surfaces.Size))
		Base->SetSurfaceBufferSize(Base->Surfaces.Size);
}
void RendererSSR(GUI::Context* UI, Renderers::SSR* Base)
{
	UI->GetElementById(0, "cmp_camera_ssr_sc").CastFormFloat(&Base->RenderPass1.Samples);
	UI->GetElementById(0, "cmp_camera_ssr_bc").CastFormFloat(&Base->RenderPass2.Samples);
	UI->GetElementById(0, "cmp_camera_ssr_i").CastFormFloat(&Base->RenderPass1.Intensity);
	UI->GetElementById(0, "cmp_camera_ssr_b").CastFormFloat(&Base->RenderPass2.Blur);
}
void RendererSSAO(GUI::Context* UI, Renderers::SSAO* Base)
{
	UI->GetElementById(0, "cmp_camera_ssao_sc").CastFormFloat(&Base->RenderPass1.Samples);
	UI->GetElementById(0, "cmp_camera_ssao_bc").CastFormFloat(&Base->RenderPass2.Samples);
	UI->GetElementById(0, "cmp_camera_ssao_d").CastFormFloat(&Base->RenderPass1.Distance);
	UI->GetElementById(0, "cmp_camera_ssao_fd").CastFormFloat(&Base->RenderPass1.Fade);
	UI->GetElementById(0, "cmp_camera_ssao_bs").CastFormFloat(&Base->RenderPass1.Bias);
	UI->GetElementById(0, "cmp_camera_ssao_i").CastFormFloat(&Base->RenderPass1.Intensity);
	UI->GetElementById(0, "cmp_camera_ssao_s").CastFormFloat(&Base->RenderPass1.Scale);
	UI->GetElementById(0, "cmp_camera_ssao_r").CastFormFloat(&Base->RenderPass1.Radius);
	UI->GetElementById(0, "cmp_camera_ssao_blr").CastFormFloat(&Base->RenderPass2.Blur);
	UI->GetElementById(0, "cmp_camera_ssao_blp").CastFormFloat(&Base->RenderPass2.Power);
}
void RendererSSDO(GUI::Context* UI, Renderers::SSDO* Base)
{
	UI->GetElementById(0, "cmp_camera_ssdo_sc").CastFormFloat(&Base->RenderPass1.Samples);
	UI->GetElementById(0, "cmp_camera_ssdo_bc").CastFormFloat(&Base->RenderPass2.Samples);
	UI->GetElementById(0, "cmp_camera_ssdo_d").CastFormFloat(&Base->RenderPass1.Distance);
	UI->GetElementById(0, "cmp_camera_ssdo_fd").CastFormFloat(&Base->RenderPass1.Fade);
	UI->GetElementById(0, "cmp_camera_ssdo_bs").CastFormFloat(&Base->RenderPass1.Bias);
	UI->GetElementById(0, "cmp_camera_ssdo_i").CastFormFloat(&Base->RenderPass1.Intensity);
	UI->GetElementById(0, "cmp_camera_ssdo_s").CastFormFloat(&Base->RenderPass1.Scale);
	UI->GetElementById(0, "cmp_camera_ssdo_r").CastFormFloat(&Base->RenderPass1.Radius);
	UI->GetElementById(0, "cmp_camera_ssdo_blr").CastFormFloat(&Base->RenderPass2.Blur);
	UI->GetElementById(0, "cmp_camera_ssdo_blp").CastFormFloat(&Base->RenderPass2.Power);
}
void RendererBloom(GUI::Context* UI, Renderers::Bloom* Base)
{
	UI->GetElementById(0, "cmp_camera_bloom_sc").CastFormFloat(&Base->RenderPass.Samples);
	UI->GetElementById(0, "cmp_camera_bloom_s").CastFormFloat(&Base->RenderPass.Scale);
	UI->GetElementById(0, "cmp_camera_bloom_i").CastFormFloat(&Base->RenderPass.Intensity);
	UI->GetElementById(0, "cmp_camera_bloom_t").CastFormFloat(&Base->RenderPass.Threshold);
}
void RendererDoF(GUI::Context* UI, Renderers::DoF* Base)
{
	UI->GetElementById(0, "cmp_camera_dof_afd").CastFormFloat(&Base->FocusDistance);
	UI->GetElementById(0, "cmp_camera_dof_aft").CastFormFloat(&Base->FocusTime);
	UI->GetElementById(0, "cmp_camera_dof_afr").CastFormFloat(&Base->FocusRadius);
	UI->GetElementById(0, "cmp_camera_dof_r").CastFormFloat(&Base->RenderPass.Radius);
	UI->GetElementById(0, "cmp_camera_dof_b").CastFormFloat(&Base->RenderPass.Bokeh);
	UI->GetElementById(0, "cmp_camera_dof_s").CastFormFloat(&Base->RenderPass.Scale);
	UI->GetElementById(0, "cmp_camera_dof_nd").CastFormFloat(&Base->RenderPass.NearDistance);
	UI->GetElementById(0, "cmp_camera_dof_nr").CastFormFloat(&Base->RenderPass.NearRange);
	UI->GetElementById(0, "cmp_camera_dof_fd").CastFormFloat(&Base->RenderPass.FarDistance);
	UI->GetElementById(0, "cmp_camera_dof_fr").CastFormFloat(&Base->RenderPass.FarRange);
}
void RendererTone(GUI::Context* UI, Renderers::Tone* Base)
{
	ResolveColor3(UI, "cmp_camera_tone_dg", &Base->RenderPass.DesaturationGamma);
	ResolveColor3(UI, "cmp_camera_tone_cg", &Base->RenderPass.ColorGamma);
	ResolveColor3(UI, "cmp_camera_tone_vc", &Base->RenderPass.VignetteColor);
	ResolveColor3(UI, "cmp_camera_tone_rt", &Base->RenderPass.BlindVisionR);
	ResolveColor3(UI, "cmp_camera_tone_gt", &Base->RenderPass.BlindVisionG);
	ResolveColor3(UI, "cmp_camera_tone_bt", &Base->RenderPass.BlindVisionB);
	UI->GetElementById(0, "cmp_camera_tone_va").CastFormFloat(&Base->RenderPass.VignetteAmount);
	UI->GetElementById(0, "cmp_camera_tone_vcv").CastFormFloat(&Base->RenderPass.VignetteCurve);
	UI->GetElementById(0, "cmp_camera_tone_vr").CastFormFloat(&Base->RenderPass.VignetteRadius);
	UI->GetElementById(0, "cmp_camera_tone_gc").CastFormFloat(&Base->RenderPass.GammaIntensity);
	UI->GetElementById(0, "cmp_camera_tone_gi").CastFormFloat(&Base->RenderPass.LinearIntensity);
	UI->GetElementById(0, "cmp_camera_tone_ti").CastFormFloat(&Base->RenderPass.ToneIntensity);
	UI->GetElementById(0, "cmp_camera_tone_di").CastFormFloat(&Base->RenderPass.DesaturationIntensity);
	UI->GetElementById(0, "cmp_camera_tone_af").CastFormFloat(&Base->RenderPass.AcesIntensity);
	UI->GetElementById(0, "cmp_camera_tone_afa").CastFormFloat(&Base->RenderPass.AcesA);
	UI->GetElementById(0, "cmp_camera_tone_afb").CastFormFloat(&Base->RenderPass.AcesB);
	UI->GetElementById(0, "cmp_camera_tone_afc").CastFormFloat(&Base->RenderPass.AcesC);
	UI->GetElementById(0, "cmp_camera_tone_afd").CastFormFloat(&Base->RenderPass.AcesD);
	UI->GetElementById(0, "cmp_camera_tone_afe").CastFormFloat(&Base->RenderPass.AcesE);
}
void RendererGlitch(GUI::Context* UI, Renderers::Glitch* Base)
{
	UI->GetElementById(0, "cmp_camera_glitch_cd").CastFormFloat(&Base->ColorDrift);
	UI->GetElementById(0, "cmp_camera_glitch_hs").CastFormFloat(&Base->HorizontalShake);
	UI->GetElementById(0, "cmp_camera_glitch_slj").CastFormFloat(&Base->ScanLineJitter);
	UI->GetElementById(0, "cmp_camera_glitch_vj").CastFormFloat(&Base->VerticalJump);
}