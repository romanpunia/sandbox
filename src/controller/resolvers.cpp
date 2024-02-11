#include "resolvers.h"
#include "../core/sandbox.h"

void ResolveResource(GUI::IElement& Target, const String& Name, const std::function<void(const String&)>& Callback, bool Changed)
{
	Sandbox* App = ((Sandbox*)Sandbox::Get());
	if (!App || !Callback)
		return;

	if (!App->GetResourceState(Name) && !Changed)
	{
		Target.SetInnerHTML("Awaiting source...");
		App->GetResource(Name, [Target, Callback](const String& File)
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
void ResolveEntity(GUI::IElement& Target, const String& Name, const std::function<void(Entity*)>& Callback, bool Changed)
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
void ResolveTexture2D(GUI::Context* UI, const String& Id, bool Assigned, const std::function<void(Texture2D*)>& Callback, bool Changed)
{
	GUI::IElement Source = UI->GetElementById(Id);
	if (Source.IsActive() && !Changed)
	{
		if (!Assigned)
		{
			ResolveResource(Source, "texture", [Callback](const String& File)
			{
				Callback(Sandbox::Get()->Content->Load<Vitex::Graphics::Texture2D>(File).Or(nullptr));
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
void ResolveKeyCode(GUI::Context* UI, const String& Id, KeyMap* Output, bool Changed)
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
		auto KeyCode = Video::GetKeyCodeAsLiteral(Output->Key);
		auto KeyMod = Video::GetKeyModAsLiteral(Output->Mod);
		if (Source.GetInnerHTML().empty())
		{
			if (!KeyCode.empty() && !KeyMod.empty())
				Source.SetInnerHTML(Stringify::Text("%s + %s", KeyMod.data(), KeyCode.data()));
			else if (!KeyCode.empty() && KeyMod.empty())
				Source.SetInnerHTML(Stringify::Text("%s", KeyCode.data()));
			else if (KeyCode.empty() && !KeyMod.empty())
				Source.SetInnerHTML(Stringify::Text("%s", KeyMod.data()));
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
bool ResolveColor4(GUI::Context* UI, const String& Id, Vector4* Output)
{
	if (!UI->GetElementById(Id).CastFormColor(Output, true))
		return false;

	UI->GetElementById(Id + "_color").SetProperty("background-color", Stringify::Text("rgb(%u, %u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f), (unsigned int)(Output->W * 255.0f)));
	return true;
}
bool ResolveColor3(GUI::Context* UI, const String& Id, Vector3* Output)
{
	Vector4 Color = *Output;
	if (!UI->GetElementById(Id).CastFormColor(&Color, false))
		return false;

	*Output = Color;
	UI->GetElementById(Id + "_color").SetProperty("background-color", Stringify::Text("rgb(%u, %u, %u)", (unsigned int)(Output->X * 255.0f), (unsigned int)(Output->Y * 255.0f), (unsigned int)(Output->Z * 255.0f)));
	return true;
}
void ResolveModel(GUI::Context* UI, const String& Id, Components::Model* Output, bool Changed)
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
			ResolveResource(Source, "model", [Output](const String& File)
			{
				auto* App = ((Sandbox*)Sandbox::Get());
				auto Instance = App->Content->Load<Vitex::Engine::Model>(File);
				Output->SetDrawable(Instance.Or(nullptr));
				App->SetMetadata(Output->GetEntity());
				if (Instance)
					Memory::Release(*Instance);
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
void ResolveSkin(GUI::Context* UI, const String& Id, Components::Skin* Output, bool Changed)
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
			ResolveResource(Source, "model", [Output](const String& File)
			{
				auto* App = ((Sandbox*)Sandbox::Get());
				auto Instance = App->Content->Load<Vitex::Engine::SkinModel>(File);
				Output->SetDrawable(Instance.Or(nullptr));
				App->SetMetadata(Output->GetEntity());
				if (Instance)
					Memory::Release(*Instance);
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
void ResolveSoftBody(GUI::Context* UI, const String& Id, Components::SoftBody* Output, bool Changed)
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
			ResolveResource(Source, "soft body", [Output](const String& File)
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
void ResolveRigidBody(GUI::Context* UI, const String& Id, Components::RigidBody* Output, bool Changed)
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
			ResolveResource(Source, "rigid body", [Output](const String& File)
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
void ResolveSliderConstraint(GUI::Context* UI, const String& Id, Components::SliderConstraint* Output, bool Ghost, bool Linear, bool Changed)
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
void ResolveSkinAnimator(GUI::Context* UI, const String& Id, Components::SkinAnimator* Output, bool Changed)
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
			ResolveResource(Source, "skin animation", [Output](const String& File)
			{
				auto Instance = Sandbox::Get()->Content->Load<SkinAnimation>(File);
				Output->SetAnimation(Instance.Or(nullptr));
				if (Instance)
					Memory::Release(*Instance);
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
void ResolveKeyAnimator(GUI::Context* UI, const String& Id, Components::KeyAnimator* Output, bool Changed)
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
			ResolveResource(Source, "key animation", [Output](const String& File)
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
void ResolveAudioSource(GUI::Context* UI, const String& Id, Components::AudioSource* Output, bool Changed)
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
			ResolveResource(Source, "audio clip", [Output](const String& File)
			{
				auto Instance = Sandbox::Get()->Content->Load<AudioClip>(File);
				Output->GetSource()->SetClip(Instance.Or(nullptr));
				if (Instance)
					Memory::Release(*Instance);
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
void ResolveScriptable(GUI::Context* UI, const String& Id, Components::Scriptable* Output, bool Changed)
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
			ResolveResource(Source, "script", [Output](const String& File)
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
