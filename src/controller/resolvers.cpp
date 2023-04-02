#include "resolvers.h"
#include "../core/sandbox.h"

void ResolveResource(GUI::IElement& Target, const std::string& Name, const std::function<void(const std::string&)>& Callback, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App || !Callback)
		return;

	if (!App->GetResourceState(Name) && !Changed)
	{
		Target.SetInnerHTML("Awaiting source...");
		App->GetResource(Name, [Target, Callback](const std::string& File)
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
void ResolveEntity(GUI::IElement& Target, const std::string& Name, const std::function<void(Entity*)>& Callback, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App || !Callback)
		return;

	if (!App->GetEntityState(Name) && !Changed)
	{
		Target.SetInnerHTML("Awaiting entity...");
		App->GetEntity(Name, [Target, Callback](Entity* Source)
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
void ResolveTexture2D(GUI::Context* UI, const std::string& Id, bool Assigned, const std::function<void(Texture2D*)>& Callback, bool Changed)
{
	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Assigned)
		{
			ResolveResource(Source, "texture", [Callback](const std::string& File)
			{
				Callback(Sandbox::Get()->Content->Load<Edge::Graphics::Texture2D>(File));
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Callback(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Assigned ? "Unassign source" : "Assign source");
	}
}
void ResolveKeyCode(GUI::Context* UI, const std::string& Id, KeyMap* Output, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App)
		return;

	GUI::IElement Source = UI->GetElementById(Id);
	if (Output->Normal && !Changed)
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
	if (!UI->GetElementById(Id).CastFormColor(Output, true))
		return false;

	UI->GetElementById(Id + "_color").SetProperty("background-color", Form("rgb(%u, %u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f), (unsigned int)(Output->W * 255.0f)).R());
	return true;
}
bool ResolveColor3(GUI::Context* UI, const std::string& Id, Vector3* Output)
{
	Vector4 Color = *Output;
	if (!UI->GetElementById(Id).CastFormColor(&Color, false))
		return false;

	*Output = Color;
	UI->GetElementById(Id + "_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f)).R());
	return true;
}
void ResolveModel(GUI::Context* UI, const std::string& Id, Components::Model* Output, bool Changed)
{
	static Components::Model* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetDrawable())
		{
			ResolveResource(Source, "model", [Output](const std::string& File)
			{
				auto* App = ((Sandbox*)Sandbox::Get());
				auto* Instance = App->Content->Load<Edge::Engine::Model>(File);
				Output->SetDrawable(Instance);
				App->SetMetadata(Output->GetEntity());
				ED_RELEASE(Instance);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->SetDrawable(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Output->GetDrawable() ? "Unassign source" : "Assign source");
	}
}
void ResolveSkin(GUI::Context* UI, const std::string& Id, Components::Skin* Output, bool Changed)
{
	static Components::Skin* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetDrawable())
		{
			ResolveResource(Source, "model", [Output](const std::string& File)
			{
				auto* App = ((Sandbox*)Sandbox::Get());
				auto* Instance = App->Content->Load<Edge::Engine::SkinModel>(File);
				Output->SetDrawable(Instance);
				App->SetMetadata(Output->GetEntity());
				ED_RELEASE(Instance);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->SetDrawable(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Output->GetDrawable() ? "Unassign source" : "Assign source");
	}
}
void ResolveSoftBody(GUI::Context* UI, const std::string& Id, Components::SoftBody* Output, bool Changed)
{
	static Components::SoftBody* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetBody())
		{
			ResolveResource(Source, "soft body", [Output](const std::string& File)
			{
				Output->Load(File, 0.0f);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Output->GetBody() ? "Unassign source" : "Assign source");
	}
}
void ResolveRigidBody(GUI::Context* UI, const std::string& Id, Components::RigidBody* Output, bool Changed)
{
	static Components::RigidBody* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetBody())
		{
			ResolveResource(Source, "rigid body", [Output](const std::string& File)
			{
				Output->Load(File, 0.0f, 0.0f);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Output->GetBody() ? "Unassign source" : "Assign source");
	}
}
void ResolveSliderConstraint(GUI::Context* UI, const std::string& Id, Components::SliderConstraint* Output, bool Ghost, bool Linear, bool Changed)
{
	static Components::SliderConstraint* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetConstraint())
		{
			ResolveEntity(Source, "slider constraint", [Output, Ghost, Linear](Entity* Source)
			{
				Output->Load(Source, Ghost, Linear);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign entity");
			Output->Clear();
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetEntity("", nullptr);
		Source.SetInnerHTML(Output->GetConstraint() ? "Unassign entity" : "Assign entity");
	}
}
void ResolveSkinAnimator(GUI::Context* UI, const std::string& Id, Components::SkinAnimator* Output, bool Changed)
{
	static Components::SkinAnimator* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (Output->GetPath().empty())
		{
			ResolveResource(Source, "skin animation", [Output](const std::string& File)
			{
				auto* Instance = Sandbox::Get()->Content->Load<SkinAnimation>(File);
				Output->SetAnimation(Instance);
				ED_RELEASE(Instance);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->SetAnimation(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(!Output->GetPath().empty() ? "Unassign source" : "Assign source");
	}
}
void ResolveKeyAnimator(GUI::Context* UI, const std::string& Id, Components::KeyAnimator* Output, bool Changed)
{
	static Components::KeyAnimator* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (Output->GetPath().empty())
		{
			ResolveResource(Source, "key animation", [Output](const std::string& File)
			{
				Output->LoadAnimation(File);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->ClearAnimation();
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(!Output->GetPath().empty() ? "Unassign source" : "Assign source");
	}
}
void ResolveAudioSource(GUI::Context* UI, const std::string& Id, Components::AudioSource* Output, bool Changed)
{
	static Components::AudioSource* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Output->GetSource()->GetClip())
		{
			ResolveResource(Source, "audio clip", [Output](const std::string& File)
			{
				auto* Instance = Sandbox::Get()->Content->Load<AudioClip>(File);
				Output->GetSource()->SetClip(Instance);
				ED_RELEASE(Instance);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->GetSource()->SetClip(nullptr);
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(Output->GetSource()->GetClip() ? "Unassign source" : "Assign source");
	}
}
void ResolveScriptable(GUI::Context* UI, const std::string& Id, Components::Scriptable* Output, bool Changed)
{
	static Components::Scriptable* Last = nullptr;
	if (Last != Output)
	{
		Last = Output;
		Changed = true;
	}

	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (Output->GetSource().empty())
		{
			ResolveResource(Source, "script", [Output](const std::string& File)
			{
				Output->LoadSource(Components::Scriptable::SourceType::Resource, File);
			}, Changed);
		}
		else
		{
			Source.SetInnerHTML("Assign source");
			Output->UnloadSource();
		}
	}
	else if (Source.GetInnerHTML().empty() || Changed)
	{
		((Sandbox*)Sandbox::Get())->GetResource("", nullptr);
		Source.SetInnerHTML(!Output->GetSource().empty() ? "Unassign source" : "Assign source");
	}
}
