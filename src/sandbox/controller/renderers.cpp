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
	UI->GetElementById(0, "cmp_camera_lighting_gaoi").CastFormFloat(&Base->VoxelBuffer.Shadows);
	UI->GetElementById(0, "cmp_camera_lighting_gaos").CastFormFloat(&Base->VoxelBuffer.Occlusion);
	UI->GetElementById(0, "cmp_camera_lighting_grs").CastFormFloat(&Base->VoxelBuffer.RayStep);
	UI->GetElementById(0, "cmp_camera_lighting_gi").CastFormFloat(&Base->VoxelBuffer.Intensity);
	UI->GetElementById(0, "cmp_camera_lighting_gdx").CastFormFloat(&Base->Radiance.Distance.X);
	UI->GetElementById(0, "cmp_camera_lighting_gdy").CastFormFloat(&Base->Radiance.Distance.Y);
	UI->GetElementById(0, "cmp_camera_lighting_gdz").CastFormFloat(&Base->Radiance.Distance.Z);
	UI->GetElementById(0, "cmp_camera_lighting_gtick").CastFormDouble(&Base->Radiance.Tick.Delay);
	UI->GetElementById(0, "cmp_camera_lighting_gactive").CastFormBoolean(&Base->Radiance.Enabled);

	unsigned int MaxSteps = Base->VoxelBuffer.MaxSteps;
	if (UI->GetElementById(0, "cmp_camera_lighting_gms").CastFormUInt32(&MaxSteps))
		Base->VoxelBuffer.MaxSteps = MaxSteps;

	bool Recursive = (Base->AmbientLight.Recursive > 0.0f);
	if (UI->GetElementById(0, "cmp_camera_lighting_rp").CastFormBoolean(&Recursive))
		Base->AmbientLight.Recursive = (Recursive ? 1.0f : 0.0f);

	uint64_t Size = Base->Radiance.Size;
	if (UI->GetElementById(0, "cmp_camera_lighting_gsize").CastFormUInt64(&Size) && Size > 0)
		Base->SetVoxelBufferSize(Size);

	Size = Base->Surfaces.Size;
	if (UI->GetElementById(0, "cmp_camera_lighting_r").CastFormUInt64(&Size) && Size > 0)
		Base->SetSurfaceBufferSize(Size);
}
void RendererSSR(GUI::Context* UI, Renderers::SSR* Base)
{
	UI->GetElementById(0, "cmp_camera_ssr_sc").CastFormFloat(&Base->Reflectance.Samples);
	UI->GetElementById(0, "cmp_camera_ssr_i").CastFormFloat(&Base->Reflectance.Intensity);
	UI->GetElementById(0, "cmp_camera_ssr_d").CastFormFloat(&Base->Reflectance.Distance);
	UI->GetElementById(0, "cmp_camera_ssr_bc").CastFormFloat(&Base->Gloss.Samples);
	UI->GetElementById(0, "cmp_camera_ssr_b").CastFormFloat(&Base->Gloss.Blur);
}
void RendererSSAO(GUI::Context* UI, Renderers::SSAO* Base)
{
	UI->GetElementById(0, "cmp_camera_ssao_sc").CastFormFloat(&Base->Shading.Samples);
	UI->GetElementById(0, "cmp_camera_ssao_d").CastFormFloat(&Base->Shading.Distance);
	UI->GetElementById(0, "cmp_camera_ssao_fd").CastFormFloat(&Base->Shading.Fade);
	UI->GetElementById(0, "cmp_camera_ssao_bs").CastFormFloat(&Base->Shading.Bias);
	UI->GetElementById(0, "cmp_camera_ssao_i").CastFormFloat(&Base->Shading.Intensity);
	UI->GetElementById(0, "cmp_camera_ssao_s").CastFormFloat(&Base->Shading.Scale);
	UI->GetElementById(0, "cmp_camera_ssao_r").CastFormFloat(&Base->Shading.Radius);
	UI->GetElementById(0, "cmp_camera_ssao_bc").CastFormFloat(&Base->Fibo.Samples);
	UI->GetElementById(0, "cmp_camera_ssao_blr").CastFormFloat(&Base->Fibo.Blur);
	UI->GetElementById(0, "cmp_camera_ssao_blp").CastFormFloat(&Base->Fibo.Power);
}
void RendererMotionBlur(GUI::Context* UI, Renderers::MotionBlur* Base)
{
	UI->GetElementById(0, "cmp_camera_motionblur_sc").CastFormFloat(&Base->Motion.Samples);
	UI->GetElementById(0, "cmp_camera_motionblur_bl").CastFormFloat(&Base->Motion.Blur);
	UI->GetElementById(0, "cmp_camera_motionblur_mn").CastFormFloat(&Base->Motion.Motion);
}
void RendererBloom(GUI::Context* UI, Renderers::Bloom* Base)
{;
	UI->GetElementById(0, "cmp_camera_bloom_i").CastFormFloat(&Base->Extraction.Intensity);
	UI->GetElementById(0, "cmp_camera_bloom_t").CastFormFloat(&Base->Extraction.Threshold);
	UI->GetElementById(0, "cmp_camera_bloom_bc").CastFormFloat(&Base->Fibo.Samples);
	UI->GetElementById(0, "cmp_camera_bloom_blr").CastFormFloat(&Base->Fibo.Blur);
	UI->GetElementById(0, "cmp_camera_bloom_blp").CastFormFloat(&Base->Fibo.Power);
}
void RendererDoF(GUI::Context* UI, Renderers::DoF* Base)
{
	UI->GetElementById(0, "cmp_camera_dof_afd").CastFormFloat(&Base->Distance);
	UI->GetElementById(0, "cmp_camera_dof_aft").CastFormFloat(&Base->Time);
	UI->GetElementById(0, "cmp_camera_dof_afr").CastFormFloat(&Base->Radius);
	UI->GetElementById(0, "cmp_camera_dof_r").CastFormFloat(&Base->Focus.Radius);
	UI->GetElementById(0, "cmp_camera_dof_b").CastFormFloat(&Base->Focus.Bokeh);
	UI->GetElementById(0, "cmp_camera_dof_s").CastFormFloat(&Base->Focus.Scale);
	UI->GetElementById(0, "cmp_camera_dof_nd").CastFormFloat(&Base->Focus.NearDistance);
	UI->GetElementById(0, "cmp_camera_dof_nr").CastFormFloat(&Base->Focus.NearRange);
	UI->GetElementById(0, "cmp_camera_dof_fd").CastFormFloat(&Base->Focus.FarDistance);
	UI->GetElementById(0, "cmp_camera_dof_fr").CastFormFloat(&Base->Focus.FarRange);
}
void RendererTone(GUI::Context* UI, Renderers::Tone* Base)
{
	UI->GetElementById(0, "cmp_camera_tone_gs").CastFormFloat(&Base->Mapping.Grayscale);
	UI->GetElementById(0, "cmp_camera_tone_aces").CastFormFloat(&Base->Mapping.ACES);
	UI->GetElementById(0, "cmp_camera_tone_fm").CastFormFloat(&Base->Mapping.Filmic);
	UI->GetElementById(0, "cmp_camera_tone_ls").CastFormFloat(&Base->Mapping.Lottes);
	UI->GetElementById(0, "cmp_camera_tone_rh").CastFormFloat(&Base->Mapping.Reinhard);
	UI->GetElementById(0, "cmp_camera_tone_rh2").CastFormFloat(&Base->Mapping.Reinhard2);
	UI->GetElementById(0, "cmp_camera_tone_ul").CastFormFloat(&Base->Mapping.Unreal);
	UI->GetElementById(0, "cmp_camera_tone_uc").CastFormFloat(&Base->Mapping.Uchimura);
	UI->GetElementById(0, "cmp_camera_tone_ubr").CastFormFloat(&Base->Mapping.UBrightness);
	UI->GetElementById(0, "cmp_camera_tone_uct").CastFormFloat(&Base->Mapping.UContrast);
	UI->GetElementById(0, "cmp_camera_tone_ust").CastFormFloat(&Base->Mapping.UStart);
	UI->GetElementById(0, "cmp_camera_tone_uln").CastFormFloat(&Base->Mapping.ULength);
	UI->GetElementById(0, "cmp_camera_tone_ubl").CastFormFloat(&Base->Mapping.UBlack);
	UI->GetElementById(0, "cmp_camera_tone_upl").CastFormFloat(&Base->Mapping.UPedestal);
	UI->GetElementById(0, "cmp_camera_tone_exp").CastFormFloat(&Base->Mapping.Exposure);
	UI->GetElementById(0, "cmp_camera_tone_eint").CastFormFloat(&Base->Mapping.EIntensity);
	UI->GetElementById(0, "cmp_camera_tone_egm").CastFormFloat(&Base->Mapping.EGamma);
	UI->GetElementById(0, "cmp_camera_tone_aint").CastFormFloat(&Base->Mapping.Adaptation);
	UI->GetElementById(0, "cmp_camera_tone_agr").CastFormFloat(&Base->Mapping.AGray);
	UI->GetElementById(0, "cmp_camera_tone_awh").CastFormFloat(&Base->Mapping.AWhite);
	UI->GetElementById(0, "cmp_camera_tone_abl").CastFormFloat(&Base->Mapping.ABlack);
	UI->GetElementById(0, "cmp_camera_tone_asp").CastFormFloat(&Base->Mapping.ASpeed);
}
void RendererGlitch(GUI::Context* UI, Renderers::Glitch* Base)
{
	UI->GetElementById(0, "cmp_camera_glitch_cd").CastFormFloat(&Base->ColorDrift);
	UI->GetElementById(0, "cmp_camera_glitch_hs").CastFormFloat(&Base->HorizontalShake);
	UI->GetElementById(0, "cmp_camera_glitch_slj").CastFormFloat(&Base->ScanLineJitter);
	UI->GetElementById(0, "cmp_camera_glitch_vj").CastFormFloat(&Base->VerticalJump);
}