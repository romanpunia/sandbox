#include "resolvers.h"
#include "../core/sandbox.h"

void ResolveResource(GUI::IElement& Target, const std::string& Name, const std::function<void(const std::string&)>& Callback)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App || !Callback)
		return;

	if (!App->GetResourceState(Name))
	{
		Target.SetInnerHTML("Awaiting source...");
		App->GetResource(Name, [App, Target, Callback](const std::string& File)
		{
			GUI::IElement Copy = Target;
			Copy.SetInnerHTML(File.empty() ? "Assign source" : "Unassign source");

			Callback(File);
		});
	}
	else
	{
		Target.SetInnerHTML("");
		App->GetResource("", nullptr);
	}
}
void ResolveEntity(GUI::IElement& Target, const std::string& Name, const std::function<void(Entity*)>& Callback)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App || !Callback)
		return;

	if (!App->GetEntityState(Name))
	{
		Target.SetInnerHTML("Awaiting entity...");
		App->GetEntity(Name, [App, Target, Callback](Entity* Source)
		{
			GUI::IElement Copy = Target;
			Copy.SetInnerHTML(!Source ? "Assign entity" : "Unassign entity");

			Callback(Source);
		});
	}
	else
	{
		Target.SetInnerHTML("");
		App->GetEntity("", nullptr);
	}
}
void ResolveTexture2D(GUI::Context* UI, const std::string& Id, bool Assigned, const std::function<void(Texture2D*)>& Callback)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Assigned)
		{
			ResolveResource(Source, "texture", [Callback](const std::string& File)
			{
				Callback(Sandbox::Get()->Content->Load<Tomahawk::Graphics::Texture2D>(File));
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Callback(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Assigned ? "Unassign source" : "Assign source");
}
void ResolveKeyCode(GUI::Context* UI, const std::string& Id, KeyMap* Output)
{
	Sandbox* App = Sandbox::Get()->As<Sandbox>();
	if (!App)
		return;

	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Output->Normal)
	{
		if (Source.GetInnerHTML().empty())
			Source.SetInnerHTML("Waiting for input...");

		App->State.IsCaptured = true;
		if (Source.IsActive())
		{
			App->Activity->CaptureKeyMap(nullptr);
			Output->Normal = false;
			Source.SetInnerHTML("");
		}
		else if (!Source.IsHovered() && App->Activity->CaptureKeyMap(Output))
		{
			Output->Normal = false;
			Source.SetInnerHTML("");
		}
	}
	else
	{
		const char* KeyCode = Activity::GetKeyCodeName(Output->Key);
		const char* KeyMod = Activity::GetKeyModName(Output->Mod);
		if (Source.GetInnerHTML().empty())
		{
			if (KeyCode != nullptr && KeyMod != nullptr)
				Source.SetInnerHTML(Form("%s + %s", KeyMod, KeyCode).R());
			else if (KeyCode != nullptr && !KeyMod)
				Source.SetInnerHTML(Form("%s", KeyCode).R());
			else if (!KeyCode && KeyMod != nullptr)
				Source.SetInnerHTML(Form("%s", KeyMod).R());
			else
				Source.SetInnerHTML("None");
		}

		if (Source.IsActive())
		{
			Source.SetInnerHTML("");
			Output->Normal = true;
		}
	}
}
bool ResolveColor4(GUI::Context* UI, const std::string& Id, Vector4* Output)
{
	if (!UI->GetElementById(0, Id).CastFormColor(Output, true))
		return false;

	UI->GetElementById(0, Id + "_color").SetProperty("background-color", Form("rgb(%u, %u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f), (unsigned int)(Output->W * 255.0f)).R());
	return true;
}
bool ResolveColor3(GUI::Context* UI, const std::string& Id, Vector3* Output)
{
	Vector4 Color = *Output;
	if (!UI->GetElementById(0, Id).CastFormColor(&Color, false))
		return false;

	*Output = Color;
	UI->GetElementById(0, Id + "_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f)).R());
	return true;
}
void ResolveAppearance(GUI::Context* UI, const std::string& Id, Appearance* Output)
{
	if (!Output)
		return;

	ResolveColor3(UI, Id + "_diffuse", &Output->Diffuse);
	ResolveTexture2D(UI, Id + "_diffuse_source", Output->GetDiffuseMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetDiffuseMap(New);
	});
	ResolveTexture2D(UI, Id + "_normal_source", Output->GetNormalMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetNormalMap(New);
	});
	ResolveTexture2D(UI, Id + "_metallic_source", Output->GetMetallicMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetMetallicMap(New);
	});
	ResolveTexture2D(UI, Id + "_roughness_source", Output->GetRoughnessMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetRoughnessMap(New);
	});
	ResolveTexture2D(UI, Id + "_height_source", Output->GetHeightMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetHeightMap(New);
	});
	ResolveTexture2D(UI, Id + "_occlusion_source", Output->GetOcclusionMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetOcclusionMap(New);
	});
	ResolveTexture2D(UI, Id + "_emission_source", Output->GetEmissionMap() != nullptr, [Output](Texture2D* New)
	{
		Output->SetEmissionMap(New);
	});

	UI->GetElementById(0, Id + "_material").CastFormInt64(&Output->Material);
	UI->GetElementById(0, Id + "_uv_x").CastFormFloat(&Output->TexCoord.X);
	UI->GetElementById(0, Id + "_uv_y").CastFormFloat(&Output->TexCoord.Y);
	UI->GetElementById(0, Id + "_ht_amnt").CastFormFloat(&Output->HeightAmount);
	UI->GetElementById(0, Id + "_ht_bias").CastFormFloat(&Output->HeightBias);
}
void ResolveModel(GUI::Context* UI, const std::string& Id, Components::Model* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetDrawable())
		{
			ResolveResource(Source, "model", [Output](const std::string& File)
			{
				Output->SetDrawable(Sandbox::Get()->Content->Load<Tomahawk::Graphics::Model>(File));
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->SetDrawable(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetDrawable() ? "Unassign source" : "Assign source");
}
void ResolveSkin(GUI::Context* UI, const std::string& Id, Components::Skin* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetDrawable())
		{
			ResolveResource(Source, "model", [Output](const std::string& File)
			{
				Output->SetDrawable(Sandbox::Get()->Content->Load<Tomahawk::Graphics::SkinModel>(File));
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->SetDrawable(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetDrawable() ? "Unassign source" : "Assign source");
}
void ResolveSoftBody(GUI::Context* UI, const std::string& Id, Components::SoftBody* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetBody())
		{
			ResolveResource(Source, "soft body", [Output](const std::string& File)
			{
				Output->Create(Sandbox::Get()->Content, File, 0);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetBody() ? "Unassign source" : "Assign source");
}
void ResolveRigidBody(GUI::Context* UI, const std::string& Id, Components::RigidBody* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetBody())
		{
			ResolveResource(Source, "rigid body", [Output](const std::string& File)
			{
				Output->Create(Sandbox::Get()->Content, File, 0, 0);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetBody() ? "Unassign source" : "Assign source");
}
void ResolveSliderConstraint(GUI::Context* UI, const std::string& Id, Components::SliderConstraint* Output, bool Ghost, bool Linear)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetConstraint())
		{
			ResolveEntity(Source, "slider constraint", [Output, Ghost, Linear](Entity* Source)
			{
				Output->Create(Source, Ghost, Linear);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign entity");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetConstraint() ? "Unassign entity" : "Assign entity");
}
void ResolveSkinAnimator(GUI::Context* UI, const std::string& Id, Components::SkinAnimator* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (Output->GetPath().empty())
		{
			ResolveResource(Source, "skin animation", [Output](const std::string& File)
			{
				Output->GetAnimation(Sandbox::Get()->Content, File);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->ClearAnimation();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(!Output->GetPath().empty() ? "Unassign source" : "Assign source");
}
void ResolveKeyAnimator(GUI::Context* UI, const std::string& Id, Components::KeyAnimator* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (Output->GetPath().empty())
		{
			ResolveResource(Source, "key animation", [Output](const std::string& File)
			{
				Output->GetAnimation(Sandbox::Get()->Content, File);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->ClearAnimation();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(!Output->GetPath().empty() ? "Unassign source" : "Assign source");
}
void ResolveAudioSource(GUI::Context* UI, const std::string& Id, Components::AudioSource* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (!Output->GetSource()->GetClip())
		{
			ResolveResource(Source, "audio clip", [Output](const std::string& File)
			{
				Output->GetSource()->SetClip(Sandbox::Get()->Content->Load<AudioClip>(File));
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->GetSource()->SetClip(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(Output->GetSource()->GetClip() ? "Unassign source" : "Assign source");
}
void ResolveScriptable(GUI::Context* UI, const std::string& Id, Components::Scriptable* Output)
{
	GUI::IElement Source = UI->GetElementById(0, Id);
	if (Source.IsActive())
	{
		if (Output->GetSource().empty())
		{
			ResolveResource(Source, "script", [Output](const std::string& File)
			{
				Output->SetSource(Components::Scriptable::SourceType_Resource, File);
			});
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->UnsetSource();
		}
	}
	else if (Source.GetInnerHTML().empty())
		Source.SetInnerHTML(!Output->GetSource().empty() ? "Unassign source" : "Assign source");
}