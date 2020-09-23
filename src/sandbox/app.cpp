#define NOMINMAX
#include "app.h"

void CGizmoTransformRender::DrawCircle(const tvector3 &orig, float r, float g, float b, const tvector3 &vtx, const tvector3 &vty)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(Matrix4x4::Create(App->State.Gizmo.Position(), App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos((2 * ZPI / 50) * (float)i);
		vt += vty * sin((2 * ZPI / 50) * (float)i);

		App->Renderer->Emit();
		App->Renderer->Position(vt.x, vt.y, vt.z);
		App->Renderer->Color(r, g, b, 1);
	}

	App->Renderer->End();
}
void CGizmoTransformRender::DrawCircleHalf(const tvector3 &orig, float r, float g, float b, const tvector3 &vtx, const tvector3 &vty, tplane &camPlan)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(Matrix4x4::Create(App->State.Gizmo.Position(), App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30)*i);
		vt += vty * sin((ZPI / 30)*i);

		if (camPlan.DotNormal(vt) > 0)
		{
			App->Renderer->Emit();
			App->Renderer->Position(vt.x, vt.y, vt.z);
			App->Renderer->Color(r, g, b, 1);
		}
	}

	App->Renderer->End();
	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(Matrix4x4::Create(App->State.Gizmo.Position(), -1, App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30) * (float)i);
		vt += vty * sin((ZPI / 30) * (float)i);

		if (camPlan.DotNormal(vt) > 0)
		{
			App->Renderer->Emit();
			App->Renderer->Position(vt.x, vt.y, vt.z);
			App->Renderer->Color(r, g, b, 1);
		}
	}

	App->Renderer->End();
}
void CGizmoTransformRender::DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx, const tvector3 &vty, float fct, float fct2, const tvector4 &col)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_List);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Position(orig.x, orig.y, orig.z);
	App->Renderer->Color(col.x, col.y, col.z, 1);
	App->Renderer->Emit();
	App->Renderer->Position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	App->Renderer->Color(col.x, col.y, col.z, 1);
	App->Renderer->End();
	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Triangle_Strip);
	App->Renderer->Transform(ViewProjection);

	for (uint32_t i = 0; i <= 30; i++)
	{
		tvector3 pt;
		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += axis * fct2;
		pt += orig;

		App->Renderer->Emit();
		App->Renderer->Position(pt.x, pt.y, pt.z);
		App->Renderer->Color(col.x, col.y, col.z, 1);

		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += axis * fct2;
		pt += orig;

		App->Renderer->Emit();
		App->Renderer->Position(pt.x, pt.y, pt.z);
		App->Renderer->Color(col.x, col.y, col.z, 1);
		App->Renderer->Emit();
		App->Renderer->Position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
		App->Renderer->Color(col.x, col.y, col.z, 1);
	}

	App->Renderer->End();
}
void CGizmoTransformRender::DrawCamem(const tvector3 &orig, const tvector3 &vtx, const tvector3 &vty, float ng)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Triangle_Strip);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Color(1, 1, 0, 0.5f);
	App->Renderer->Position(orig.x, orig.y, orig.z);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50)*i);
		vt += vty * sin(((ng) / 50)*i);
		vt += orig;

		App->Renderer->Emit();
		App->Renderer->Color(1, 1, 0, 0.5f);
		App->Renderer->Position(vt.x, vt.y, vt.z);
	}

	App->Renderer->Emit();
	App->Renderer->Color(1, 1, 0, 0.5f);
	App->Renderer->Position(orig.x, orig.y, orig.z);
	App->Renderer->End();
	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Color(1, 1, 0.2F, 1);
	App->Renderer->Position(orig.x, orig.y, orig.z);

	for (INT i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50)*i);
		vt += vty * sin(((ng) / 50)*i);
		vt += orig;

		App->Renderer->Emit();
		App->Renderer->Color(1, 1, 0.2F, 1);
		App->Renderer->Position(vt.x, vt.y, vt.z);
	}

	App->Renderer->Emit();
	App->Renderer->Color(1, 1, 0.2F, 1);
	App->Renderer->Position(orig.x, orig.y, orig.z);
	App->Renderer->End();
}
void CGizmoTransformRender::DrawQuadAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx, const tvector3 &vty, float fct, float fct2, const tvector4 &col)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_List);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Position(orig.x, orig.y, orig.z);
	App->Renderer->Color(col.x, col.y, col.z, 1);
	App->Renderer->Emit();
	App->Renderer->Position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	App->Renderer->Color(col.x, col.y, col.z, 1);
	App->Renderer->End();
}
void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3 &axisV)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	tvector3 pts[4];
	pts[0] = orig;
	pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisU + axisV) * size;
	pts[3] = orig + (axisV * size);

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Triangle_Strip);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->End();
	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->End();
}
void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
	auto* App = Sandbox::Get()->As<Sandbox>();
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = App->Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();

	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Triangle_Strip);
	App->Renderer->Transform(ViewProjection);

	tvector3 pts[3];
	pts[0] = orig;
	pts[1] = (axisU);
	pts[2] = (axisV);
	pts[1] *= size;
	pts[2] *= size;
	pts[1] += orig;
	pts[2] += orig;

	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0, 0.5f);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->End();
	App->Renderer->Begin();
	App->Renderer->Topology(PrimitiveTopology_Line_Strip);
	App->Renderer->Transform(ViewProjection);
	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->Emit();
	App->Renderer->Position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->Color(1, 1, 0.2f, 1);
	else
		App->Renderer->Color(1, 1, 1, 0.6f);

	App->Renderer->End();
}

static void ReactApplicationLoop(GUI::Body* Target, Sandbox* App)
{
	if (App->State.Asset != nullptr)
		Target->SetGlobal("scene.resources", std::to_string(App->State.Asset->Files.size() + App->State.Asset->Directories.size()));

	std::ostringstream Frames;
	Frames.precision(1);
	Frames << std::fixed << App->State.Frames << " fps";

	Target->SetGlobal("scene.status", App->State.Status);
	Target->SetGlobal("scene.frames", Frames.str());
	Target->SetGlobal("scene.entities", std::to_string(App->Scene->GetEntityCount() - 1));
	Target->SetGlobal("scene.materials", std::to_string(App->Scene->GetMaterialCount()));
	Target->SetGlobal("scene.assets", !App->State.Filename.empty() ? "Assets (choose " + App->State.Filename + " file)" : "Assets");
	Target->SetGlobal("selection.entity", std::to_string(App->Selection.Entity ? App->Selection.Entity->Id : -1));
	Target->SetGlobal("selection.material", std::to_string(App->Selection.Material ? (int)App->Selection.Material->Id : -1));
	Target->SetGlobal("selection.resource", App->Selection.Folder && App->Selection.Folder->Files.size() > 0 ? "0" : "-1");

	switch (App->Selection.Window)
	{
		case Inspector_Entity:
			Target->SetGlobal("selection.window", "entity");
			break;
		case Inspector_Material:
			Target->SetGlobal("selection.window", "material");
			break;
		case Inspector_Settings:
			Target->SetGlobal("selection.window", "settings");
			break;
		case Inspector_Materials:
			Target->SetGlobal("selection.window", "materials");
			break;
		case Inspector_ImportModel:
			Target->SetGlobal("selection.window", "import-model");
			break;
		case Inspector_ImportAnimation:
			Target->SetGlobal("selection.window", "import-animation");
			break;
		case Inspector_None:
		default:
			Target->SetGlobal("selection.window", "none");
			break;
	}
}
static void ReactApplicationDraggable(GUI::DrawLabel* Target, Sandbox* App)
{
	App->State.Label = Target;
	if (!App->State.Draggable)
	{
		Target->Source.Text.clear();
		return;
	}

	Target->Source.Text = App->GetName(App->State.Draggable);
	Target->Source.Width = 180;
	Target->Source.X = App->Activity->GetCursorPosition().X;
	Target->Source.Y = App->Activity->GetCursorPosition().Y;
	Target->Build();
}
static void ReactPreviewGroupInputs(GUI::Widget* Target, Sandbox* App)
{
	App->State.IsSceneFocused = Target->IsWidgetHovered() && App->State.IsCameraActive;
}
static void ReactMenuFileOpen(GUI::HeaderItem* Target, Sandbox* App)
{
	App->GetResource("scene", [=](const std::string& File)
	{
		App->Resource.NextPath = File;
	});
}
static void ReactMenuFileSave(GUI::HeaderItem* Target, Sandbox* App)
{
	std::string Path;
	if (!OS::WantFileSave("Save scene", App->Content->GetEnvironment(), "*.xml,*.json,*.tsg", "Serialized scene (*.xml, *.json, *.tsg)", &Path))
		return;

	ContentMap Args;
	if (Stroke(&Path).EndsWith(".tsg"))
		Args["BIN"] = ContentKey(true);
	else if (Stroke(&Path).EndsWith(".json"))
		Args["JSON"] = ContentKey(true);
	else
		Args["XML"] = ContentKey(true);

	App->Scene->RemoveEntity(App->State.Camera, false);
	App->Content->Save<SceneGraph>(Path, App->Scene, &Args);
	App->Scene->AddEntity(App->State.Camera);

	if (!App->Scene->IsActive())
		App->Scene->SetCamera(App->State.Camera);

	App->UpdateHierarchy();
	App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Scene was saved");
	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactMenuFileCancel(GUI::HeaderItem* Target, Sandbox* App)
{
	App->GetResource("", nullptr);
}
static void ReactMenuEntityAdd(GUI::HeaderItem* Target, Sandbox* App)
{
	Entity* Value = new Entity(App->Scene);
	if (App->Scene->AddEntity(Value))
	{
		App->SetSelection(Inspector_Entity);
		App->Selection.Entity = Value;
	}
}
static void ReactMenuEntityRemove(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		App->Scene->RemoveEntity(App->Selection.Entity, true);
		App->SetSelection();
	}
}
static void ReactMenuEntityMove(GUI::HeaderItem* Target, Sandbox* App)
{
	App->Selection.Gizmo = App->Resource.Gizmo[App->Selection.Move = 0];
	App->Selection.Gizmo->SetEditMatrix(App->State.Gizmo.Row);
	App->Selection.Gizmo->SetDisplayScale(App->State.GizmoScale);
	App->Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
}
static void ReactMenuEntityRotate(GUI::HeaderItem* Target, Sandbox* App)
{
	App->Selection.Gizmo = App->Resource.Gizmo[App->Selection.Move = 1];
	App->Selection.Gizmo->SetEditMatrix(App->State.Gizmo.Row);
	App->Selection.Gizmo->SetDisplayScale(App->State.GizmoScale);
	App->Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
}
static void ReactMenuEntityScale(GUI::HeaderItem* Target, Sandbox* App)
{
	App->Selection.Gizmo = App->Resource.Gizmo[App->Selection.Move = 2];
	App->Selection.Gizmo->SetEditMatrix(App->State.Gizmo.Row);
	App->Selection.Gizmo->SetDisplayScale(App->State.GizmoScale);
	App->Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
}
static void ReactMenuComponentsSkinAnimator(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::SkinAnimator>();
}
static void ReactMenuComponentsKeyAnimator(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::KeyAnimator>();
}
static void ReactMenuComponentsEmitterAnimator(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::EmitterAnimator>();
}
static void ReactMenuComponentsListener(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::AudioListener>();
}
static void ReactMenuComponentsSource(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::AudioSource>();
}
static void ReactMenuComponentsReverbEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::ReverbEffect());
	}
}
static void ReactMenuComponentsChorusEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::ChorusEffect());
	}
}
static void ReactMenuComponentsDistortionEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::DistortionEffect());
	}
}
static void ReactMenuComponentsEchoEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::EchoEffect());
	}
}
static void ReactMenuComponentsFlangerEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::FlangerEffect());
	}
}
static void ReactMenuComponentsFrequencyShifterEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::FrequencyShifterEffect());
	}
}
static void ReactMenuComponentsVocalMorpherEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::VocalMorpherEffect());
	}
}
static void ReactMenuComponentsPitchShifterEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::PitchShifterEffect());
	}
}
static void ReactMenuComponentsRingModulatorEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::RingModulatorEffect());
	}
}
static void ReactMenuComponentsAutowahEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::AutowahEffect());
	}
}
static void ReactMenuComponentsCompressorEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::CompressorEffect());
	}
}
static void ReactMenuComponentsEqualizerEffect(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::AudioSource>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::AudioSource>();

		Source->GetSource()->AddEffect(new Effects::EqualizerEffect());
	}
}
static void ReactMenuComponentsModel(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Model>();
}
static void ReactMenuComponentsLimpidModel(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LimpidModel>();
}
static void ReactMenuComponentsSkin(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Skin>();
}
static void ReactMenuComponentsLimpidSkin(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LimpidSkin>();
}
static void ReactMenuComponentsEmitter(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Emitter>();
}
static void ReactMenuComponentsLimpidEmitter(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LimpidEmitter>();
}
static void ReactMenuComponentsDecal(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Decal>();
}
static void ReactMenuComponentsLimpidDecal(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LimpidDecal>();
}
static void ReactMenuComponentsPointLight(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::PointLight>();
}
static void ReactMenuComponentsSpotLight(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::SpotLight>();
}
static void ReactMenuComponentsLineLight(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LineLight>();
}
static void ReactMenuComponentsReflectionProbe(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::ReflectionProbe>();
}
static void ReactMenuComponentsRigidBody(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::RigidBody>();
}
static void ReactMenuComponentsSoftBody(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::SoftBody>();
}
static void ReactMenuComponentsLimpidSoftBody(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::LimpidSoftBody>();
}
static void ReactMenuComponentsSliderConstraint(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::SliderConstraint>();
}
static void ReactMenuComponentsAcceleration(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Acceleration>();
}
static void ReactMenuComponentsFly(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Fly>();
}
static void ReactMenuComponentsFreeLook(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::FreeLook>();
}
static void ReactMenuComponentsCamera(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Camera>();
}
static void ReactMenuComponents3DCamera(GUI::HeaderItem* Target, Sandbox* App)
{
	if (!App->Selection.Entity)
		return;

	Components::Camera* Camera = App->Selection.Entity->AddComponent<Components::Camera>();
	Camera->GetRenderer()->AddRenderer<Renderers::DepthRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::ProbeRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::ModelRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::SkinRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::EmitterRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::DecalRenderer>();
	Camera->GetRenderer()->AddRenderer<Renderers::LightRenderer>();
}
static void ReactMenuComponentsScriptable(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
		App->Selection.Entity->AddComponent<Components::Scriptable>();
}
static void ReactMenuComponentsModelRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::ModelRenderer>();
	}
}
static void ReactMenuComponentsSkinRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::SkinRenderer>();
	}
}
static void ReactMenuComponentsEmitterRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::EmitterRenderer>();
	}
}
static void ReactMenuComponentsDepthRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::DepthRenderer>();
	}
}
static void ReactMenuComponentsLightRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::LightRenderer>();
	}
}
static void ReactMenuComponentsProbeRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::ProbeRenderer>();
	}
}
static void ReactMenuComponentsLimpidRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::LimpidRenderer>();
	}
}
static void ReactMenuComponentsReflectionsRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::ReflectionsRenderer>();
	}
}
static void ReactMenuComponentsEmissionRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::EmissionRenderer>();
	}
}
static void ReactMenuComponentsGlitchRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::GlitchRenderer>();
	}
}
static void ReactMenuComponentsDOFRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::DepthOfFieldRenderer>();
	}
}
static void ReactMenuComponentsAORenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::AmbientOcclusionRenderer>();
	}
}
static void ReactMenuComponentsDORenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::DirectOcclusionRenderer>();
	}
}
static void ReactMenuComponentsToneRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::ToneRenderer>();
	}
}
static void ReactMenuComponentsGUIRenderer(GUI::HeaderItem* Target, Sandbox* App)
{
	if (App->Selection.Entity != nullptr)
	{
		auto* Source = App->Selection.Entity->GetComponent<Components::Camera>();
		if (!Source)
			Source = App->Selection.Entity->AddComponent<Components::Camera>();

		Source->GetRenderer()->AddRenderer<Renderers::GUIRenderer>();
	}
}
static void ReactSceneAddMaterial(GUI::HeaderItem* Target, Sandbox* App)
{
	App->SetSelection(Inspector_Material);
	App->Selection.Material = App->Scene->AddMaterial("Material", Material());
}
static void ReactSceneImportModel(GUI::HeaderItem* Target, Sandbox* App)
{
	App->SetSelection(Inspector_ImportModel);
}
static void ReactSceneImportSkinAnimation(GUI::HeaderItem* Target, Sandbox* App)
{
	App->SetSelection(Inspector_ImportAnimation);
}
static void ReactSceneExportSkinAnimation(GUI::HeaderItem* Target, Sandbox* App)
{
	if (!App->Selection.Entity || !App->Selection.Entity->GetComponent<Components::SkinAnimator>())
	{
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select entity with skin animator to export");
		App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		App->Activity->Message.Result(nullptr);
		return;
	}

	std::string Path;
	if (!OS::WantFileSave("Save skin animation", App->Content->GetEnvironment(), "*.xml,*.json,*.tsc", "Serialized skin animation (*.xml, *.json, *.tsc)", &Path))
		return;

	Document* Result = new Document();
	Result->Name = "skin-animation";

	auto* Animator = App->Selection.Entity->GetComponent<Components::SkinAnimator>();
	if (!NMake::Pack(Result, Animator->Clips))
		return;

	ContentMap Args;
	if (Stroke(&Path).EndsWith(".tsc"))
		Args["BIN"] = ContentKey(true);
	else if (Stroke(&Path).EndsWith(".json"))
		Args["JSON"] = ContentKey(true);
	else
		Args["XML"] = ContentKey(true);

	if (!App->Content->Save<Document>(Path, Result, &Args))
		App->Activity->Message.Setup(AlertType_Error, "Sandbox", "Skin animation cannot be saved");
	else
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Skin animation was saved");

	delete Result;
	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactSceneExportKeyAnimation(GUI::HeaderItem* Target, Sandbox* App)
{
	if (!App->Selection.Entity || !App->Selection.Entity->GetComponent<Components::KeyAnimator>())
	{
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select entity with key animator to export");
		App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		App->Activity->Message.Result(nullptr);
		return;
	}

	std::string Path;
	if (!OS::WantFileSave("Save key animation", App->Content->GetEnvironment(), "*.xml,*.json,*.tkc", "Serialized key animation (*.xml, *.json, *.tsc)", &Path))
		return;

	Document* Result = new Document();
	Result->Name = "key-animation";

	auto* Animator = App->Selection.Entity->GetComponent<Components::KeyAnimator>();
	if (!NMake::Pack(Result, Animator->Clips))
		return;

	ContentMap Args;
	if (Stroke(&Path).EndsWith(".tkc"))
		Args["BIN"] = ContentKey(true);
	else if (Stroke(&Path).EndsWith(".json"))
		Args["JSON"] = ContentKey(true);
	else
		Args["XML"] = ContentKey(true);

	if (!App->Content->Save<Document>(Path, Result, &Args))
		App->Activity->Message.Setup(AlertType_Error, "Sandbox", "Key animation cannot be saved");
	else
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Key animation was saved");

	delete Result;
	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactSceneImportMaterial(GUI::HeaderItem* Target, Sandbox* App)
{
	App->GetResource("material", [=](const std::string& File)
	{
		Document* Result = App->Content->Load<Document>(File, nullptr);
		if (!Result)
			return;

		Material New;
		if (!NMake::Unpack(Result, &New))
			return;

		std::string Name = File;
		App->GetPathName(Name);

		App->SetSelection(Inspector_Material);
		App->Selection.Material = App->Scene->AddMaterial(Name, New);
	});
}
static void ReactSceneExportMaterial(GUI::HeaderItem* Target, Sandbox* App)
{
	if (!App->Selection.Material)
	{
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select material to export");
		App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		App->Activity->Message.Result(nullptr);
		return;
	}

	std::string Path;
	if (!OS::WantFileSave("Save material", App->Content->GetEnvironment(), "*.xml,*.json,*.tmd", "Serialized material (*.xml, *.json, *.tmd)", &Path))
		return;

	Document* Result = new Document();
	Result->Name = "material";

	if (!NMake::Pack(Result, *App->Selection.Material))
		return;

	ContentMap Args;
	if (Stroke(&Path).EndsWith(".tmd"))
		Args["BIN"] = ContentKey(true);
	else if (Stroke(&Path).EndsWith(".json"))
		Args["JSON"] = ContentKey(true);
	else
		Args["XML"] = ContentKey(true);

	if (!App->Content->Save<Document>(Path, Result, &Args))
		App->Activity->Message.Setup(AlertType_Error, "Sandbox", "Material cannot be saved");
	else
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Material was saved");

	delete Result;
	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactSceneMaterials(GUI::HeaderItem* Target, Sandbox* App)
{
	App->SetSelection(Inspector_Materials);
}
static void ReactSceneSettings(GUI::HeaderItem* Target, Sandbox* App)
{
	App->SetSelection(Inspector_Settings);
}
static void ReactSceneSwitchCameras(GUI::HeaderItem* Target, Sandbox* App, int Type)
{
	if (Type < 0 && !App->Scene->IsActive())
		return;

	auto* Cameras = App->Scene->GetComponents<Components::Camera>();
	if (Type == 1 || (Type < 0 && App->Scene->GetCamera() == App->State.Camera->GetComponent<Components::Camera>()))
	{
		for (auto It = Cameras->Begin(); It != Cameras->End(); It++)
		{
			Components::Camera* Base = (*It)->As<Components::Camera>();
			if (Base->GetEntity() == App->State.Camera)
				continue;

			if (Base->IsActive())
			{
				App->Scene->SetCamera(Base->GetEntity());
				App->State.IsCameraActive = false;
				break;
			}
		}
	}
	else if (Type == 2 || Type < 0)
	{
		App->Scene->AddEntity(App->State.Camera);
		App->Scene->SetCamera(App->State.Camera);
		App->Scene->GetRenderer()->EnableFrustumCull = true;
		App->Scene->GetRenderer()->EnableOcclusionCull = false;
		App->State.IsCameraActive = true;
	}
}
static void ReactSceneDemo(GUI::HeaderItem* Target, Sandbox* App)
{
	App->Activity->Message.Setup(AlertType_Warning, "Sandbox", "Editor's state will be flushed before start");
	App->Activity->Message.Button(AlertConfirm_Escape, "No", 1);
	App->Activity->Message.Button(AlertConfirm_Return, "Yes", 2);
	App->Activity->Message.Result([App](int Button)
	{
		if (Button == 2)
		{
			Demo::SetSource(App->Resource.ScenePath);
			App->Restate(ApplicationState_Terminated);
		}
	});
}
static void ReactHierarchyViewItem(GUI::Template* Target, Sandbox* App, Entity* Base)
{
	static uint64_t Time = 0;
	std::string Name = App->GetName(Base);
	bool Hovered = false;

	if (Base->Transform->GetChildCount() > 0)
	{
		GUI::Tree* Tree = Target->Get<GUI::Tree>("tree");
		if (!Tree)
			return;

		Target->ComposeStateful(Tree, [=, &Hovered]()
		{
			Tree->Source.Text = Name;
			Tree->SetValue(Base == App->Selection.Entity);
			Hovered = Tree->IsHovered();
		}, [=](bool Active)
		{
			if (Tree->IsClicked())
			{
				if (App->Selection.Entity != Base)
				{
					App->SetSelection(Inspector_Entity);
					App->Selection.Entity = Base;
				}
				else
					App->SetSelection(Inspector_None);
			}

			std::vector<Transform*>* Childs = Base->Transform->GetChilds();
			if (!Active || !Childs)
				return Active;

			for (auto& Node : *Childs)
			{
				if (Node->UserPointer)
					ReactHierarchyViewItem(Target, App, Node->Ptr<Entity>());
			}

			return Active;
		});
	}
	else
	{
		GUI::Selectable* Selectable = Target->Get<GUI::Selectable>("selectable");
		if (!Selectable)
			return;

		Selectable->Source.Text = Name;
		Selectable->SetValue(Base == App->Selection.Entity);

		Target->Compose("layout", nullptr);
		Hovered = Selectable->IsWidgetHovered();

		Target->Compose(Selectable, [=](bool Active)
		{
			if (Active)
			{
				if (App->Selection.Entity != Base)
				{
					App->SetSelection(Inspector_Entity);
					App->Selection.Entity = Base;
				}
				else
					App->SetSelection(Inspector_None);
			}

			return Active;
		});
	}

	if (!Hovered)
		return;

	if (App->Activity->IsKeyDown(KeyCode::KeyCode_CURSORLEFT))
	{
		if (!App->State.Draggable)
		{
			if (App->Activity->IsKeyDownHit(KeyCode::KeyCode_CURSORLEFT))
				Time = DateTime().Milliseconds();

			if (DateTime().Milliseconds() - Time > 100)
				App->State.Draggable = Base;
		}
	}
	else if (App->State.Draggable && App->State.Draggable != Base)
	{
		App->State.Draggable->Transform->SetRoot(Base->Transform);
		App->State.Draggable = nullptr;
	}
}
static void ReactHierarchyView(GUI::Template* Target, Sandbox* App)
{
	size_t C = std::min<size_t>(App->Scene->GetEntityCount(), 256);
	for (size_t i = 0; i < C; i++)
	{
		if (App->State.Camera == App->Scene->GetEntity(i))
			continue;

		if (App->Scene->GetEntity(i)->Transform->GetRoot())
			continue;

		Entity* Base = App->Scene->GetEntity(i);
		ReactHierarchyViewItem(Target, App, Base);
	}

	if (!App->State.Draggable)
		return;

	if (App->State.Label != nullptr)
	{
		if (!App->State.IsDragHovered)
			App->State.Label->Source.Color = Vector4(255, 255, 175, 255);
		else
			App->State.Label->Source.Color = Vector4(255, 255, 255, 255);
	}

	if (!App->Activity->IsKeyDown(KeyCode::KeyCode_CURSORLEFT))
	{
		if (App->State.Target.empty())
		{
			if (App->State.IsDragHovered)
				App->State.Draggable->Transform->SetRoot(nullptr);
		}
		else if (App->State.OnEntity)
		{
			auto Callback = App->State.OnEntity;
			auto Entity = App->State.Draggable;
			App->GetEntity("__got__", nullptr);

			Callback(Entity);
		}

		App->State.Draggable = nullptr;
	}
}
static void ReactHierarchyGroupInputs(GUI::Widget* Target, Sandbox* App)
{
	App->State.IsDragHovered = Target->IsWidgetHovered();
}
static void ReactHierarchyToggle(GUI::Button* Target, Sandbox* App)
{
	if (!App->Scene->IsActive())
	{
		ContentMap Args;
		Args["XML"] = ContentKey(true);

		App->Scene->RemoveEntity(App->State.Camera, false);
		App->Content->Save<SceneGraph>("./system/cache.xml", App->Scene, &Args);
		App->Scene->AddEntity(App->State.Camera);
		App->Scene->SetActive(true);
		App->Scene->ScriptHook();

		ReactSceneSwitchCameras(nullptr, App, 1);
		Target->Source.Text = "Deactivate scene";
	}
	else
	{
		App->Resource.NextPath = "./system/cache.xml";
		App->UpdateScene();
		Target->Source.Text = "Activate scene";
	}
}
static void ReactInspectorText(GUI::Template* Target, GUI::Text* Text, const std::string& Value)
{
	if (!Target || !Text)
		return;

	Text->Source.Text = Value;
	Target->Compose(Text, nullptr);
}
static bool ReactInspectorButton(GUI::Template* Target, GUI::Button* Button, const std::string& Name)
{
	bool Result = false;
	if (!Target || !Button)
		return Result;

	Button->Source.Text = Name;
	Target->Compose(Button, [&](bool Active)
	{
		Result = Active;
		return Active;
	});

	return Result;
}
static bool ReactInspectorFloat(GUI::Template* Target, GUI::Property* Property, float* Value, float Min, float Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = false;
	Property->SetValue((double)*Value);
	Property->Source.Max = (double)Max;
	Property->Source.Min = (double)Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = (float)Property->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorRadians(GUI::Template* Target, GUI::Property* Property, float* Value, float Min, float Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = false;
	Property->SetValue((double)*Value * Math<double>::Rad2Deg());
	Property->Source.Max = (double)Max;
	Property->Source.Min = (double)Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = (float)Property->GetValue() * Math<float>::Deg2Rad();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorDouble(GUI::Template* Target, GUI::Property* Property, double* Value, double Min, double Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = false;
	Property->SetValue(*Value);
	Property->Source.Max = Max;
	Property->Source.Min = Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = Property->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorBool(GUI::Template* Target, GUI::Checkbox* Checkbox, bool* Value, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Checkbox || !Value)
		return Changed;

	Checkbox->Source.Text = Name;
	Checkbox->SetValue(*Value);
	Target->Compose(Checkbox, [&](bool Active)
	{
		if (Active)
			*Value = Checkbox->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorOpt(GUI::Template* Target, GUI::Checkbox* Checkbox, uint32_t* Value, uint32_t Opt, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Checkbox || !Value)
		return Changed;

	Checkbox->Source.Text = Name;
	Checkbox->SetValue(*Value & Opt);
	Target->Compose(Checkbox, [&](bool Active)
	{
		if (Checkbox->GetValue())
			*Value |= Opt;
		else
			*Value &= ~Opt;

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorInt(GUI::Template* Target, GUI::Property* Property, int* Value, int Min, int Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = true;
	Property->SetValue((double)*Value);
	Property->Source.Max = (double)Max;
	Property->Source.Min = (double)Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = (int)Property->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorBigInt(GUI::Template* Target, GUI::Property* Property, uint64_t* Value, uint64_t Min, uint64_t Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = true;
	Property->SetValue((double)*Value);
	Property->Source.Max = (double)Max;
	Property->Source.Min = (double)Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = (uint64_t)Property->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorSBigInt(GUI::Template* Target, GUI::Property* Property, int64_t* Value, int64_t Min, int64_t Max, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Property || !Value)
		return Changed;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = true;
	Property->SetValue((double)*Value);
	Property->Source.Max = (double)Max;
	Property->Source.Min = (double)Min;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			*Value = (int64_t)Property->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static void ReactInspectorConst(GUI::Template* Target, GUI::Property* Property, int Value, const std::string& Name)
{
	if (!Target || !Property)
		return;

	Property->Source.Text = "#" + Name;
	Property->Source.Decimal = true;
	Property->SetValue((double)Value);
	Property->Source.Max = (double)Value;
	Property->Source.Min = (double)Value;
	Target->Compose(Property, nullptr);
}
static void ReactInspectorText(GUI::Template* Target, GUI::Edit* Edit, std::string* Value, const std::string& Name)
{
	if (!Target || !Edit || !Value)
		return;

	Edit->Source.Text = Name;
	Edit->SetValue(*Value);
	Target->Compose(Edit, [=](bool Active)
	{
		if (Active)
			*Value = Edit->GetValue();

		return Active;
	});
}
static bool ReactInspectorVector2(GUI::Template* Target, GUI::Vector* Vector, Vector2* Value, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Vector || !Value)
		return Changed;

	Vector->Source.Text = Name;
	Vector->Source.Count = 2;
	Vector->SetValue(*Value);
	Target->Compose(Vector, [&](bool Active)
	{
		if (Active)
			*Value = Vector->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorVector3(GUI::Template* Target, GUI::Vector* Vector, Vector3* Value, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Vector || !Value)
		return Changed;

	Vector->Source.Text = Name;
	Vector->Source.Count = 3;
	Vector->SetValue(*Value);
	Target->Compose(Vector, [&](bool Active)
	{
		if (Active)
			*Value = Vector->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static bool ReactInspectorVector4(GUI::Template* Target, GUI::Vector* Vector, Vector4* Value, const std::string& Name)
{
	bool Changed = false;
	if (!Target || !Vector || !Value)
		return Changed;

	Vector->Source.Text = Name;
	Vector->Source.Count = 4;
	Vector->SetValue(*Value);
	Target->Compose(Vector, [&](bool Active)
	{
		if (Active)
			*Value = Vector->GetValue();

		Changed = Active;
		return Active;
	});

	return Changed;
}
static void ReactInspectorDirection(GUI::Template* Target, GUI::Vector* Vector, Vector3* Value, const std::string& Name)
{
	if (!Target || !Vector || !Value)
		return;

	Vector->Source.Text = Name;
	Vector->Source.Count = 3;
	Vector->SetValue(*Value);
	Target->Compose(Vector, [&](bool Active)
	{
		if (Active)
			*Value = Vector->GetValue().NormalizeSafe();

		return Active;
	});
}
static void ReactInspectorColor(GUI::Template* Target, GUI::ColorPicker* ColorPicker, GUI::Layout* Layout, Vector3* Value)
{
	if (!Target || !ColorPicker || !Layout || !Value)
		return;

	float Height = Layout->Source.Height;
	Layout->Source.Height = (ColorPicker->GetWidgetWidth() * 0.8f) / Layout->GetAreaHeight();
	Target->Compose(Layout, nullptr);
	Layout->Source.Height = Height;

	ColorPicker->SetValue(Value->XYZW().SetW(1.0f));
	Target->Compose(ColorPicker, [=](bool Active)
	{
		if (Active)
			*Value = ColorPicker->GetValue().XYZ();

		return Active;
	});
	Target->Compose(Layout, nullptr);
}
static void ReactInspectorRotation(GUI::Template* Target, GUI::Property* Property, Vector3* Value, const std::string& Name)
{
	if (!Target || !Property || !Value)
		return;

	Property->Source.Text = "#" + Name + " X";
	Property->Source.Decimal = false;
	Property->SetValue((double)Value->X * Math<double>::Rad2Deg());
	Property->Source.Max = 360.0;
	Property->Source.Min = 0.0;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			Value->X = (float)Property->GetValue() * Math<float>::Deg2Rad();

		return Active;
	});

	Property->Source.Text = "#" + Name + " Y";
	Property->Source.Decimal = false;
	Property->SetValue((double)Value->Y * Math<double>::Rad2Deg());
	Property->Source.Max = 360.0;
	Property->Source.Min = 0.0;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			Value->Y = (float)Property->GetValue() * Math<float>::Deg2Rad();

		return Active;
	});

	Property->Source.Text = "#" + Name + " Z";
	Property->Source.Decimal = false;
	Property->SetValue((double)Value->Z * Math<double>::Rad2Deg());
	Property->Source.Max = 360.0;
	Property->Source.Min = 0.0;
	Target->Compose(Property, [&](bool Active)
	{
		if (Active)
			Value->Z = (float)Property->GetValue() * Math<float>::Deg2Rad();

		return Active;
	});
}
static void ReactInspectorTexture2D(GUI::Template* Target, Sandbox* App, Texture2D** Resource, const std::string& Name)
{
	if (!Target || !App || !Resource)
		return;

	GUI::Button* Button = Target->Get<GUI::Button>("button");
	if (!Button)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	if (App->GetResourceState(Name))
		Button->Source.Text = "Awaiting source...";
	else if (*Resource != nullptr)
		Button->Source.Text = "Unassign " + Name;
	else
		Button->Source.Text = "Assign " + Name;

	Target->Compose(Layout, nullptr);
	if (*Resource != nullptr)
	{
		GUI::Contextual* Contextual = Target->Get<GUI::Contextual>("contextual");
		if (Contextual != nullptr)
		{
			Contextual->Source.TriggerX = Button->GetWidgetPosition().X;
			Contextual->Source.TriggerY = Button->GetWidgetPosition().Y;
			Contextual->Source.TriggerWidth = Button->GetWidgetWidth();
			Contextual->Source.TriggerHeight = Button->GetWidgetHeight();
			Contextual->Source.Width = Contextual->Source.TriggerWidth * 0.5f;

			Target->Compose(Contextual, [=](bool Active)
			{
				if (!Active)
					return Active;

				GUI::Image* Image = Target->Get<GUI::Image>("image");
				if (Image != nullptr)
				{
					float Height = Layout->Source.Height;

					Layout->Source.Height = (Contextual->Source.Width * 0.9f) / Layout->GetAreaHeight();
					Target->Compose(Layout, nullptr);

					Image->Source.Image = *Resource;
					Target->Compose(Image, nullptr);

					Layout->Source.Height = Height;
				}

				return Active;
			});
		}
	}

	Target->Compose(Button, [=](bool Active)
	{
		if (!Active)
			return Active;

		if (!*Resource)
		{
			if (!App->GetResourceState(Name))
			{
				App->GetResource(Name, [=](const std::string& File)
				{
					*Resource = App->Content->Load<Texture2D>(File, nullptr);
				});
			}
			else
				App->GetResource("", nullptr);
		}
		else
			*Resource = nullptr;

		return Active;
	});
}
static void ReactInspectorAppearance(GUI::Template* Target, Sandbox* App, Appearance* Value)
{
	if (!Value)
		return;

	GUI::Button* Button = Target->Get<GUI::Button>("button");
	if (!Button)
		return;

	GUI::Combo* Combo = Target->Get<GUI::Combo>("combo");
	if (!Combo)
		return;

	GUI::ComboItem* ComboItem = Target->Get<GUI::ComboItem>("combo-item");
	if (!ComboItem)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	GUI::Vector* Vector = Target->Get<GUI::Vector>("vector");
	if (!Vector)
		return;

	GUI::Text* Text = Target->Get<GUI::Text>("text");
	if (!Text)
		return;

	GUI::ColorPicker* ColorPicker = Target->Get<GUI::ColorPicker>("color-picker");
	if (!ColorPicker)
		return;

	GUI::Property* Property = Target->Get<GUI::Property>("property");
	if (!Property)
		return;

	ReactInspectorText(Target, Text, "Diffuse color");
	ReactInspectorColor(Target, ColorPicker, Layout, &Value->Diffuse);
	ReactInspectorVector2(Target, Vector, &Value->TexCoord, "Texture offset");
	ReactInspectorTexture2D(Target, App, &Value->DiffuseMap, "diffuse map");
	ReactInspectorTexture2D(Target, App, &Value->NormalMap, "normal map");
	ReactInspectorTexture2D(Target, App, &Value->MetallicMap, "metallic map");
	ReactInspectorTexture2D(Target, App, &Value->RoughnessMap, "roughness map");
	ReactInspectorTexture2D(Target, App, &Value->HeightMap, "height map");
	ReactInspectorTexture2D(Target, App, &Value->OcclusionMap, "occlusion map");
	ReactInspectorTexture2D(Target, App, &Value->EmissionMap, "emission map");
	ReactInspectorFloat(Target, Property, &Value->HeightAmount, -1, 1, "Height amount");
	ReactInspectorFloat(Target, Property, &Value->HeightBias, -1, 1, "Height bias");

	if (Value->Material >= 0)
	{
		std::string Name = App->Scene->GetMaterialName(Value->Material);
		Combo->Source.Text = (Name.empty() ? "Unnamed" : Name) + " (" + std::to_string((int)Value->Material) + ')';
	}
	else
		Combo->Source.Text = "Empty material";

	Target->Compose(Combo, [=](bool Active)
	{
		if (!Active)
			return Active;

		ComboItem->SetLayer(Combo);
		Target->Compose(Layout, nullptr);

		ComboItem->Source.Text = "Empty material";
		Target->Compose(ComboItem, [=](bool Active)
		{
			if (Active)
				Value->Material = -1;

			return Active;
		});

		for (uint64_t i = 0; i < App->Scene->GetMaterialCount(); i++)
		{
			Material* Material = App->Scene->GetMaterialById(i);
			if (!Material)
				continue;

			std::string Name = App->Scene->GetMaterialName(Material->Id);
			ComboItem->Source.Text = (Name.empty() ? "Unnamed" : Name) + " (" + std::to_string((int)Material->Id) + ')';

			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Value->Material = i;

				return Active;
			});
		}

		return Active;
	});
}
static void ReactInspectorEffect(GUI::Template* Target, Sandbox* App, AudioEffect* Value)
{
	if (!Target || !App || !Value)
		return;

	GUI::Property* Property = Target->Get<GUI::Property>("property");
	if (!Property)
		return;

	GUI::Vector* Vector = Target->Get<GUI::Vector>("vector");
	if (!Vector)
		return;

	GUI::Checkbox* Checkbox = Target->Get<GUI::Checkbox>("checkbox");
	if (!Checkbox)
		return;

	GUI::Text* Text = Target->Get<GUI::Text>("text");
	if (!Text)
		return;

	GUI::Combo* Combo = Target->Get<GUI::Combo>("combo");
	if (!Combo)
		return;

	GUI::ComboItem* ComboItem = Target->Get<GUI::ComboItem>("combo-item");
	if (!ComboItem)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	if (Value->GetFilter() != nullptr)
	{
		auto* Base = Value->GetFilter();
		if (TH_COMPONENT_IS(Base, LowpassFilter))
		{
			auto* Filter = Base->As<Filters::LowpassFilter>();
			ReactInspectorFloat(Target, Property, &Filter->Gain, 0.0, 1.0, "Filter gain");
			ReactInspectorFloat(Target, Property, &Filter->GainHF, 0.0, 1.0, "Filter gain HF");
			Combo->Source.Text = "Lowpass filter";
		}
		else if (TH_COMPONENT_IS(Base, HighpassFilter))
		{
			auto* Filter = Base->As<Filters::HighpassFilter>();
			ReactInspectorFloat(Target, Property, &Filter->Gain, 0.0, 1.0, "Filter gain");
			ReactInspectorFloat(Target, Property, &Filter->GainLF, 0.0, 1.0, "Filter gain LF");
			Combo->Source.Text = "Highpass filter";
		}
		else if (TH_COMPONENT_IS(Base, BandpassFilter))
		{
			auto* Filter = Base->As<Filters::BandpassFilter>();
			ReactInspectorFloat(Target, Property, &Filter->Gain, 0.0, 1.0, "Filter gain");
			ReactInspectorFloat(Target, Property, &Filter->GainHF, 0.0, 1.0, "Filter gain HF");
			ReactInspectorFloat(Target, Property, &Filter->GainLF, 0.0, 1.0, "Filter gain LF");
			Combo->Source.Text = "Bandpass filter";
		}
	}
	else
		Combo->Source.Text = "Empty filter";

	Target->Compose(Combo, [=](bool Active)
	{
		if (!Active)
			return Active;

		ComboItem->SetLayer(Combo);
		Target->Compose(Layout, nullptr);

		ComboItem->Source.Text = "Empty filter";
		Target->Compose(ComboItem, [=](bool Active)
		{
			if (Active)
				Value->SetFilter(nullptr);

			return Active;
		});

		ComboItem->Source.Text = "Lowpass filter";
		Target->Compose(ComboItem, [=](bool Active)
		{
			if (Active)
			{
				AudioFilter* New = new Filters::LowpassFilter();
				Value->SetFilter(&New);
			}

			return Active;
		});

		ComboItem->Source.Text = "Bandpass filter";
		Target->Compose(ComboItem, [=](bool Active)
		{
			if (Active)
			{
				AudioFilter* New = new Filters::BandpassFilter();
				Value->SetFilter(&New);
			}

			return Active;
		});

		ComboItem->Source.Text = "Highpass filter";
		Target->Compose(ComboItem, [=](bool Active)
		{
			if (Active)
			{
				AudioFilter* New = new Filters::HighpassFilter();
				Value->SetFilter(&New);
			}

			return Active;
		});

		return Active;
	});

	if (TH_COMPONENT_IS(Value, ReverbEffect))
	{
		Effects::ReverbEffect* Base = Value->As<Effects::ReverbEffect>();
		ReactInspectorVector3(Target, Vector, &Base->LateReverbPan, "Late reverb pan");
		ReactInspectorVector3(Target, Vector, &Base->ReflectionsPan, "Reflections pan");
		ReactInspectorFloat(Target, Property, &Base->Density, 0.0, 1.0, "Density");
		ReactInspectorFloat(Target, Property, &Base->Diffusion, 0.0, 1.0, "Diffusion");
		ReactInspectorFloat(Target, Property, &Base->Gain, 0.0, 1.0, "Gain");
		ReactInspectorFloat(Target, Property, &Base->GainHF, 0.0, 1.0, "Gain HF");
		ReactInspectorFloat(Target, Property, &Base->GainLF, 0.0, 1.0, "Gain LF");
		ReactInspectorFloat(Target, Property, &Base->DecayTime, 0.1, 20.0, "Decay time");
		ReactInspectorFloat(Target, Property, &Base->DecayHFRatio, 0.1, 2.0, "Decay HF ratio");
		ReactInspectorFloat(Target, Property, &Base->DecayLFRatio, 0.1, 2.0, "Decay LF ratio");
		ReactInspectorFloat(Target, Property, &Base->ReflectionsGain, 0.0, 3.16, "Reflections gain");
		ReactInspectorFloat(Target, Property, &Base->ReflectionsDelay, 0.0, 0.3, "Reflections decay");
		ReactInspectorFloat(Target, Property, &Base->EchoTime, 0.075, 0.25, "Echo time");
		ReactInspectorFloat(Target, Property, &Base->EchoDepth, 0.0, 1.0, "Echo depth");
		ReactInspectorFloat(Target, Property, &Base->ModulationTime, 0.04, 4.0, "Modulation time");
		ReactInspectorFloat(Target, Property, &Base->ModulationDepth, 0.0, 1.0, "Modulation depth");
		ReactInspectorFloat(Target, Property, &Base->AirAbsorptionGainHF, 0.892, 1.0, "Air absorption gain HF");
		ReactInspectorFloat(Target, Property, &Base->HFReference, 1000.0, 20000.0, "HF reference");
		ReactInspectorFloat(Target, Property, &Base->LFReference, 20.0, 1000.0, "LF reference");
		ReactInspectorFloat(Target, Property, &Base->RoomRolloffFactor, 0.0, 10.0, "Room roll-off");
		ReactInspectorBool(Target, Checkbox, &Base->IsDecayHFLimited, "Is decay HF limited");
	}
	else if (TH_COMPONENT_IS(Value, ChorusEffect))
	{
		Effects::ChorusEffect* Base = Value->As<Effects::ChorusEffect>();
		ReactInspectorFloat(Target, Property, &Base->Rate, 0.0, 10.0, "Rate");
		ReactInspectorFloat(Target, Property, &Base->Depth, 0.0, 1.0, "Depth");
		ReactInspectorFloat(Target, Property, &Base->Feedback, -1.0, 1.0, "Feedback");
		ReactInspectorFloat(Target, Property, &Base->Delay, 0.0, 0.016, "Delay");
		ReactInspectorInt(Target, Property, &Base->Waveform, 0, 1, "Waveform");
		ReactInspectorInt(Target, Property, &Base->Phase, -180, 180, "Phase");
	}
	else if (TH_COMPONENT_IS(Value, DistortionEffect))
	{
		Effects::DistortionEffect* Base = Value->As<Effects::DistortionEffect>();
		ReactInspectorFloat(Target, Property, &Base->Edge, 0.0, 1.0, "Edge");
		ReactInspectorFloat(Target, Property, &Base->Gain, 0.0, 1.0, "Gain");
		ReactInspectorFloat(Target, Property, &Base->LowpassCutOff, 80.0, 24000.0, "Lowpass cut-off");
		ReactInspectorFloat(Target, Property, &Base->EQCenter, 80.0, 24000.0, "EQ center");
		ReactInspectorFloat(Target, Property, &Base->EQBandwidth, 80.0, 24000.0, "EQ bandwidth");
	}
	else if (TH_COMPONENT_IS(Value, EchoEffect))
	{
		Effects::EchoEffect* Base = Value->As<Effects::EchoEffect>();
		ReactInspectorFloat(Target, Property, &Base->Delay, 0.0, 0.207, "Delay");
		ReactInspectorFloat(Target, Property, &Base->LRDelay, 0.0, 0.404, "LR Delay");
		ReactInspectorFloat(Target, Property, &Base->Damping, 0.0, 0.99, "Damping");
		ReactInspectorFloat(Target, Property, &Base->Feedback, 0.0, 1.0, "Feedback");
		ReactInspectorFloat(Target, Property, &Base->Spread, -1.0, 1.0, "Spread");
	}
	else if (TH_COMPONENT_IS(Value, FlangerEffect))
	{
		Effects::FlangerEffect* Base = Value->As<Effects::FlangerEffect>();
		ReactInspectorFloat(Target, Property, &Base->Rate, 0.0, 10.0, "Rate");
		ReactInspectorFloat(Target, Property, &Base->Depth, 0.0, 1.0, "Depth");
		ReactInspectorFloat(Target, Property, &Base->Feedback, -1.0, 1.0, "Feedback");
		ReactInspectorFloat(Target, Property, &Base->Delay, 0.0, 0.004, "Delay");
		ReactInspectorInt(Target, Property, &Base->Waveform, 0, 1, "Waveform");
		ReactInspectorInt(Target, Property, &Base->Phase, -180, 180, "Phase");
	}
	else if (TH_COMPONENT_IS(Value, FrequencyShifterEffect))
	{
		Effects::FrequencyShifterEffect* Base = Value->As<Effects::FrequencyShifterEffect>();
		ReactInspectorFloat(Target, Property, &Base->Frequency, 0.0, 24000.0, "Frequency");
		ReactInspectorInt(Target, Property, &Base->LeftDirection, 0, 2, "Left direction");
		ReactInspectorInt(Target, Property, &Base->RightDirection, 0, 2, "Right direction");
	}
	else if (TH_COMPONENT_IS(Value, VocalMorpherEffect))
	{
		Effects::VocalMorpherEffect* Base = Value->As<Effects::VocalMorpherEffect>();
		ReactInspectorFloat(Target, Property, &Base->Rate, 0.0, 10.0, "Rate");
		ReactInspectorInt(Target, Property, &Base->Phonemea, 0, 29, "Phonemea");
		ReactInspectorInt(Target, Property, &Base->PhonemeaCoarseTuning, -24, 24, "Phonemea coarse");
		ReactInspectorInt(Target, Property, &Base->Phonemeb, 0, 29, "Phonemeb");
		ReactInspectorInt(Target, Property, &Base->PhonemebCoarseTuning, -24, 24, "Phonemeb coarse");
		ReactInspectorInt(Target, Property, &Base->Waveform, 0, 2, "Waveform");
	}
	else if (TH_COMPONENT_IS(Value, PitchShifterEffect))
	{
		Effects::PitchShifterEffect* Base = Value->As<Effects::PitchShifterEffect>();
		ReactInspectorInt(Target, Property, &Base->CoarseTune, -12, 12, "Coarse tune");
		ReactInspectorInt(Target, Property, &Base->FineTune, -50, 50, "Fine tune");
	}
	else if (TH_COMPONENT_IS(Value, RingModulatorEffect))
	{
		Effects::RingModulatorEffect* Base = Value->As<Effects::RingModulatorEffect>();
		ReactInspectorFloat(Target, Property, &Base->Frequency, 0.0, 8000.0, "Frequency");
		ReactInspectorFloat(Target, Property, &Base->HighpassCutOff, 0.0, 24000.0, "Highpass cut-off");
		ReactInspectorInt(Target, Property, &Base->Waveform, 0, 2, "Waveform");
	}
	else if (TH_COMPONENT_IS(Value, AutowahEffect))
	{
		Effects::AutowahEffect* Base = Value->As<Effects::AutowahEffect>();
		ReactInspectorFloat(Target, Property, &Base->AttackTime, 0.0001, 1.0, "Attack time");
		ReactInspectorFloat(Target, Property, &Base->ReleaseTime, 0.0001, 1.0, "Release time");
		ReactInspectorFloat(Target, Property, &Base->Resonance, 2.0, 1000.0, "Resonance");
		ReactInspectorFloat(Target, Property, &Base->PeakGain, 0.00003, 31621.0, "Peak gain");
	}
	else if (TH_COMPONENT_IS(Value, CompressorEffect))
	{
		Effects::CompressorEffect* Base = Value->As<Effects::CompressorEffect>();
		ReactInspectorText(Target, Text, "There are no parameters");
	}
	else if (TH_COMPONENT_IS(Value, EqualizerEffect))
	{
		Effects::EqualizerEffect* Base = Value->As<Effects::EqualizerEffect>();
		ReactInspectorFloat(Target, Property, &Base->LowCutOff, 0.126, 7.943, "Low gain");
		ReactInspectorFloat(Target, Property, &Base->LowCutOff, 50.0, 800.0, "Low cut-off");
		ReactInspectorFloat(Target, Property, &Base->Mid1Gain, 0.126, 7.943, "Mid1 gain");
		ReactInspectorFloat(Target, Property, &Base->Mid1Center, 200.0, 3000.0, "Mid1 center");
		ReactInspectorFloat(Target, Property, &Base->Mid1Width, 0.01, 1.0, "Mid1 width");
		ReactInspectorFloat(Target, Property, &Base->Mid2Center, 1000.0, 8000.0, "Mid2 center");
		ReactInspectorFloat(Target, Property, &Base->Mid2Width, 0.01, 1.0, "Mid2 width");
		ReactInspectorFloat(Target, Property, &Base->HighGain, 0.126, 7.943, "High gain");
		ReactInspectorFloat(Target, Property, &Base->HighCutOff, 4000.0, 16000.0, "High cut-off");
	}
}
static void ReactInspectorKeyCode(GUI::Template* Target, Sandbox* App, KeyMap* Value, const std::string& Text)
{
	if (!Target || !App || !Value)
		return;

	GUI::Button* Button = Target->Get<GUI::Button>("button");
	if (!Button)
		return;

	Value->Mod = KeyMod_NONE;
	if (!Value->Normal)
	{
		const char* Name = Activity::GetKeyName(Value->Key);
		if (ReactInspectorButton(Target, Button, Text + " key: " + (Name ? Name : "none")))
			Value->Normal = true;
	}
	else
	{
		Button->GetContext()->SetOverflow(true);
		if (!Button->IsWidgetHovered())
		{
			if (App->Activity->CaptureKeyMap(Value))
				Value->Normal = false;
		}
		else if (App->Activity->IsKeyDownHit(KeyCode_CURSORRIGHT))
		{
			App->Activity->CaptureKeyMap(nullptr);
			Value->Normal = false;
			Value->Key = KeyCode_NONE;
		}

		if (ReactInspectorButton(Target, Button, "Waiting for input..."))
		{
			App->Activity->CaptureKeyMap(nullptr);
			Value->Normal = false;
		}
	}
}
static void ReactInspectorJoint(GUI::Template* Target, Sandbox* App, PoseBuffer* Buffer, Joint* Value)
{
	if (!Target || !App || !Buffer || !Value)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	GUI::Tree* Tree = Target->Get<GUI::Tree>("tree");
	if (!Tree)
		return;

	GUI::Property* Property = Target->Get<GUI::Property>("property");
	if (!Property)
		return;

	GUI::Vector* Vector = Target->Get<GUI::Vector>("vector");
	if (!Vector)
		return;

	Target->ComposeStateful(Tree, [=]()
	{
		Tree->Source.Text = Value->Name + " (" + std::to_string(Value->Index) + ")";
	}, [=](bool Active)
	{
		if (!Active)
			return Active;

		Target->Compose(Layout, nullptr);

		auto Offset = Buffer->Pose.find(Value->Index);
		if (Offset != Buffer->Pose.end())
		{
			ReactInspectorVector3(Target, Vector, &Offset->second.Position, "Position");
			ReactInspectorRotation(Target, Property, &Offset->second.Rotation, "Rotation");
		}

		for (auto& Joint : Value->Childs)
			ReactInspectorJoint(Target, App, Buffer, &Joint);

		return Active;
	});
}
static void ReactInspectorComponent(GUI::Template* Target, Sandbox* App, Component* Ref)
{
	GUI::Button* Button = Target->Get<GUI::Button>("button");
	if (!Button)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	GUI::Tree* Tree = Target->Get<GUI::Tree>("tree");
	if (!Tree)
		return;

	GUI::Property* Property = Target->Get<GUI::Property>("property");
	if (!Property)
		return;

	GUI::Vector* Vector = Target->Get<GUI::Vector>("vector");
	if (!Vector)
		return;

	GUI::Checkbox* Checkbox = Target->Get<GUI::Checkbox>("checkbox");
	if (!Checkbox)
		return;

	GUI::ColorPicker* ColorPicker = Target->Get<GUI::ColorPicker>("color-picker");
	if (!ColorPicker)
		return;

	GUI::Edit* Edit = Target->Get<GUI::Edit>("edit");
	if (!Edit)
		return;

	GUI::Combo* Combo = Target->Get<GUI::Combo>("combo");
	if (!Combo)
		return;

	GUI::ComboItem* ComboItem = Target->Get<GUI::ComboItem>("combo-item");
	if (!ComboItem)
		return;

	GUI::Contextual* Contextual = Target->Get<GUI::Contextual>("contextual");
	if (!Contextual)
		return;

	GUI::Text* Text = Target->Get<GUI::Text>("text");
	if (!Text)
		return;

	if (TH_COMPONENT_IS(Ref, SliderConstraint))
	{
		auto* Base = Ref->As<Components::SliderConstraint>();
		static bool IsGhosted = true;
		static bool IsLinear = true;

		Target->Compose(Layout, nullptr);
		if (Base->GetConstraint() != nullptr)
		{
			float Value = Base->GetConstraint()->GetAngularMotorVelocity();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular motor velocity"))
				Base->GetConstraint()->SetAngularMotorVelocity(Value);

			Value = Base->GetConstraint()->GetLinearMotorVelocity();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear motor velocity"))
				Base->GetConstraint()->SetLinearMotorVelocity(Value);

			Value = Base->GetConstraint()->GetUpperLinearLimit();
			if (ReactInspectorFloat(Target, Property, &Value, Base->GetConstraint()->GetLowerLinearLimit(), 100, "Upper linear limit"))
				Base->GetConstraint()->SetUpperLinearLimit(Value);

			Value = Base->GetConstraint()->GetLowerLinearLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, Base->GetConstraint()->GetUpperLinearLimit(), "Lower linear limit"))
				Base->GetConstraint()->SetLowerLinearLimit(Value);

			Value = Base->GetConstraint()->GetAngularDampingDirection();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular damping direction"))
				Base->GetConstraint()->SetAngularDampingDirection(Value);

			Value = Base->GetConstraint()->GetLinearDampingDirection();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear damping direction"))
				Base->GetConstraint()->SetLinearDampingDirection(Value);

			Value = Base->GetConstraint()->GetAngularDampingLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular damping limit"))
				Base->GetConstraint()->SetAngularDampingLimit(Value);

			Value = Base->GetConstraint()->GetLinearDampingLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear damping limit"))
				Base->GetConstraint()->SetLinearDampingLimit(Value);

			Value = Base->GetConstraint()->GetAngularDampingOrtho();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular damping ortho"))
				Base->GetConstraint()->SetAngularDampingOrtho(Value);

			Value = Base->GetConstraint()->GetLinearDampingOrtho();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear damping ortho"))
				Base->GetConstraint()->SetLinearDampingOrtho(Value);

			Value = Base->GetConstraint()->GetUpperAngularLimit();
			if (ReactInspectorFloat(Target, Property, &Value, Base->GetConstraint()->GetLowerAngularLimit(), 100, "Upper angular limit"))
				Base->GetConstraint()->SetUpperAngularLimit(Value);

			Value = Base->GetConstraint()->GetLowerAngularLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, Base->GetConstraint()->GetUpperAngularLimit(), "Lower angular limit"))
				Base->GetConstraint()->SetLowerAngularLimit(Value);

			Value = Base->GetConstraint()->GetMaxAngularMotorForce();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Max angular motor force"))
				Base->GetConstraint()->SetMaxAngularMotorForce(Value);

			Value = Base->GetConstraint()->GetMaxLinearMotorForce();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Max linear motor force"))
				Base->GetConstraint()->SetMaxLinearMotorForce(Value);

			Value = Base->GetConstraint()->GetAngularRestitutionDirection();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Angular restitution direction"))
				Base->GetConstraint()->SetAngularRestitutionDirection(Value);

			Value = Base->GetConstraint()->GetLinearRestitutionDirection();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Linear restitution direction"))
				Base->GetConstraint()->SetLinearRestitutionDirection(Value);

			Value = Base->GetConstraint()->GetAngularSoftnessDirection();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular softness direction"))
				Base->GetConstraint()->SetAngularSoftnessDirection(Value);

			Value = Base->GetConstraint()->GetLinearSoftnessDirection();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear softness direction"))
				Base->GetConstraint()->SetLinearSoftnessDirection(Value);

			Value = Base->GetConstraint()->GetAngularSoftnessLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular softness limit"))
				Base->GetConstraint()->SetAngularSoftnessLimit(Value);

			Value = Base->GetConstraint()->GetLinearSoftnessLimit();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear softness limit"))
				Base->GetConstraint()->SetLinearSoftnessLimit(Value);

			Value = Base->GetConstraint()->GetAngularSoftnessOrtho();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Angular softness ortho"))
				Base->GetConstraint()->SetAngularSoftnessOrtho(Value);

			Value = Base->GetConstraint()->GetLinearSoftnessOrtho();
			if (ReactInspectorFloat(Target, Property, &Value, -100, 100, "Linear softness ortho"))
				Base->GetConstraint()->SetLinearSoftnessOrtho(Value);

			bool Option = Base->GetConstraint()->GetPoweredAngularMotor();
			if (ReactInspectorBool(Target, Checkbox, &Option, "Is angular-powered motor"))
				Base->GetConstraint()->SetPoweredAngularMotor(Option);

			Option = Base->GetConstraint()->GetPoweredLinearMotor();
			if (ReactInspectorBool(Target, Checkbox, &Option, "Is linear-powered motor"))
				Base->GetConstraint()->SetPoweredLinearMotor(Option);

			Option = Base->GetConstraint()->IsEnabled();
			if (ReactInspectorBool(Target, Checkbox, &Option, "Is enabled"))
				Base->GetConstraint()->SetEnabled(Option);
		}
		else
		{
			ReactInspectorBool(Target, Checkbox, &IsGhosted, "Is ghosted");
			ReactInspectorBool(Target, Checkbox, &IsLinear, "Is linear");
		}

		if (Base->GetConstraint() != nullptr)
			Button->Source.Text = "Unassign entity";
		else if (App->GetEntityState("rigid body"))
			Button->Source.Text = "Awaiting entity...";
		else
			Button->Source.Text = "Assign entity";

		Target->Compose(Layout, nullptr);
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetConstraint())
			{
				if (!App->GetEntityState("rigid body"))
				{
					App->GetEntity("rigid body", [=](Entity* Result)
					{
						Base->Initialize(Result, IsGhosted, IsLinear);
					});
				}
				else
					App->GetEntity("", nullptr);
			}
			else
				Base->Clear();

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, RigidBody))
	{
		auto* Base = Ref->As<Components::RigidBody>();
		Target->Compose(Layout, nullptr);

		if (Base->GetBody() != nullptr)
		{
			Vector3 Offset = Base->GetBody()->GetLinearVelocity();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Velocity"))
				Base->GetBody()->SetLinearVelocity(Offset);

			Offset = Base->GetBody()->GetAngularVelocity();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Torque"))
				Base->GetBody()->SetAngularVelocity(Offset);

			Offset = Base->GetBody()->GetGravity();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Gravity"))
				Base->GetBody()->SetGravity(Offset);

			Offset = Base->GetBody()->GetAngularFactor();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Angular factor"))
				Base->GetBody()->SetAngularFactor(Offset);

			Offset = Base->GetBody()->GetAnisotropicFriction();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Anisotropic friction"))
				Base->GetBody()->SetAnisotropicFriction(Offset);

			Offset = Base->GetBody()->GetLinearFactor();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Linear factor"))
				Base->GetBody()->SetLinearFactor(Offset);

			float Value = Base->GetBody()->GetMass();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1000, "Mass"))
				Base->GetBody()->SetMass(Value);

			Value = Base->GetBody()->GetAngularDamping();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Angular damping"))
				Base->GetBody()->SetAngularDamping(Value);

			Value = Base->GetBody()->GetLinearDamping();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Linear damping"))
				Base->GetBody()->SetLinearDamping(Value);

			Value = Base->GetBody()->GetRestitution();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Restitution"))
				Base->GetBody()->SetRestitution(Value);

			Value = Base->GetBody()->GetSpinningFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Spinning friction"))
				Base->GetBody()->SetSpinningFriction(Value);

			Value = Base->GetBody()->GetContactDamping();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 9999, "Contact damping"))
				Base->GetBody()->SetContactDamping(Value);

			Value = Base->GetBody()->GetContactStiffness() / 1000000000000.0f;
			if (ReactInspectorFloat(Target, Property, &Value, 1, 9999999, "Contact stiffness"))
				Base->GetBody()->SetContactStiffness(Value * 1000000000000.0f);

			Value = Base->GetBody()->GetFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Friction"))
				Base->GetBody()->SetFriction(Value);

			Value = Base->GetBody()->GetHitFraction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Hit fraction"))
				Base->GetBody()->SetHitFraction(Value);

			Value = Base->GetBody()->GetAngularSleepingThreshold();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Angular threshold"))
				Base->GetBody()->SetAngularSleepingThreshold(Value);

			Value = Base->GetBody()->GetLinearSleepingThreshold();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Linear threshold"))
				Base->GetBody()->SetLinearSleepingThreshold(Value);

			Value = Base->GetBody()->GetCcdMotionThreshold();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Motion threshold"))
				Base->GetBody()->SetCcdMotionThreshold(Value);

			Value = Base->GetBody()->GetCcdSweptSphereRadius();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Swept sphere radius"))
				Base->GetBody()->SetCcdSweptSphereRadius(Value);

			Value = Base->GetBody()->GetDeactivationTime();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Deactivation time"))
				Base->GetBody()->SetDeactivationTime(Value);

			Value = Base->GetBody()->GetRollingFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Rolling friction"))
				Base->GetBody()->SetRollingFriction(Value);

			bool Option = (Base->GetBody()->GetActivationState() != MotionState_Disable_Deactivation);
			if (ReactInspectorBool(Target, Checkbox, &Option, "Can be deactivated"))
			{
				if (Option)
					Base->GetBody()->SetActivationState(MotionState_Active);
				else
					Base->GetBody()->SetActivationState(MotionState_Disable_Deactivation);
			}
			ReactInspectorBool(Target, Checkbox, &Base->Kinematic, "Kinematic");

			if (ReactInspectorButton(Target, Button, Base->GetBody()->IsActive() ? "Deactivate" : "Activate"))
			{
				if (Base->GetBody()->IsActive())
					Base->GetBody()->SetActivity(false);
				else
					Base->GetBody()->SetActivity(true);
			}

			if (ReactInspectorButton(Target, Button, Base->GetBody()->IsGhost() ? "Ghost" : "Normal"))
			{
				if (Base->GetBody()->IsGhost())
					Base->GetBody()->SetAsNormal();
				else
					Base->GetBody()->SetAsGhost();
			}

			int Type = Base->GetBody()->GetCollisionShapeType();
			if (Type == Shape_Box)
				Combo->Source.Text = "Box shape";
			else if (Type == Shape_Sphere)
				Combo->Source.Text = "Sphere shape";
			else if (Type == Shape_Capsule)
				Combo->Source.Text = "Capsule shape";
			else if (Type == Shape_Cone)
				Combo->Source.Text = "Cone shape";
			else if (Type == Shape_Cylinder)
				Combo->Source.Text = "Cylinder shape";
			else
				Combo->Source.Text = "Other shape";
		}
		else
			Combo->Source.Text = "Empty shape";

		Target->Compose(Combo, [=](bool Active)
		{
			if (!Active)
				return Active;

			ComboItem->SetLayer(Combo);
			Target->Compose(Layout, nullptr);

			ComboItem->Source.Text = "Empty shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Clear();

				return Active;
			});

			ComboItem->Source.Text = "Box shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Initialize(App->Scene->GetSimulator()->CreateCube(), 0, 0);

				return Active;
			});

			ComboItem->Source.Text = "Sphere shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Initialize(App->Scene->GetSimulator()->CreateSphere(), 0, 0);

				return Active;
			});

			ComboItem->Source.Text = "Capsule shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Initialize(App->Scene->GetSimulator()->CreateCapsule(), 0, 0);

				return Active;
			});

			ComboItem->Source.Text = "Cone shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Initialize(App->Scene->GetSimulator()->CreateCone(), 0, 0);

				return Active;
			});

			ComboItem->Source.Text = "Cylinder shape";
			Target->Compose(ComboItem, [=](bool Active)
			{
				if (Active)
					Base->Initialize(App->Scene->GetSimulator()->CreateCylinder(), 0, 0);

				return Active;
			});

			return Active;
		});

		if (Base->GetBody() != nullptr)
			Button->Source.Text = "Unassign shape";
		else if (App->GetResourceState("shape"))
			Button->Source.Text = "Awaiting shape...";
		else
			Button->Source.Text = "Assign shape";

		Target->Compose(Layout, nullptr);
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetBody())
			{
				if (!App->GetResourceState("shape"))
				{
					App->GetResource("shape", [=](const std::string& File)
					{
						Base->Initialize(App->Content, File, 0, 0);
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->Clear();

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, SoftBody))
	{
		auto* Base = Ref->As<Components::SoftBody>();
		Target->Compose(Layout, nullptr);

		if (Base->GetBody() != nullptr)
		{
			SoftBody::Desc& Desc = Base->GetBody()->GetInitialState();
			ReactInspectorFloat(Target, Property, &Desc.Config.VCF, 0, 10, "VCF");
			ReactInspectorFloat(Target, Property, &Desc.Config.DP, 0, 1, "DP");
			ReactInspectorFloat(Target, Property, &Desc.Config.DG, 0, 10, "DG");
			ReactInspectorFloat(Target, Property, &Desc.Config.LF, 0, 10, "LF");
			ReactInspectorFloat(Target, Property, &Desc.Config.PR, -10, 10, "PR");
			ReactInspectorFloat(Target, Property, &Desc.Config.VC, 0, 10, "VC");
			ReactInspectorFloat(Target, Property, &Desc.Config.DF, 0, 1, "DF");
			ReactInspectorFloat(Target, Property, &Desc.Config.MT, 0, 1, "MT");
			ReactInspectorFloat(Target, Property, &Desc.Config.CHR, 0, 1, "CHR");
			ReactInspectorFloat(Target, Property, &Desc.Config.KHR, 0, 1, "KHR");
			ReactInspectorFloat(Target, Property, &Desc.Config.SHR, 0, 1, "SHR");
			ReactInspectorFloat(Target, Property, &Desc.Config.AHR, 0, 1, "AHR");
			ReactInspectorFloat(Target, Property, &Desc.Config.SRHR_CL, 0, 1, "SRHR-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.SKHR_CL, 0, 1, "SKHR-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.SSHR_CL, 0, 1, "SSHR-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.SR_SPLT_CL, 0, 1, "SR-SPLT-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.SK_SPLT_CL, 0, 1, "SK-SPLT-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.SS_SPLT_CL, 0, 1, "SS-SPLT-CL");
			ReactInspectorFloat(Target, Property, &Desc.Config.MaxVolume, 0, 10, "Max volume");
			ReactInspectorFloat(Target, Property, &Desc.Config.TimeScale, 0, 10, "Time scale");
			ReactInspectorFloat(Target, Property, &Desc.Config.Drag, 0, 1, "Drag");
			ReactInspectorFloat(Target, Property, &Desc.Config.MaxStress, 0, 10, "Max stress");
			ReactInspectorInt(Target, Property, &Desc.Config.VIterations, 0, 100, "V iterations");
			ReactInspectorInt(Target, Property, &Desc.Config.PIterations, 0, 100, "P iterations");
			ReactInspectorInt(Target, Property, &Desc.Config.DIterations, 0, 100, "D iterations");
			ReactInspectorInt(Target, Property, &Desc.Config.CIterations, 0, 100, "C iterations");

			Vector3 Offset = Base->GetBody()->GetAnisotropicFriction();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Anisotropic friction"))
				Base->GetBody()->SetAnisotropicFriction(Offset);

			Offset = Base->GetBody()->GetWindVelocity();
			if (ReactInspectorVector3(Target, Vector, &Offset, "Wind velocity"))
				Base->GetBody()->SetWindVelocity(Offset);

			float Value = Base->GetBody()->GetTotalMass();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1000, "Mass"))
				Base->GetBody()->SetTotalMass(Value);

			Value = Base->GetBody()->GetRestitution();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 1, "Restitution"))
				Base->GetBody()->SetRestitution(Value);

			Value = Base->GetBody()->GetSpinningFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Spinning friction"))
				Base->GetBody()->SetSpinningFriction(Value);

			Value = Base->GetBody()->GetContactDamping();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 9999, "Contact damping"))
				Base->GetBody()->SetContactDamping(Value);

			Value = Base->GetBody()->GetContactStiffness() / 1000000000000.0f;
			if (ReactInspectorFloat(Target, Property, &Value, 1, 9999999, "Contact stiffness"))
				Base->GetBody()->SetContactStiffness(Value * 1000000000000.0f);

			Value = Base->GetBody()->GetFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Friction"))
				Base->GetBody()->SetFriction(Value);

			Value = Base->GetBody()->GetHitFraction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Hit fraction"))
				Base->GetBody()->SetHitFraction(Value);

			Value = Base->GetBody()->GetCcdMotionThreshold();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Motion threshold"))
				Base->GetBody()->SetCcdMotionThreshold(Value);

			Value = Base->GetBody()->GetCcdSweptSphereRadius();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Swept sphere radius"))
				Base->GetBody()->SetCcdSweptSphereRadius(Value);

			Value = Base->GetBody()->GetDeactivationTime();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Deactivation time"))
				Base->GetBody()->SetDeactivationTime(Value);

			Value = Base->GetBody()->GetRollingFriction();
			if (ReactInspectorFloat(Target, Property, &Value, 0, 10, "Rolling friction"))
				Base->GetBody()->SetRollingFriction(Value);

			bool Option = (Base->GetBody()->GetActivationState() != MotionState_Disable_Deactivation);
			if (ReactInspectorBool(Target, Checkbox, &Option, "Can be deactivated"))
			{
				if (Option)
					Base->GetBody()->SetActivationState(MotionState_Active);
				else
					Base->GetBody()->SetActivationState(MotionState_Disable_Deactivation);
			}
			ReactInspectorBool(Target, Checkbox, &Base->Kinematic, "Kinematic");

			if (ReactInspectorButton(Target, Button, Base->GetBody()->IsActive() ? "Deactivate" : "Activate"))
			{
				if (Base->GetBody()->IsActive())
					Base->GetBody()->SetActivity(false);
				else
					Base->GetBody()->SetActivity(true);
			}

			if (ReactInspectorButton(Target, Button, Base->GetBody()->IsGhost() ? "Ghost" : "Normal"))
			{
				if (Base->GetBody()->IsGhost())
					Base->GetBody()->SetAsNormal();
				else
					Base->GetBody()->SetAsGhost();
			}
		}

		if (Base->GetBody() != nullptr)
			Button->Source.Text = "Unassign shape";
		else if (App->GetResourceState("soft shape"))
			Button->Source.Text = "Awaiting shape...";
		else
			Button->Source.Text = "Assign shape";

		Target->Compose(Layout, nullptr);
		ReactInspectorBool(Target, Checkbox, &Base->Static, "Is static");
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetBody())
			{
				if (!App->GetResourceState("soft shape"))
				{
					App->GetResource("soft shape", [=](const std::string& File)
					{
						Base->InitializeShape(App->Content, File, 0);
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->Clear();

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, AudioSource))
	{
		auto* Base = Ref->As<Components::AudioSource>();
		if (Base->GetSource()->GetClip() != nullptr)
		{
			auto* Effects = Base->GetSource()->GetEffects();
			for (size_t i = 0; i < Effects->size(); i++)
			{
				auto* Effect = Effects->at(i);
				Target->ComposeStateful(Tree, [=]()
				{
					Tree->Source.Text = App->GetPascal(Effect->GetName());
				}, [=](bool Active)
				{
					if (Active)
					{
						Target->Compose(Layout, nullptr);
						ReactInspectorEffect(Target, App, Effect);

						if (ReactInspectorButton(Target, Button, "Remove effect"))
							Base->GetSource()->RemoveEffect(i);
					}

					return Active;
				});
			}

			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Audio properties";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				Target->Compose(Layout, nullptr);
				ReactInspectorDirection(Target, Vector, &Base->GetSync().Direction, "Direction");
				ReactInspectorFloat(Target, Property, &Base->GetSync().ConeInnerAngle, 0, 360, "Cone inner angle");
				ReactInspectorFloat(Target, Property, &Base->GetSync().ConeOuterAngle, 0, 360, "Cone outer angle");
				ReactInspectorFloat(Target, Property, &Base->GetSync().ConeOuterGain, 0, 1, "Cone outer gain");
				ReactInspectorFloat(Target, Property, &Base->GetSync().AirAbsorption, 0, 10, "Air absorption");
				ReactInspectorFloat(Target, Property, &Base->GetSync().RoomRollOff, 0, 10, "Room roll-off");
				ReactInspectorFloat(Target, Property, &Base->GetSync().Rolloff, 0, 1000, "Roll-off");
				ReactInspectorFloat(Target, Property, &Base->GetSync().Distance, 0, 1000, "Distance");
				ReactInspectorFloat(Target, Property, &Base->GetSync().RefDistance, 0, 1000, "Ref-distance");
				ReactInspectorBool(Target, Checkbox, &Base->GetSync().IsLooped, "Is audio looped");

				if (Base->GetSource()->GetClip()->IsMono())
					ReactInspectorBool(Target, Checkbox, &Base->GetSync().IsRelative, "Is audio surrounded (3d)");

				return Active;
			});

			Target->Compose(Layout, nullptr);
			ReactInspectorFloat(Target, Property, &Base->GetSync().Gain, 0, 5, "Gain");
			ReactInspectorFloat(Target, Property, &Base->GetSync().Pitch, 0, 10, "Pitch");
			ReactInspectorFloat(Target, Property, &Base->GetSync().Position, 0, Base->GetSource()->GetClip()->Length(), "Playback (sec.)");
			ReactInspectorConst(Target, Property, Base->GetSource()->GetClip()->GetBuffer(), "Buffer");
			ReactInspectorConst(Target, Property, Base->GetSource()->GetClip()->GetFormat(), "Format");
			ReactInspectorConst(Target, Property, Base->GetSource()->GetClip()->Length(), "Length");
			ReactInspectorConst(Target, Property, Base->GetSource()->GetClip()->IsMono(), "Mono");

			if (ReactInspectorButton(Target, Button, "Play audio"))
				Base->GetSource()->Play();

			if (ReactInspectorButton(Target, Button, "Pause audio"))
				Base->GetSource()->Pause();

			if (ReactInspectorButton(Target, Button, "Stop audio"))
				Base->GetSource()->Stop();

			Button->Source.Text = "Unassign source";
		}
		else if (App->GetResourceState("audio clip"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Layout, nullptr);
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetSource()->GetClip())
			{
				if (!App->GetResourceState("audio clip"))
				{
					App->GetResource("audio clip", [=](const std::string& File)
					{
						Base->GetSource()->SetClip(App->Content->Load<AudioClip>(File, nullptr));
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->GetSource()->SetClip(nullptr);

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, AudioListener))
	{
		auto* Base = Ref->As<Components::AudioListener>();
		Target->Compose(Layout, nullptr);

		ReactInspectorFloat(Target, Property, &Base->Gain, 0, 5, "Gain");
	}
	else if (TH_COMPONENT_IS(Ref, Acceleration))
	{
		auto* Base = Ref->As<Components::Acceleration>();
		Target->Compose(Layout, nullptr);

		ReactInspectorVector3(Target, Vector, &Base->AmplitudeVelocity, "Amplitude velocity");
		ReactInspectorVector3(Target, Vector, &Base->AmplitudeTorque, "Amplitude torque");
		ReactInspectorVector3(Target, Vector, &Base->ConstantVelocity, "Velocity");
		ReactInspectorVector3(Target, Vector, &Base->ConstantTorque, "Torque");
		ReactInspectorVector3(Target, Vector, &Base->ConstantCenter, "Center");
		ReactInspectorBool(Target, Checkbox, &Base->Velocity, "Velocity");
	}
	else if (TH_COMPONENT_IS(Ref, KeyAnimator))
	{
		auto* Base = Ref->As<Components::KeyAnimator>();
		Target->ComposeStateful(Tree, [=]()
		{
			Tree->Source.Text = "Animation source";
		}, [=](bool Active)
		{
			if (!Active)
				return Active;

			for (size_t i = 0; i < Base->Clips.size(); i++)
			{
				auto* Clip = &Base->Clips[i];
				Target->ComposeStateful(Tree, [=]()
				{
					Tree->Source.Text = Clip->Name + " (" + std::to_string(i) + ")";
				}, [=](bool Active)
				{
					if (!Active)
						return Active;

					Target->ComposeStateful(Tree, [=]()
					{
						Tree->Source.Text = "Properties";
					}, [=](bool Active)
					{
						if (!Active)
							return Active;

						Target->Compose(Layout, nullptr);
						ReactInspectorText(Target, Edit, &Clip->Name, "Clip name");
						ReactInspectorFloat(Target, Property, &Clip->Duration, 0.0, 128, "Duration");
						ReactInspectorFloat(Target, Property, &Clip->Rate, 0.0, 32, "Rate");

						if (ReactInspectorButton(Target, Button, "Normalize frames"))
						{
							for (size_t i = 0; i < Clip->Keys.size(); i++)
								Clip->Keys[i].Rotation = Clip->Keys[i].Rotation.SaturateRotation();
						}

						if (ReactInspectorButton(Target, Button, "Remove all frames"))
							Clip->Keys.clear();

						if (ReactInspectorButton(Target, Button, "Push frame"))
						{
							AnimatorKey Key;
							Key.Scale = 1.0f;
							Key.Position = 0.0f;
							Key.Rotation = 0.0f;

							Clip->Keys.push_back(Key);
						}

						if (ReactInspectorButton(Target, Button, "Push model frame"))
						{
							AnimatorKey Key;
							Key.Scale = Base->GetEntity()->Transform->Scale;
							Key.Position = Base->GetEntity()->Transform->Position;
							Key.Rotation = Base->GetEntity()->Transform->Rotation.SaturateRotation();

							Clip->Keys.push_back(Key);
						}

						if (ReactInspectorButton(Target, Button, "Push view frame"))
						{
							AnimatorKey Key;
							Key.Scale = App->Scene->GetCamera()->GetEntity()->Transform->Scale;
							Key.Position = App->Scene->GetCamera()->GetEntity()->Transform->Position;
							Key.Rotation = App->Scene->GetCamera()->GetEntity()->Transform->Rotation.SaturateRotation();

							Clip->Keys.push_back(Key);
						}

						if (ReactInspectorButton(Target, Button, "Push view-based frame"))
						{
							AnimatorKey Key;
							Key.Scale = App->Scene->GetCamera()->GetEntity()->Transform->Scale;
							Key.Position = App->Scene->GetCamera()->GetEntity()->Transform->Position;
							Key.Rotation = 0;

							Clip->Keys.push_back(Key);
						}

						return Active;
					});

					for (size_t j = 0; j < Clip->Keys.size(); j++)
					{
						AnimatorKey* Key = &Clip->Keys[j];
						bool Erase = false;

						Target->ComposeStateful(Tree, [=]()
						{
							Tree->Source.Text = "Frame " + std::to_string(j);
						}, [&](bool Active)
						{
							if (!Active)
								return Active;

							Target->Compose(Layout, nullptr);
							ReactInspectorVector3(Target, Vector, &Key->Position, "Position");
							ReactInspectorVector3(Target, Vector, &Key->Scale, "Scale");
							ReactInspectorRotation(Target, Property, &Key->Rotation, "Rotation");
							ReactInspectorFloat(Target, Property, &Key->Time, 0.0, 32, "Time");

							if (ReactInspectorButton(Target, Button, "Remove frame"))
								Erase = true;

							return Active;
						});

						if (Erase)
						{
							Clip->Keys.erase(Clip->Keys.begin() + j);
							j--;
						}
					}

					return Active;
				});
			}

			if (ReactInspectorButton(Target, Button, "Push new clip"))
				Base->Clips.push_back(KeyAnimatorClip());

			if (!Base->GetPath().empty())
				ReactInspectorText(Target, Text, "Changes will not be saved");

			return Active;
		});

		Target->Compose(Layout, nullptr);

		int Frame = (int)Base->State.Clip;
		ReactInspectorInt(Target, Property, &Frame, -1, (int)Base->Clips.size() - 1, "Animation clip");
		Base->State.Clip = Frame;

		if (Base->State.Clip >= 0 && (size_t)Base->State.Clip < Base->Clips.size())
		{
			Frame = Base->State.Frame;
			ReactInspectorInt(Target, Property, &Frame, -1, (int)Base->Clips[Base->State.Clip].Keys.size() - 1, "Animation frame");
			Base->State.Frame = Frame;
		}

		ReactInspectorBool(Target, Checkbox, &App->State.IsPathTracked, "Track animation paths");
		ReactInspectorBool(Target, Checkbox, &Base->State.Looped, "Is animation looped");

		if (ReactInspectorButton(Target, Button, "Play animation"))
			Base->Play();

		if (ReactInspectorButton(Target, Button, "Pause animation"))
			Base->Pause();

		if (ReactInspectorButton(Target, Button, "Stop animation"))
			Base->Stop();

		if (!Base->GetPath().empty())
			Button->Source.Text = "Unassign source";
		else if (App->GetResourceState("key animation"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (Base->GetPath().empty())
			{
				if (!App->GetResourceState("key animation"))
				{
					App->GetResource("key animation", [=](const std::string& File)
					{
						Base->GetAnimation(App->Content, File);
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->ClearAnimation();

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, SkinAnimator))
	{
		auto* Base = Ref->As<Components::SkinAnimator>();
		Target->ComposeStateful(Tree, [=]()
		{
			Tree->Source.Text = "Animation source";
		}, [=](bool Active)
		{
			if (!Active)
				return Active;

			for (size_t i = 0; i < Base->Clips.size(); i++)
			{
				auto* Clip = &Base->Clips[i];
				Target->ComposeStateful(Tree, [=]()
				{
					Tree->Source.Text = Clip->Name + " (" + std::to_string(i) + ")";
				}, [=](bool Active)
				{
					if (!Active)
						return Active;

					Target->ComposeStateful(Tree, [=]()
					{
						Tree->Source.Text = "Properties";
					}, [=](bool Active)
					{
						if (!Active)
							return Active;

						Target->Compose(Layout, nullptr);
						ReactInspectorText(Target, Edit, &Clip->Name, "Clip name");
						ReactInspectorFloat(Target, Property, &Clip->Duration, 0.0, 128, "Duration");
						ReactInspectorFloat(Target, Property, &Clip->Rate, 0.0, 32, "Rate");

						if (ReactInspectorButton(Target, Button, "Normalize frames"))
						{
							for (size_t i = 0; i < Clip->Keys.size(); i++)
							{
								for (size_t j = 0; j < Clip->Keys[i].Pose.size(); j++)
									Clip->Keys[i].Pose[j].Rotation = Clip->Keys[i].Pose[j].Rotation.SaturateRotation();
							}
						}

						if (ReactInspectorButton(Target, Button, "Push frame"))
						{
							SkinAnimatorKey Key;
							Key.Pose.resize(Base->Default.Pose.size());
							Key.Time = Base->Default.Time;

							Clip->Keys.push_back(Key);
						}

						if (ReactInspectorButton(Target, Button, "Push model frame"))
						{
							SkinAnimatorKey Key;
							Base->GetPose(&Key);

							Clip->Keys.push_back(Key);
						}

						return Active;
					});

					for (size_t j = 0; j < Clip->Keys.size(); j++)
					{
						SkinAnimatorKey* Keys = &Clip->Keys[j];
						bool Erase = false;

						Target->ComposeStateful(Tree, [=]()
						{
							Tree->Source.Text = "Frame " + std::to_string(j);
						}, [&](bool Active)
						{
							if (!Active)
								return Active;

							for (size_t k = 0; k < Keys->Pose.size(); k++)
							{
								AnimatorKey* Key = &Keys->Pose[k];
								Target->ComposeStateful(Tree, [=]()
								{
									Tree->Source.Text = "Joint " + std::to_string(k);
								}, [&](bool Active)
								{
									if (!Active)
										return Active;

									Target->Compose(Layout, nullptr);
									ReactInspectorVector3(Target, Vector, &Key->Position, "Position");
									ReactInspectorVector3(Target, Vector, &Key->Scale, "Scale");
									ReactInspectorRotation(Target, Property, &Key->Rotation, "Rotation");
									ReactInspectorFloat(Target, Property, &Key->Time, 0.0, 32, "Time");

									return Active;
								});
							}

							if (ReactInspectorButton(Target, Button, "Remove frame"))
								Erase = true;

							return Active;
						});

						if (Erase)
						{
							Clip->Keys.erase(Clip->Keys.begin() + j);
							j--;
						}
					}

					return Active;
				});
			}

			if (ReactInspectorButton(Target, Button, "Push new clip"))
				Base->Clips.push_back(SkinAnimatorClip());

			if (!Base->GetPath().empty())
				ReactInspectorText(Target, Text, "Changes will not be saved");

			return Active;
		});

		Target->Compose(Layout, nullptr);

		int Frame = (int)Base->State.Clip;
		ReactInspectorInt(Target, Property, &Frame, -1, (int)Base->Clips.size() - 1, "Animation clip");
		Base->State.Clip = Frame;

		if (Base->State.Clip >= 0 && (size_t)Base->State.Clip < Base->Clips.size())
		{
			Frame = Base->State.Frame;
			ReactInspectorInt(Target, Property, &Frame, -1, (int)Base->Clips[Base->State.Clip].Keys.size() - 1, "Animation frame");
			Base->State.Frame = Frame;
		}

		ReactInspectorBool(Target, Checkbox, &Base->State.Looped, "Is animation looped");
		if (ReactInspectorButton(Target, Button, "Play animation"))
			Base->Play();

		if (ReactInspectorButton(Target, Button, "Pause animation"))
			Base->Pause();

		if (ReactInspectorButton(Target, Button, "Stop animation"))
			Base->Stop();

		if (!Base->GetPath().empty())
			Button->Source.Text = "Unassign source";
		else if (App->GetResourceState("skin animation"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (Base->GetPath().empty())
			{
				if (!App->GetResourceState("skin animation"))
				{
					App->GetResource("skin animation", [=](const std::string& File)
					{
						Base->GetAnimation(App->Content, File);
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->ClearAnimation();

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, EmitterAnimator))
	{
		auto* Base = Ref->As<Components::EmitterAnimator>();
		Target->ComposeStateful(Tree, [=]()
		{
			Tree->Source.Text = "Spawner";
		}, [=](bool Active)
		{
			if (!Active)
				return Active;

			ReactInspectorInt(Target, Property, &Base->Spawner.Iterations, 0, 50, "Iterations");
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Rotation";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorRadians(Target, Property, &Base->Spawner.Rotation.Min, 0, Base->Spawner.Rotation.Max, "Min");
				ReactInspectorRadians(Target, Property, &Base->Spawner.Rotation.Max, Base->Spawner.Rotation.Min, 360, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Rotation.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Rotation.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Angular";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorRadians(Target, Property, &Base->Spawner.Rotation.Min, 0, Base->Spawner.Angular.Max, "Min");
				ReactInspectorRadians(Target, Property, &Base->Spawner.Rotation.Max, Base->Spawner.Angular.Min, 360, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Angular.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Angular.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Scale";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorFloat(Target, Property, &Base->Spawner.Rotation.Min, 0.001f, Base->Spawner.Scale.Max, "Min");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Rotation.Max, Base->Spawner.Scale.Min, 10, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Scale.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Scale.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Diffusion";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorVector4(Target, Vector, &Base->Spawner.Diffusion.Min, "Min");
				ReactInspectorVector4(Target, Vector, &Base->Spawner.Diffusion.Max, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Diffusion.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Diffusion.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Position";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorVector3(Target, Vector, &Base->Spawner.Position.Min, "Min");
				ReactInspectorVector3(Target, Vector, &Base->Spawner.Position.Max, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Position.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Position.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Velocity";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorVector3(Target, Vector, &Base->Spawner.Velocity.Min, "Min");
				ReactInspectorVector3(Target, Vector, &Base->Spawner.Velocity.Max, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Velocity.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Velocity.Intensity, "Is intensitive");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Noise";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				ReactInspectorVector3(Target, Vector, &Base->Spawner.Noise.Min, "Min");
				ReactInspectorVector3(Target, Vector, &Base->Spawner.Noise.Max, "Max");
				ReactInspectorFloat(Target, Property, &Base->Spawner.Noise.Accuracy, 1, 1000, "Accuracy");
				ReactInspectorBool(Target, Checkbox, &Base->Spawner.Noise.Intensity, "Is intensitive");

				return Active;
			});

			return Active;
		});

		ReactInspectorVector3(Target, Vector, &Base->Position, "Position");
		ReactInspectorVector3(Target, Vector, &Base->Velocity, "Velocity");
		ReactInspectorVector4(Target, Vector, &Base->Diffuse, "Diffuse");
		ReactInspectorRadians(Target, Property, &Base->RotationSpeed, -360, 360, "Rotation speed");
		ReactInspectorFloat(Target, Property, &Base->ScaleSpeed, -10, 10, "Scale speed");
		ReactInspectorFloat(Target, Property, &Base->Noisiness, 0, 1000, "Noisiness");
		ReactInspectorBool(Target, Checkbox, &Base->Simulate, "Is simulation enabled");
	}
	else if (TH_COMPONENT_IS(Ref, FreeLook))
	{
		auto* Base = Ref->As<Components::FreeLook>();
		Target->Compose(Layout, nullptr);

		ReactInspectorFloat(Target, Property, &Base->Sensitivity, 0, 4, "Sensitivity");
		ReactInspectorKeyCode(Target, App, &Base->Rotate, "Rotate");
	}
	else if (TH_COMPONENT_IS(Ref, Fly))
	{
		auto* Base = Ref->As<Components::Fly>();
		Target->Compose(Layout, nullptr);

		ReactInspectorVector3(Target, Vector, &Base->Axis, "Axis");
		ReactInspectorFloat(Target, Property, &Base->SpeedDown, 0, 4, "Speed down");
		ReactInspectorFloat(Target, Property, &Base->SpeedNormal, 0, 4, "Speed normal");
		ReactInspectorFloat(Target, Property, &Base->SpeedUp, 0, 4, "Speed up");
		ReactInspectorKeyCode(Target, App, &Base->Forward, "Forward");
		ReactInspectorKeyCode(Target, App, &Base->Backward, "Backward");
		ReactInspectorKeyCode(Target, App, &Base->Right, "Right");
		ReactInspectorKeyCode(Target, App, &Base->Left, "Left");
		ReactInspectorKeyCode(Target, App, &Base->Up, "Up");
		ReactInspectorKeyCode(Target, App, &Base->Down, "Down");
		ReactInspectorKeyCode(Target, App, &Base->Fast, "Fast");
		ReactInspectorKeyCode(Target, App, &Base->Slow, "Slow");
	}
	else if (TH_COMPONENT_IS(Ref, Model) || TH_COMPONENT_IS(Ref, LimpidModel))
	{
		auto* Base = Ref->As<Components::Model>();
		if (Base->GetDrawable() != nullptr)
		{
			for (auto It : Base->GetDrawable()->Meshes)
			{
				Target->ComposeStateful(Tree, [=]()
				{
					Tree->Source.Text = (It->Name.empty() ? "Unidentified" : It->Name);
				}, [=](bool Active)
				{
					if (!Active)
						return Active;

					ReactInspectorAppearance(Target, App, Base->GetSurface(It));
					ReactInspectorConst(Target, Property, It->GetIndexBuffer()->GetElements(), "Indices count");
					ReactInspectorConst(Target, Property, It->GetVertexBuffer()->GetElements(), "Vertices count");

					return Active;
				});
			}

			Button->Source.Text = "Unassign source";
		}
		else if (App->GetResourceState("mesh"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Layout, nullptr);
		ReactInspectorBool(Target, Checkbox, &Base->Static, "Is static");
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetDrawable())
			{
				if (!App->GetResourceState("mesh"))
				{
					App->GetResource("mesh", [=](const std::string& File)
					{
						Base->SetDrawable(App->Content->Load<Tomahawk::Graphics::Model>(File, nullptr));
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->SetDrawable(nullptr);

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, Skin) || TH_COMPONENT_IS(Ref, LimpidSkin))
	{
		auto* Base = Ref->As<Components::Skin>();
		if (Base->GetDrawable() != nullptr)
		{
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Skeleton tree";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				for (auto& Joint : Base->GetDrawable()->Joints)
					ReactInspectorJoint(Target, App, &Base->Skeleton, &Joint);

				return Active;
			});

			for (auto It : Base->GetDrawable()->Meshes)
			{
				Target->ComposeStateful(Tree, [=]()
				{
					Tree->Source.Text = (It->Name.empty() ? "Unidentified" : It->Name);
				}, [=](bool Active)
				{
					if (!Active)
						return Active;

					ReactInspectorAppearance(Target, App, Base->GetSurface(It));
					ReactInspectorConst(Target, Property, It->GetIndexBuffer()->GetElements(), "Indices count");
					ReactInspectorConst(Target, Property, It->GetVertexBuffer()->GetElements(), "Vertices count");

					return Active;
				});
			}

			Target->Compose(Layout, nullptr);
			ReactInspectorConst(Target, Property, (int)Base->Skeleton.Pose.size(), "Joints count");

			Button->Source.Text = "Unassign source";
		}
		else if (App->GetResourceState("skin mesh"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Layout, nullptr);
		ReactInspectorBool(Target, Checkbox, &Base->Static, "Is static");
		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetDrawable())
			{
				if (!App->GetResourceState("skin mesh"))
				{
					App->GetResource("skin mesh", [=](const std::string& File)
					{
						Base->SetDrawable(App->Content->Load<Tomahawk::Graphics::SkinModel>(File, nullptr));
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->SetDrawable(nullptr);

			return Active;
		});
	}
	else if (TH_COMPONENT_IS(Ref, Decal) || TH_COMPONENT_IS(Ref, LimpidDecal))
	{
		auto* Base = Ref->As<Components::Decal>();
		ReactInspectorAppearance(Target, App, Base->GetSurface());
		ReactInspectorFloat(Target, Property, &Base->FieldOfView, 0, 360, "Field of view");
		ReactInspectorFloat(Target, Property, &Base->Distance, 1, 1000, "Distance");
	}
	else if (TH_COMPONENT_IS(Ref, Emitter) || TH_COMPONENT_IS(Ref, LimpidEmitter))
	{
		auto* Base = Ref->As<Components::Emitter>();
		Target->ComposeStateful(Tree, [=]()
		{
			Tree->Source.Text = "Material";
		}, [=](bool Active)
		{
			if (Active)
				ReactInspectorAppearance(Target, App, Base->GetSurface());

			return Active;
		});

		Target->Compose(Layout, nullptr);
		if (ReactInspectorButton(Target, Button, "Refill element buffer"))
		{
			if (App->State.ElementsLimit < 1)
				App->State.ElementsLimit = 1;

			if (App->State.ElementsLimit > 5000000)
				App->State.ElementsLimit = 5000000;

			App->Renderer->UpdateBufferSize(Base->GetBuffer(), App->State.ElementsLimit);
		}

		ReactInspectorVector3(Target, Vector, &Base->Volume, "Volume");
		ReactInspectorInt(Target, Property, &App->State.ElementsLimit, 1024, 5000000, "Elements limit");
		ReactInspectorConst(Target, Property, Base->GetBuffer()->GetArray()->Size(), "Elements count");
		ReactInspectorBool(Target, Checkbox, &Base->QuadBased, "Is quad based");
		ReactInspectorBool(Target, Checkbox, &Base->Connected, "Is connected");
		ReactInspectorBool(Target, Checkbox, &Base->Static, "Is static");
	}
	else if (TH_COMPONENT_IS(Ref, PointLight))
	{
		auto* Base = Ref->As<Components::PointLight>();
		ReactInspectorText(Target, Text, "Diffuse color");
		ReactInspectorColor(Target, ColorPicker, Layout, &Base->Diffuse);
		ReactInspectorInt(Target, Property, &Base->ShadowIterations, 1, 8, "PCF filter");
		ReactInspectorFloat(Target, Property, &Base->Emission, 0, 16, "Emission");
		ReactInspectorFloat(Target, Property, &Base->ShadowBias, -0.001, 0.001, "Shadow bias");
		ReactInspectorFloat(Target, Property, &Base->ShadowDistance, 1, 1000, "Shadow distance");
		ReactInspectorFloat(Target, Property, &Base->ShadowSoftness, 0, 10, "Shadow softness");
		ReactInspectorBool(Target, Checkbox, &Base->Shadowed, "Is shadowed");
	}
	else if (TH_COMPONENT_IS(Ref, SpotLight))
	{
		auto* Base = Ref->As<Components::SpotLight>();
		ReactInspectorText(Target, Text, "Diffuse color");
		ReactInspectorColor(Target, ColorPicker, Layout, &Base->Diffuse);
		ReactInspectorTexture2D(Target, App, &Base->ProjectMap, "Project map");
		ReactInspectorInt(Target, Property, &Base->ShadowIterations, 1, 8, "PCF filter");
		ReactInspectorFloat(Target, Property, &Base->Emission, 0, 16, "Emission");
		ReactInspectorFloat(Target, Property, &Base->FieldOfView, 0, 360, "Field of view");
		ReactInspectorFloat(Target, Property, &Base->ShadowBias, -0.001, 0.001, "Shadow bias");
		ReactInspectorFloat(Target, Property, &Base->ShadowDistance, 1, 1000, "Shadow distance");
		ReactInspectorFloat(Target, Property, &Base->ShadowSoftness, 0, 10, "Shadow softness");
		ReactInspectorBool(Target, Checkbox, &Base->Shadowed, "Is shadowed");
	}
	else if (TH_COMPONENT_IS(Ref, LineLight))
	{
		auto* Base = Ref->As<Components::LineLight>();
		Vector3 RlhEmission = Base->RlhEmission * 3000;
		Vector3 MieEmission = Base->MieEmission * 3000;

		ReactInspectorText(Target, Text, "Diffuse color");
		ReactInspectorColor(Target, ColorPicker, Layout, &Base->Diffuse);
		ReactInspectorText(Target, Text, "Rlh emission");
		ReactInspectorColor(Target, ColorPicker, Layout, &RlhEmission);
		ReactInspectorText(Target, Text, "Mie emission");
		ReactInspectorColor(Target, ColorPicker, Layout, &MieEmission);
		ReactInspectorInt(Target, Property, &Base->ShadowIterations, 1, 8, "PCF filter");
		ReactInspectorFloat(Target, Property, &Base->Emission, 0, 16, "Emission");
		ReactInspectorFloat(Target, Property, &Base->RlhHeight, 100, 10000, "Rlh height");
		ReactInspectorFloat(Target, Property, &Base->MieHeight, 100, 10000, "Mie height");
		ReactInspectorFloat(Target, Property, &Base->MieDirection, -1, 1, "Mie direction");
		ReactInspectorFloat(Target, Property, &Base->ScatterIntensity, 0, 12, "Scatter intensity");
		ReactInspectorFloat(Target, Property, &Base->PlanetRadius, 100000, 10000000.0f, "Planet radius");
		ReactInspectorFloat(Target, Property, &Base->AtmosphereRadius, 100000, 10000000.0f, "Atmosphere radius");
		ReactInspectorFloat(Target, Property, &Base->ShadowFarBias, 1, 100, "Shadow far bias");
		ReactInspectorFloat(Target, Property, &Base->ShadowLength, 0, 100, "Shadow fade power");
		ReactInspectorFloat(Target, Property, &Base->ShadowHeight, 0, 100, "Shadow height");
		ReactInspectorFloat(Target, Property, &Base->ShadowBias, -0.001, 0.001, "Shadow bias");
		ReactInspectorFloat(Target, Property, &Base->ShadowDistance, 1, 1000, "Shadow distance");
		ReactInspectorFloat(Target, Property, &Base->ShadowSoftness, 0, 10, "Shadow softness");
		ReactInspectorBool(Target, Checkbox, &Base->Shadowed, "Is shadowed");

		Base->RlhEmission = RlhEmission / 3000;
		Base->MieEmission = MieEmission / 3000;
	}
	else if (TH_COMPONENT_IS(Ref, ReflectionProbe))
	{
		auto* Base = Ref->As<Components::ReflectionProbe>();
		ReactInspectorText(Target, Text, "Reflection color");
		ReactInspectorColor(Target, ColorPicker, Layout, &Base->Diffuse);
		ReactInspectorDouble(Target, Property, &Base->Rebuild.Delay, 1, 10000, "Rebuild delay");
		ReactInspectorVector3(Target, Vector, &Base->ViewOffset, "View offset");
		ReactInspectorFloat(Target, Property, &Base->CaptureRange, 0, 1000, "Capture range");
		ReactInspectorFloat(Target, Property, &Base->Emission, 0, 16, "Emission");
		ReactInspectorFloat(Target, Property, &Base->Infinity, 0, 1, "Infinity");

		if (App->GetResourceState("probe map"))
			Button->Source.Text = "Awaiting source...";
		else if (Base->GetDiffuseMap() != nullptr)
			Button->Source.Text = "Unassign probe map";
		else
			Button->Source.Text = "Assign probe map";

		Target->Compose(Layout, nullptr);
		if (Base->GetDiffuseMap() != nullptr)
		{
			GUI::Contextual* Contextual = Target->Get<GUI::Contextual>("contextual");
			if (Contextual != nullptr)
			{
				Contextual->Source.TriggerX = Button->GetWidgetPosition().X;
				Contextual->Source.TriggerY = Button->GetWidgetPosition().Y;
				Contextual->Source.TriggerWidth = Button->GetWidgetWidth();
				Contextual->Source.TriggerHeight = Button->GetWidgetHeight();
				Contextual->Source.Width = Contextual->Source.TriggerWidth * 0.5f;

				Target->Compose(Contextual, [=](bool Active)
				{
					if (!Active)
						return Active;

					GUI::Image* Image = Target->Get<GUI::Image>("image");
					if (Image != nullptr)
					{
						float Height = Layout->Source.Height;

						Layout->Source.Height = (Contextual->Source.Width * 0.9f) / Layout->GetAreaHeight();
						Target->Compose(Layout, nullptr);

						Image->Source.Image = Base->GetDiffuseMap();
						Target->Compose(Image, nullptr);

						Layout->Source.Height = Height;
					}

					return Active;
				});
			}
		}

		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (!Base->GetDiffuseMap())
			{
				if (!App->GetResourceState("probe map"))
				{
					App->GetResource("probe map", [=](const std::string& File)
					{
						Base->SetDiffuseMap(App->Content->Load<Texture2D>(File, nullptr));
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->SetDiffuseMap(nullptr);

			return Active;
		});

		ReactInspectorBool(Target, Checkbox, &Base->ParallaxCorrected, "Parallax corrected");
		ReactInspectorBool(Target, Checkbox, &Base->StaticMask, "Static mask");
	}
	else if (TH_COMPONENT_IS(Ref, Camera))
	{
		auto* Base = Ref->As<Components::Camera>();
		for (auto It = Base->GetRenderer()->GetRenderers()->begin(); It != Base->GetRenderer()->GetRenderers()->end(); It++)
		{
			Tomahawk::Engine::Renderer* Render = *It;
			Contextual->Source.TriggerX = Tree->GetWidgetPosition().X;
			Contextual->Source.TriggerY = Tree->GetWidgetPosition().Y;
			Contextual->Source.TriggerWidth = Tree->GetWidgetWidth();
			Contextual->Source.TriggerHeight = Tree->GetWidgetHeight();
			Contextual->Source.Width = Contextual->Source.TriggerWidth * 0.5f;

			bool Removed = false;
			Target->Compose(Contextual, [&](bool Active)
			{
				if (!Active)
					return Active;

				Target->Compose(Layout, nullptr);
				if (ReactInspectorButton(Target, Button, "Toggle"))
				{
					Render->Active = !Render->Active;
					Contextual->Close();
				}

				if (ReactInspectorButton(Target, Button, "Rebuild"))
				{
					Render->Deactivate();
					Render->Activate();
					Contextual->Close();
				}

				if (ReactInspectorButton(Target, Button, "Remove"))
				{
					Base->GetRenderer()->RemoveRenderer(Render->GetId());
					Contextual->Close();
					Removed = true;
				}

				if (ReactInspectorButton(Target, Button, "Move up"))
				{
					Base->GetRenderer()->MoveRenderer(Render->GetId(), -1);
					Contextual->Close();
					Removed = true;
				}

				if (ReactInspectorButton(Target, Button, "Move down"))
				{
					Base->GetRenderer()->MoveRenderer(Render->GetId(), 1);
					Contextual->Close();
					Removed = true;
				}

				return Active;
			});

			if (Removed)
				break;

			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = App->GetPascal(Render->GetName()) + (Render->Active ? "" : " (inactive)");
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				Target->Compose(Layout, nullptr);
				if (TH_COMPONENT_IS(Render, DepthRenderer))
				{
					auto* Source = Render->As<Renderers::DepthRenderer>();
					ReactInspectorBigInt(Target, Property, &Source->Renderers.PointLightResolution, 4, 4096, "Point light resolution");
					ReactInspectorBigInt(Target, Property, &Source->Renderers.PointLightLimits, 0, 32, "Point light limit");
					ReactInspectorBigInt(Target, Property, &Source->Renderers.SpotLightResolution, 4, 4096, "Spot light resolution");
					ReactInspectorBigInt(Target, Property, &Source->Renderers.SpotLightLimits, 0, 32, "Spot light limit");
					ReactInspectorBigInt(Target, Property, &Source->Renderers.LineLightResolution, 4, 4096, "Line light resolution");
					ReactInspectorBigInt(Target, Property, &Source->Renderers.LineLightLimits, 0, 32, "Line light limit");
					ReactInspectorFloat(Target, Property, &Source->ShadowDistance, 0, 1000, "Shadow Distance");
				}
				else if (TH_COMPONENT_IS(Render, LightRenderer))
				{
					auto* Source = Render->As<Renderers::LightRenderer>();
					ReactInspectorText(Target, Text, "High emission");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->AmbientLight.HighEmission);
					ReactInspectorText(Target, Text, "Low emission");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->AmbientLight.LowEmission);
					ReactInspectorText(Target, Text, "Sky color");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->AmbientLight.SkyColor);
					ReactInspectorFloat(Target, Property, &Source->AmbientLight.SkyEmission, 0, 2, "Sky emission");
					ReactInspectorFloat(Target, Property, &Source->AmbientLight.LightEmission, 0, 2, "Light emission");

					if (App->GetResourceState("sky map"))
						Button->Source.Text = "Awaiting source...";
					else if (Source->GetSkyMap() != nullptr)
						Button->Source.Text = "Unassign sky map";
					else
						Button->Source.Text = "Assign sky map";

					Target->Compose(Layout, nullptr);
					if (Source->GetSkyMap() != nullptr)
					{
						GUI::Contextual* Contextual = Target->Get<GUI::Contextual>("contextual");
						if (Contextual != nullptr)
						{
							Contextual->Source.TriggerX = Button->GetWidgetPosition().X;
							Contextual->Source.TriggerY = Button->GetWidgetPosition().Y;
							Contextual->Source.TriggerWidth = Button->GetWidgetWidth();
							Contextual->Source.TriggerHeight = Button->GetWidgetHeight();
							Contextual->Source.Width = Contextual->Source.TriggerWidth * 0.5f;

							Target->Compose(Contextual, [=](bool Active)
							{
								if (!Active)
									return Active;

								GUI::Image* Image = Target->Get<GUI::Image>("image");
								if (Image != nullptr)
								{
									float Height = Layout->Source.Height;

									Layout->Source.Height = (Contextual->Source.Width * 0.9f) / Layout->GetAreaHeight();
									Target->Compose(Layout, nullptr);

									Image->Source.Image = Source->GetSkyBase();
									Target->Compose(Image, nullptr);

									Layout->Source.Height = Height;
								}

								return Active;
							});
						}
					}

					Target->Compose(Button, [=](bool Active)
					{
						if (!Active)
							return Active;

						if (!Source->GetSkyMap())
						{
							if (!App->GetResourceState("sky map"))
							{
								App->GetResource("sky map", [=](const std::string& File)
								{
									Source->SetSkyMap(App->Content->Load<Texture2D>(File, nullptr));
								});
							}
							else
								App->GetResource("", nullptr);
						}
						else
							Source->SetSkyMap(nullptr);

						return Active;
					});

					ReactInspectorBool(Target, Checkbox, &Source->RecursiveProbes, "Recursive probes");
				}
				else if (TH_COMPONENT_IS(Render, ProbeRenderer))
				{
					auto* Source = Render->As<Renderers::ProbeRenderer>();
					if (ReactInspectorBigInt(Target, Property, &Source->Size, 4, 4096, "Resolution"))
						Source->SetCaptureSize(Source->Size);

					ReactInspectorBigInt(Target, Property, &Source->MipLevels, 0, 11, "Mip levels");
				}
				else if (TH_COMPONENT_IS(Render, ReflectionsRenderer))
				{
					auto* Source = Render->As<Renderers::ReflectionsRenderer>();
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Samples, 1, 64, "Sample count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Samples, 1, 8, "Blur count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Intensity, 0, 3, "Intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Blur, 0, 2, "Blur");
				}
				else if (TH_COMPONENT_IS(Render, DepthOfFieldRenderer))
				{
					auto* Source = Render->As<Renderers::DepthOfFieldRenderer>();
					ReactInspectorFloat(Target, Property, &Source->FocusDistance, -1.0f, 1000, "Auto focus distance");
					ReactInspectorFloat(Target, Property, &Source->FocusTime, 0.01f, 2.0f, "Auto focus speed");
					ReactInspectorFloat(Target, Property, &Source->FocusRadius, 0, 8.0f, "Auto focus radius");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Radius, 0, 8, "Radius");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Bokeh, 0, 512, "Bokeh");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Scale, -7, 7, "Scale");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.NearDistance, 0, 1000, "Near distance");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.NearRange, 0, 50, "Near range");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.FarDistance, 0, 1000, "Far distance");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.FarRange, 0, 50, "Far range");
				}
				else if (TH_COMPONENT_IS(Render, EmissionRenderer))
				{
					auto* Source = Render->As<Renderers::EmissionRenderer>();
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Samples, 0, 128, "Sample count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Scale, 0, 16, "Scale");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Intensity, 0, 10, "Intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.Threshold, 0, 1, "Threshold");
				}
				else if (TH_COMPONENT_IS(Render, GlitchRenderer))
				{
					auto* Source = Render->As<Renderers::GlitchRenderer>();
					ReactInspectorFloat(Target, Property, &Source->ColorDrift, 0, 5, "Color drift");
					ReactInspectorFloat(Target, Property, &Source->HorizontalShake, 0, 5, "Horizontal shake");
					ReactInspectorFloat(Target, Property, &Source->ScanLineJitter, 0, 5, "Scan line jitter");
					ReactInspectorFloat(Target, Property, &Source->VerticalJump, 0, 0.1, "Vertical jump");
				}
				else if (TH_COMPONENT_IS(Render, AmbientOcclusionRenderer))
				{
					auto* Source = Render->As<Renderers::AmbientOcclusionRenderer>();
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Samples, 1, 8, "Sample count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Samples, 1, 8, "Blur count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Distance, 0, 15, "Distance");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Fade, 1, 32, "Fade");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Bias, -1, 2, "Bias");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Intensity, 0, 20, "Intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Scale, 0, 5, "Scale");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Radius, 0, 1, "Radius");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Blur, 0, 8, "Blur strength");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Power, 0, 8, "Blur power");
				}
				else if (TH_COMPONENT_IS(Render, DirectOcclusionRenderer))
				{
					auto* Source = Render->As<Renderers::DirectOcclusionRenderer>();
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Samples, 1, 8, "Sample count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Samples, 1, 8, "Blur count");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Distance, 0, 15, "Distance");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Fade, 1, 32, "Fade");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Bias, -1, 2, "Bias");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Intensity, 0, 20, "Intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Scale, 0, 5, "Scale");
					ReactInspectorFloat(Target, Property, &Source->RenderPass1.Radius, 0, 1, "Radius");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Blur, 0, 8, "Blur strength");
					ReactInspectorFloat(Target, Property, &Source->RenderPass2.Power, 0, 8, "Blur power");
				}
				else if (TH_COMPONENT_IS(Render, ToneRenderer))
				{
					auto* Source = Render->As<Renderers::ToneRenderer>();
					ReactInspectorText(Target, Text, "Desaturation gamma");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.DesaturationGamma);
					ReactInspectorText(Target, Text, "Color gamma");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.ColorGamma);
					ReactInspectorText(Target, Text, "Vignette color");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.VignetteColor);
					ReactInspectorText(Target, Text, "Red tone");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.BlindVisionR);
					ReactInspectorText(Target, Text, "Green tone");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.BlindVisionG);
					ReactInspectorText(Target, Text, "Blue tone");
					ReactInspectorColor(Target, ColorPicker, Layout, &Source->RenderPass.BlindVisionB);
					ReactInspectorFloat(Target, Property, &Source->RenderPass.VignetteAmount, 0, 2, "Vignette amount");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.VignetteCurve, -5, 5, "Vignette curve");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.VignetteRadius, 0, 3, "Vignette radius");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.GammaIntensity, 0.1, 4, "Gamma correction");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.LinearIntensity, 0, 1, "GC intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.ToneIntensity, 0, 1, "Tone intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.DesaturationIntensity, 0, 1, "Desaturation intensity");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesIntensity, 0, 1, "Aces film");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesA, 0, 5, "Aces film A");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesB, 0, 5, "Aces film B");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesC, 0, 5, "Aces film C");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesD, 0, 5, "Aces film D");
					ReactInspectorFloat(Target, Property, &Source->RenderPass.AcesE, 0, 5, "Aces film E");
				}
				else
					ReactInspectorText(Target, Text, "There are no parameters");

				return Active;
			});
		}

		uint64_t Size = Base->GetRenderer()->GetDepthSize();
		uint64_t Stalls = Base->GetRenderer()->StallFrames;

		ReactInspectorFloat(Target, Property, &Base->FieldOfView, 1, 90, "Field of view");
		ReactInspectorFloat(Target, Property, &Base->Width, -1, 8192, "Width");
		ReactInspectorFloat(Target, Property, &Base->Height, -1, 8192, "Height");
		ReactInspectorFloat(Target, Property, &Base->FarPlane, 0, 10000, "Far plane");
		ReactInspectorFloat(Target, Property, &Base->NearPlane, 0, 10000, "Near plane");
		ReactInspectorDouble(Target, Property, &Base->GetRenderer()->Occlusion.Delay, 0, 100, "OC render delay");
		ReactInspectorDouble(Target, Property, &Base->GetRenderer()->Sorting.Delay, 0, 200, "OC sorting delay");
		ReactInspectorBigInt(Target, Property, &Stalls, 0, 120, "OC stalled frames");
		ReactInspectorBigInt(Target, Property, &Size, 32, 1024, "OC depth buffer");

		Base->GetRenderer()->StallFrames = Stalls;
		if (Size != Base->GetRenderer()->GetDepthSize())
			Base->GetRenderer()->SetDepthSize(Size);

		if (ReactInspectorButton(Target, Button, "Set as perspective"))
			Base->Mode = Components::Camera::ProjectionMode_Perspective;

		if (ReactInspectorButton(Target, Button, "Set as orthographic"))
			Base->Mode = Components::Camera::ProjectionMode_Orthographic;

		ReactInspectorBool(Target, Checkbox, &Base->GetRenderer()->EnableFrustumCull, "Frustum culling");
		if (ReactInspectorBool(Target, Checkbox, &Base->GetRenderer()->EnableOcclusionCull, "Occlusion culling"))
			Base->GetRenderer()->ClearCull();

		bool Option = !App->State.IsCameraActive;
		if (ReactInspectorBool(Target, Checkbox, &Option, "Enable preview"))
		{
			if (!Base->IsActive() || !Option)
			{
				auto* Main = App->State.Camera->GetComponent<Components::Camera>();
				App->Scene->SetCamera(App->State.Camera);

				if (App->Scene->IsActive())
				{
					Main->GetRenderer()->EnableOcclusionCull = false;
					Main->GetRenderer()->EnableFrustumCull = false;
				}
				else
					Main->GetRenderer()->ClearCull();
			}
			else
				App->Scene->SetCamera(Base->GetEntity());

			App->State.IsCameraActive = !Option;
			if (!Base->IsActive())
				App->State.IsCameraActive = true;
		}
		else if (Option && !Base->IsActive())
		{
			App->Scene->SetCamera(App->State.Camera);
			App->State.IsCameraActive = true;
		}
	}
	else if (TH_COMPONENT_IS(Ref, Scriptable))
	{
		auto* Base = Ref->As<Components::Scriptable>();
		if (!Base->GetSource().empty())
		{
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Properties";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				Target->Compose(Layout, nullptr);

				int Count = Base->GetPropertiesCount();
				for (int i = 0; i < Count; i++)
				{
					VMProperty Result;
					if (!Base->GetPropertyByIndex(i, &Result) || !Result.Name || !Result.Pointer)
						continue;

					switch (Result.TypeId)
					{
						case VMTypeId_BOOL:
							ReactInspectorBool(Target, Checkbox, (bool*)Result.Pointer, Result.Name);
							break;
						case VMTypeId_INT8:
						{
							int V = *(char*)Result.Pointer;
							if (ReactInspectorInt(Target, Property, &V, std::numeric_limits<char>::min(), std::numeric_limits<char>::max(), Result.Name))
								*(char*)Result.Pointer = V;

							break;
						}
						case VMTypeId_INT16:
						{
							int V = *(short*)Result.Pointer;
							if (ReactInspectorInt(Target, Property, &V, std::numeric_limits<short>::min(), std::numeric_limits<short>::max(), Result.Name))
								*(short*)Result.Pointer = V;

							break;
						}
						case VMTypeId_INT32:
							ReactInspectorInt(Target, Property, (int*)Result.Pointer, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), Result.Name);
							break;
						case VMTypeId_INT64:
							ReactInspectorSBigInt(Target, Property, (int64_t*)Result.Pointer, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), Result.Name);
							break;
						case VMTypeId_UINT8:
						{
							int V = *(unsigned char*)Result.Pointer;
							if (ReactInspectorInt(Target, Property, &V, std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max(), Result.Name))
								*(unsigned char*)Result.Pointer = V;

							break;
						}
						case VMTypeId_UINT16:
						{
							int V = *(unsigned short*)Result.Pointer;
							if (ReactInspectorInt(Target, Property, &V, std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max(), Result.Name))
								*(unsigned short*)Result.Pointer = V;

							break;
						}
						case VMTypeId_UINT32:
						{
							int V = *(unsigned int*)Result.Pointer;
							if (ReactInspectorInt(Target, Property, &V, std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max(), Result.Name))
								*(unsigned int*)Result.Pointer = V;

							break;
						}
						case VMTypeId_UINT64:
							ReactInspectorBigInt(Target, Property, (uint64_t*)Result.Pointer, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max(), Result.Name);
							break;
						case VMTypeId_FLOAT:
							ReactInspectorFloat(Target, Property, (float*)Result.Pointer, std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), Result.Name);
							break;
						case VMTypeId_DOUBLE:
							ReactInspectorDouble(Target, Property, (double*)Result.Pointer, std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), Result.Name);
							break;
						default:
						{
							VMTypeInfo Type = Base->GetCompiler()->GetManager()->Global().GetTypeInfoById(Result.TypeId);
							if (Type.IsValid() && strcmp(Type.GetName(), "string") == 0)
							{
								ReactInspectorText(Target, Text, Result.Name);
								ReactInspectorText(Target, Edit, (std::string*)Result.Pointer, Result.Name);
							}
							break;
						}
					}
				}

				if (!Count)
					ReactInspectorText(Target, Text, "There are no properties");

				return Active;
			});
			Target->ComposeStateful(Tree, [=]()
			{
				Tree->Source.Text = "Functions";
			}, [=](bool Active)
			{
				if (!Active)
					return Active;

				Target->Compose(Layout, nullptr);

				int Count = Base->GetFunctionsCount();
				for (int i = 0; i < Count; i++)
				{
					VMFunction Function = Base->GetFunctionByIndex(i, 0);
					if (!Function.IsValid())
						continue;

					if (ReactInspectorButton(Target, Button, std::string("Call ") + Function.GetName()))
						Base->Call(Function.GetFunction(), nullptr);
				}

				if (!Count)
					ReactInspectorText(Target, Text, "There are no functions");

				return Active;
			});
			Button->Source.Text = "Unassign source";
		}
		else if (App->GetResourceState("script"))
			Button->Source.Text = "Awaiting source...";
		else
			Button->Source.Text = "Assign source";

		Target->Compose(Layout, nullptr);

		bool Typeless = (Base->GetInvokeType() == Components::Scriptable::InvokeType_Typeless);
		if (ReactInspectorBool(Target, Checkbox, &Typeless, "Typeless invocation"))
			Base->SetInvocation(Typeless ? Components::Scriptable::InvokeType_Typeless : Components::Scriptable::InvokeType_Normal);

		Target->Compose(Button, [=](bool Active)
		{
			if (!Active)
				return Active;

			if (Base->GetSource().empty())
			{
				if (!App->GetResourceState("script"))
				{
					App->GetResource("script", [=](const std::string& File)
					{
						Base->SetSource(Components::Scriptable::SourceType_Resource, File);
					});
				}
				else
					App->GetResource("", nullptr);
			}
			else
				Base->UnsetSource();

			return Active;
		});
		if (!Base->GetSource().empty() && ReactInspectorButton(Target, Button, "Recompile"))
		{
			App->VM->ClearCache();
			Base->SetSource();
		}
	}
}
static void ReactInspectorEntity(GUI::Template* Target, Sandbox* App)
{
	Entity* Base = App->Selection.Entity;
	if (!Base)
		return;

	GUI::Tree* Tree = Target->Get<GUI::Tree>("tree");
	if (!Tree)
		return;

	GUI::Contextual* Contextual = Target->Get<GUI::Contextual>("contextual");
	if (!Contextual)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	GUI::Button* Button = Target->Get<GUI::Button>("button");
	if (!Button)
		return;

	for (auto It = Base->First(); It != Base->Last(); It++)
	{
		Contextual->Source.TriggerX = Tree->GetWidgetPosition().X;
		Contextual->Source.TriggerY = Tree->GetWidgetPosition().Y;
		Contextual->Source.TriggerWidth = Tree->GetWidgetWidth();
		Contextual->Source.TriggerHeight = Tree->GetWidgetHeight();
		Contextual->Source.Width = Contextual->Source.TriggerWidth * 0.5f;

		bool Removed = false;
		Target->Compose(Contextual, [&](bool Active)
		{
			if (!Active)
				return Active;

			Target->Compose(Layout, nullptr);
			if (ReactInspectorButton(Target, Button, "Toggle"))
			{
				It->second->SetActive(!It->second->IsActive());
				Contextual->Close();
			}

			if (ReactInspectorButton(Target, Button, "Remove"))
			{
				Base->RemoveComponent(It->second->GetId());
				Contextual->Close();
				Removed = true;
			}

			return Active;
		});

		if (Removed)
			break;

		Target->ComposeStateful(Tree, [=]()
		{
			Tree->Source.Text = App->GetPascal(It->second->IsActive() ? It->second->GetName() : std::string(It->second->GetName()) + " (inactive)");
		}, [=](bool Active)
		{
			if (Active)
			{
				Tree->Source.Selectable = false;
				ReactInspectorComponent(Target, App, It->second);
				Tree->Source.Selectable = true;
			}

			return Active;
		});
	}
}
static void ReactInspectorMaterialBack(GUI::Button* Target, Sandbox* App)
{
	App->SetSelection(Inspector_Materials);
}
static void ReactInspectorSettingsSave(GUI::Button* Target, Sandbox* App)
{
	SceneGraph::Desc& Conf = App->Scene->GetConf();
	Conf.Device = App->Renderer;
	Conf.Queue = App->Queue;

	App->Scene->Configure(Conf);
}
static void ReactInspectorMaterials(GUI::Template* Target, Sandbox* App)
{
	if (!App->Scene->GetMaterialCount())
		return;

	GUI::Button* Button = Target->Get<GUI::Button>("button");
	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");

	for (uint64_t i = 0; i < App->Scene->GetMaterialCount(); i++)
	{
		Material* Material = App->Scene->GetMaterialById(i);
		std::string Name = App->Scene->GetMaterialName(Material->Id);

		Target->Compose(Layout, nullptr);
		if (ReactInspectorButton(Target, Button, (Name.empty() ? "Unnamed" : Name) + " (" + std::to_string((int)Material->Id) + ')'))
		{
			App->SetSelection(Inspector_Material);
			App->Selection.Material = Material;
		}
	}
}
static void ReactInspectorImportModel(GUI::Template* Target, Sandbox* App)
{
	GUI::Checkbox* Checkbox = Target->Get<GUI::Checkbox>("checkbox");
	if (!Checkbox)
		return;

	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_CalcTangentSpace, "Compute tangent space");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_JoinIdenticalVertices, "Join identical vertices");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_Triangulate, "Triangulate");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_RemoveComponent, "Remove component");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_GenNormals, "Make normals");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_GenSmoothNormals, "Make smooth normals");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_SplitLargeMeshes, "Split large meshes");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_PreTransformVertices, "Pret ransform vertices");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_ValidateDataStructure, "Validate data structure");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_MakeLeftHanded, "Make left handed");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_ImproveCacheLocality, "Improve cache locality");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_RemoveRedundantMaterials, "Remove redundant materials");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_FixInfacingNormals, "Fix infacing normals");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_SortByPType, "Sort by type");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_RemoveDegenerates, "Remove degenerates");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_RemoveInstances, "Remove instances");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_RemoveInvalidData, "Remove invalid data");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_GenUVCoords, "Make UV");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_TransformUVCoords, "Transform UV");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_OptimizeMeshes, "Optimize meshes");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_OptimizeGraph, "Optimize graph");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_FlipUVs, "Flip UVs");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_FlipWindingOrder, "Flip winding order");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_SplitByBoneCount, "Split by bones");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_Debone, "Debone");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_GlobalScale, "Global scaled");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_ForceGenNormals, "Force make normals");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_EmbedTextures, "Embed textures");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_DropNormals, "Drop normals");
	ReactInspectorOpt(Target, Checkbox, &App->State.MeshImportOpts, FileProcessors::MeshOpt_GenBoundingBoxes, "Compute bounding boxes");
}
static void ReactInspectorImportModelAction(GUI::Button* Target, Sandbox* App)
{
	std::string From;
	if (!OS::WantFileOpen("Import mesh", App->Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d", "", false, &From))
		return;

	if (!OS::FileExists(From.c_str()))
		return;

	FileProcessors::ModelProcessor* Processor = App->Content->GetProcessor<Model>()->As<FileProcessors::ModelProcessor>();
	if (Processor != nullptr)
	{
		Document* Doc = Processor->Import(From, App->State.MeshImportOpts);
		if (Doc != nullptr)
		{
			std::string To;
			if (!OS::WantFileSave("Save mesh", App->Content->GetEnvironment(), "*.xml,*.json,*.tmv", "Serialized mesh (*.xml, *.json, *.tmv)", &To))
				return;

			ContentMap Args;
			if (Stroke(&To).EndsWith(".tmv"))
				Args["BIN"] = ContentKey(true);
			else if (Stroke(&To).EndsWith(".json"))
				Args["JSON"] = ContentKey(true);
			else
				Args["XML"] = ContentKey(true);

			App->Content->Save<Document>(To, Doc, &Args);
			delete Doc;

			App->UpdateHierarchy();
			App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Mesh was imported");
		}
		else
			App->Activity->Message.Setup(AlertType_Error, "Sandbox", "Mesh is unsupported");
	}
	else
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Mesh cannot be imported");

	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactInspectorImportAnimation(GUI::Template* Target, Sandbox* App)
{
	ReactInspectorImportModel(Target, App);
}
static void ReactInspectorImportAnimationAction(GUI::Button* Target, Sandbox* App)
{
	std::string From;
	if (!OS::WantFileOpen("Import animation from mesh", App->Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d", "", false, &From))
		return;

	if (!OS::FileExists(From.c_str()))
		return;

	FileProcessors::SkinModelProcessor* Processor = App->Content->GetProcessor<Model>()->As<FileProcessors::SkinModelProcessor>();
	if (Processor != nullptr)
	{
		Document* Doc = Processor->ImportAnimation(From, App->State.MeshImportOpts);
		if (Doc != nullptr)
		{
			std::string To;
			if (!OS::WantFileSave("Save animation", App->Content->GetEnvironment(), "*.xml,*.json,*.tsc", "Serialized skin animation (*.xml, *.json, *.tsc)", &To))
				return;

			ContentMap Args;
			if (Stroke(&To).EndsWith(".tsc"))
				Args["BIN"] = ContentKey(true);
			else if (Stroke(&To).EndsWith(".json"))
				Args["JSON"] = ContentKey(true);
			else
				Args["XML"] = ContentKey(true);

			App->Content->Save<Document>(To, Doc, &Args);
			delete Doc;

			App->UpdateHierarchy();
			App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Animation was imported");
		}
		else
			App->Activity->Message.Setup(AlertType_Error, "Sandbox", "Animation is unsupported");
	}
	else
		App->Activity->Message.Setup(AlertType_Info, "Sandbox", "Animation cannot be imported");

	App->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
	App->Activity->Message.Result(nullptr);
}
static void ReactContentHierarchyItem(GUI::Template* Target, Sandbox* App, FileTree* Asset)
{
	std::string Path = Asset->Path;
	App->GetPathName(Path);

	GUI::Tree* Tree = Target->Get<GUI::Tree>("tree");
	if (!Tree)
		return;

	Target->ComposeStateful(Tree, [=]()
	{
		Tree->Source.Text = Path;
		Tree->SetValue(Asset == App->Selection.Folder);
	}, [=](bool Active)
	{
		if (Tree->IsClicked())
		{
			if (App->Selection.Folder != Asset)
				App->Selection.Folder = Asset;
			else
				App->Selection.Folder = nullptr;
		}

		if (Active)
		{
			for (auto& It : Asset->Directories)
				ReactContentHierarchyItem(Target, App, It);
		}

		return Active;
	});
}
static void ReactContentHierarchy(GUI::Template* Target, Sandbox* App)
{
	if (!App->State.Asset)
		return;

	for (auto& It : App->State.Asset->Directories)
		ReactContentHierarchyItem(Target, App, It);
}
static void ReactContentAssets(GUI::Template* Target, Sandbox* App)
{
	if (!App->Selection.Folder)
		return;

	GUI::Selectable* Selectable = Target->Get<GUI::Selectable>("selectable");
	if (!Selectable)
		return;

	GUI::Layout* Layout = Target->Get<GUI::Layout>("layout");
	if (!Layout)
		return;

	Target->Compose(Layout, nullptr);
	for (auto& It : App->Selection.Folder->Files)
	{
		Selectable->Source.Text = It;
		Selectable->SetValue(false);

		App->GetPathName(Selectable->Source.Text);
		App->State.Resource = It;

		Target->Compose(Selectable, [=](bool Active)
		{
			if (!Active)
				return false;

			if (App->State.Resource.empty() || !App->State.OnResource)
				return true;

			auto Callback = App->State.OnResource;
			auto Resource = App->State.Resource;
			App->GetResource("__got__", nullptr);

			Callback(Resource);
			return true;
		});
	}
}
static void ReactLogsWindow(GUI::Template* Target, Sandbox* App)
{
	GUI::Text* Text = Target->Get<GUI::Text>("text");
	if (!Text)
		return;

	Vector4 Color = Text->Source.Color;
	for (auto& Message : App->State.Logs)
	{
		if (Message.second == 1)
			Text->Source.Color = Vector4(200, 60, 60, 255);
		else if (Message.second == 2)
			Text->Source.Color = Vector4(200, 200, 60, 255);
		else
			Text->Source.Color = Vector4(175, 175, 175, 255);

		ReactInspectorText(Target, Text, Message.first);
	}

	Text->Source.Color = Color;
}
#ifdef _DEBUG
static void __spawn_cubes__(int Width, int Height, int Depth)
{
	SceneGraph* Scene = Sandbox::Get()->Scene;
	if (!Scene)
		return;

	auto* Models = Scene->GetComponents<Components::Model>();
	for (auto It = Models->Begin(); It != Models->End(); It++)
	{
		Scene->RemoveEntity((*It)->GetEntity(), true);
		It--;
	}

	Model* Mesh = Sandbox::Get()->Content->Load<Model>("./models/cube/mesh.xml", nullptr);
	for (int x = -Width; x < Width + 1; x++)
	{
		for (int y = -Height; y < Height + 1; y++)
		{
			for (int z = -Depth; z < Depth + 1; z++)
			{
				Entity* New = new Entity(Scene);
				New->Transform->Position.X = x * 2;
				New->Transform->Position.Y = y * 2;
				New->Transform->Position.Z = z * 2 - Depth;

				auto* Model = New->AddComponent<Components::Model>();
				Model->SetDrawable(Mesh);

				auto* Surface = Model->GetSurface(Mesh->Meshes.front());
				Surface->TexCoord = Vector2(2, 2);
				Surface->Material = 0;

				Scene->AddEntity(New);
			}
		}
	}
}
#endif

Demo::Demo(Application::Desc* Conf) : Application(Conf)
{
}
Demo::~Demo()
{
}
void Demo::WindowEvent(WindowState NewState, int X, int Y)
{
	switch (NewState)
	{
	case WindowState_Resize:
		Renderer->ResizeBuffers((unsigned int)X, (unsigned int)Y);
		if (Scene != nullptr)
			Scene->ResizeBuffers();
		break;
	case WindowState_Close:
		Activity->Message.Setup(AlertType_Warning, "Demo", "Do you want to go back to sandbox?");
		Activity->Message.Button(AlertConfirm_Escape, "No", 1);
		Activity->Message.Button(AlertConfirm_Return, "Yes", 2);
		Activity->Message.Result([this](int Button)
		{
			if (Button == 2)
				Restate(ApplicationState_Terminated);
		});
		break;
	default:
		break;
	}
}
void Demo::ScriptHook(VMGlobal* Global)
{
#ifdef _DEBUG
	Global->SetFunction("void __spawn_cubes__(int, int, int)", &__spawn_cubes__);
#endif
}
void Demo::Initialize(Application::Desc* Conf)
{
	Scene = Content->Load<SceneGraph>(Source, nullptr);
	if (!Scene)
		return Restate(ApplicationState_Terminated);

	Scene->ScriptHook();
	Enqueue([this](Timer* Time) { Scene->Synchronize(Time); });
	Enqueue([this](Timer* Time) { Scene->Simulation(Time); });
}
void Demo::Update(Timer* Time)
{
	Scene->Update(Time);
}
void Demo::Render(Timer* Time)
{
	Renderer->Clear(0, 0, 0);
	Renderer->ClearDepth();

	Scene->Render(Time);
	Scene->Submit();
	
	Renderer->Submit();
}
void Demo::SetSource(const std::string& Resource)
{
	Source = Resource;
}
std::string& Demo::GetSource()
{
	return Source;
}
std::string Demo::Source;

Sandbox::Sandbox(Application::Desc* Conf) : Application(Conf)
{
	Debug::AttachCallback([this](const char* Buffer, int Level)
	{
		if (State.Logs.size() + 1 > 22)
			State.Logs.erase(State.Logs.end() - 1);

		Stroke Result = Stroke(Buffer).ReplaceOf("\r\n", "");
		Stroke::Settle Start = Result.Find(' ');

		if (Start.Found)
		{
			Start = Result.Find(' ', Start.End);
			if (Start.Found)
			{
				Stroke::Settle End = Result.Find(')');
				if (End.Found)
					Result.EraseOffsets(Start.Start, End.End);
			}
		}

		State.Logs.insert(State.Logs.begin(), std::make_pair(Result.R(), Level));
	});

	Resource.Gizmo[0] = nullptr;
	Resource.Gizmo[1] = nullptr;
	Resource.Gizmo[2] = nullptr;
	State.Asset = nullptr;
	State.Frames = 0.0f;
}
Sandbox::~Sandbox()
{
	Debug::DetachCallback();

	delete State.GUI;
	delete State.Asset;
	delete Resource.Layout;
	delete Resource.Style;
	delete (CGizmoTransformMove*)Resource.Gizmo[0];
	delete (CGizmoTransformRotate*)Resource.Gizmo[1];
	delete (CGizmoTransformScale*)Resource.Gizmo[2];
}
void Sandbox::KeyEvent(KeyCode Key, KeyMod, int, int, bool Pressed)
{
	if (Key == KeyCode_CURSORLEFT && Selection.Gizmo)
	{
		if (!Pressed)
		{
			Activity->Grab(false);
			Selection.Gizmo->OnMouseUp(State.Cursor.X, State.Cursor.Y);
		}
		else if (Selection.Gizmo->OnMouseDown(State.Cursor.X, State.Cursor.Y))
			Activity->Grab(true);
	}
}
void Sandbox::WindowEvent(WindowState NewState, int X, int Y)
{
	switch (NewState)
	{
		case WindowState_Resize:
			Renderer->ResizeBuffers((unsigned int)X, (unsigned int)Y);
			if (Scene != nullptr)
				Scene->ResizeBuffers();

			SetStatus("Buffer resize was submitted");
			break;
		case WindowState_Close:
			Activity->Message.Setup(AlertType_Warning, "Sandbox", "Do you really want to exit?");
			Activity->Message.Button(AlertConfirm_Escape, "No", 1);
			Activity->Message.Button(AlertConfirm_Return, "Yes", 2);
			Activity->Message.Result([this](int Button)
			{
				if (Button == 2)
					Restate(ApplicationState_Terminated);
			});
			break;
		default:
			break;
	}
}
void Sandbox::ScriptHook(VMGlobal* Global)
{
#ifdef _DEBUG
	Global->SetFunction("void __spawn_cubes__(int, int, int)", &__spawn_cubes__);
#endif
}
void Sandbox::Initialize(Application::Desc* Conf)
{
	State.Camera = new ::Entity(nullptr);
	State.Camera->AddComponent<Components::Camera>();
	State.Camera->AddComponent<Components::FreeLook>();

	auto* Fly = State.Camera->AddComponent<Components::Fly>();
	Fly->SpeedDown *= 0.25f;
	Fly->SpeedNormal *= 0.35f;

	States.DepthStencil = Renderer->GetDepthStencilState("DEF_NONE");
	States.NoneRasterizer = Renderer->GetRasterizerState("DEF_CULL_NONE");
	States.BackRasterizer = Renderer->GetRasterizerState("DEF_CULL_BACK");
	States.Blend = Renderer->GetBlendState("DEF_ADDITIVE");
	States.Sampler = Renderer->GetSamplerState("DEF_TRILINEAR_X16");
	Icons.Empty = Content->Load<Texture2D>("system/empty.png", nullptr);
	Icons.Animation = Content->Load<Texture2D>("system/animation.png", nullptr);
	Icons.Body = Content->Load<Texture2D>("system/body.png", nullptr);
	Icons.Camera = Content->Load<Texture2D>("system/camera.png", nullptr);
	Icons.Decal = Content->Load<Texture2D>("system/decal.png", nullptr);
	Icons.Mesh = Content->Load<Texture2D>("system/mesh.png", nullptr);
	Icons.Motion = Content->Load<Texture2D>("system/motion.png", nullptr);
	Icons.Light = Content->Load<Texture2D>("system/light.png", nullptr);
	Icons.Probe = Content->Load<Texture2D>("system/probe.png", nullptr);
	Icons.Listener = Content->Load<Texture2D>("system/listener.png", nullptr);
	Icons.Source = Content->Load<Texture2D>("system/source.png", nullptr);
	Icons.Emitter = Content->Load<Texture2D>("system/emitter.png", nullptr);
	Resource.CurrentPath = Content->GetEnvironment();
	Resource.NextPath = Demo::GetSource();
	Resource.Layout = Content->Load<Document>("system/layout.xml", nullptr);
	Resource.Style = Content->Load<Document>("system/style.xml", nullptr);
	Resource.Gizmo[0] = CreateMoveGizmo();
	Resource.Gizmo[1] = CreateRotateGizmo();
	Resource.Gizmo[2] = CreateScaleGizmo();
	Resource.Gizmo[1]->SetAxisMask(IGizmo::AXIS_X | IGizmo::AXIS_Y | IGizmo::AXIS_Z);
	State.IsInteractive = true;
	State.IsPathTracked = false;
	State.IsCameraActive = true;
	State.IsTraceMode = false;
	State.IsDraggable = false;
	State.IsDragHovered = false;
	State.Asset = new FileTree(Resource.CurrentPath);
	State.GUI = new GUI::Context(Renderer, Activity);
	State.Draggable = nullptr;
	State.Label = nullptr;
	State.GizmoScale = 1.25f;
	State.Status = "Ready";
	State.ElementsLimit = 1024;
	State.MeshImportOpts =
		FileProcessors::MeshOpt_CalcTangentSpace |
		FileProcessors::MeshOpt_GenSmoothNormals |
		FileProcessors::MeshOpt_JoinIdenticalVertices |
		FileProcessors::MeshOpt_ImproveCacheLocality |
		FileProcessors::MeshOpt_LimitBoneWeights |
		FileProcessors::MeshOpt_RemoveRedundantMaterials |
		FileProcessors::MeshOpt_SplitLargeMeshes |
		FileProcessors::MeshOpt_Triangulate |
		FileProcessors::MeshOpt_GenUVCoords |
		FileProcessors::MeshOpt_SortByPType |
		FileProcessors::MeshOpt_RemoveDegenerates |
		FileProcessors::MeshOpt_RemoveInvalidData |
		FileProcessors::MeshOpt_RemoveInstances |
		FileProcessors::MeshOpt_ValidateDataStructure |
		FileProcessors::MeshOpt_OptimizeMeshes |
		FileProcessors::MeshOpt_TransformUVCoords | 0;
	Selection.Folder = nullptr;

	Demo::SetSource("");
	UpdateGUI();
	UpdateScene();

	Activity->Callbacks.CursorMove = [this](int X, int Y, int RX, int RY)
	{
		State.Cursor = Activity->GetCursorPosition();
		if (Selection.Gizmo)
			Selection.Gizmo->OnMouseMove(X, Y);
	};
	Activity->Show();
	Activity->Maximize();
	SetStatus("Setting up done");
}
void Sandbox::Render(Timer* Time)
{
	Renderer->Clear(0, 0, 0);
	Renderer->ClearDepth();
	Renderer->SetSamplerState(States.Sampler);

	Scene->Render(Time);
	if (State.IsInteractive && State.Camera == Scene->GetCamera()->GetEntity())
	{
		Scene->SetSurface();
		UpdateGrid(Time);
	}

	Scene->Submit();
	if (State.IsInteractive && State.GUI != nullptr)
		State.GUI->Render(Matrix4x4::Identity(), true);

	Renderer->Submit();
}
void Sandbox::Update(Timer* Time)
{
#ifdef _DEBUG
	if (Activity->IsKeyDownHit(KeyCode::KeyCode_F5))
	{
		if (State.GUI != nullptr)
		{
			Content->InvalidatePath("system/layout.xml");
			Content->InvalidatePath("system/style.xml");

			delete Resource.Style;
			Resource.Layout = Content->Load<Document>("system/layout.xml", nullptr);
			Resource.Style = Content->Load<Document>("system/style.xml", nullptr);

			State.Label = nullptr;
			State.GUI->Restore();

			UpdateGUI();
		}
	}

	if (Activity->IsKeyDownHit(KeyCode::KeyCode_F6))
		State.IsInteractive = !State.IsInteractive;
#endif

	if (State.IsInteractive)
	{
		State.Frames = (float)Time->GetFrameCount();
		if (!Resource.NextPath.empty())
			UpdateScene();

		if (State.GUI != nullptr)
			State.GUI->Prepare(Renderer->GetRenderTarget()->GetWidth(), Renderer->GetRenderTarget()->GetHeight() + 5);
	}

	Scene->Update(Time);
	Scene->Simulation(Time);
	Scene->Synchronize(Time);

	if (!State.IsCameraActive && Scene->GetCamera()->GetEntity() == State.Camera)
		State.IsCameraActive = true;

	State.Camera->GetComponent<Components::Fly>()->SetActive(GetSceneFocus());
	State.Camera->GetComponent<Components::FreeLook>()->SetActive(GetSceneFocus());
	if (!Scene->IsActive())
	{
		for (auto It = State.Camera->First(); It != State.Camera->Last(); It++)
			It->second->Update(Time);
	}

	if (!State.IsInteractive)
		return;

	if (Activity->IsKeyDownHit(KeyCode_F1))
	{
		State.IsTraceMode = !State.IsTraceMode;
		if (!State.IsTraceMode)
			SetStatus("Tracing mode: off");
		else
			SetStatus("Tracing mode: on");
	}

	if (!State.Camera || Scene->GetCamera()->As<Components::Camera>()->GetEntity() != State.Camera || !State.IsCameraActive)
		return;

	if (Selection.Entity != nullptr)
	{
		if (Activity->IsKeyDownHit(KeyCode::KeyCode_DELETE))
		{
			SetStatus("Entity was removed");
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection();
		}
	}

	if (Selection.Entity != nullptr)
	{
		if (GetSceneFocus())
		{
			if (Activity->IsKeyDownHit(KeyCode_1))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 0];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Movement gizmo selected");
			}
			else if (Activity->IsKeyDownHit(KeyCode_2))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 1];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Rotation gizmo selected");
			}
			else if (Activity->IsKeyDownHit(KeyCode_3))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 2];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Scale gizmo selected");
			}
		}

		if (Selection.Gizmo != nullptr)
		{
			Vector3 Position = State.Gizmo.Position();
			Vector3 Rotation = State.Gizmo.Rotation();
			Selection.Entity->Transform->Localize(&Position, nullptr, &Rotation);

			if (Selection.Gizmo->IsActive())
			{
				switch (Selection.Move)
				{
					case 1:
						Selection.Entity->Transform->GetLocalRotation()->Set(Rotation);
						break;
					case 2:
						Selection.Entity->Transform->GetLocalScale()->Set(State.Gizmo.Scale());
						break;
					default:
						Selection.Entity->Transform->GetLocalPosition()->Set(Position);
						break;
				}
				GetEntitySync();
			}
			else
			{
				State.Gizmo = Selection.Entity->Transform->GetWorld();
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
			}
		}
		else
		{
			State.Gizmo = Selection.Entity->Transform->GetWorld();
			Selection.Gizmo = Resource.Gizmo[Selection.Move];
			Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
			Selection.Gizmo->SetDisplayScale(State.GizmoScale);
			Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
		}
	}

	GetEntityCell();
	if (Selection.Entity != nullptr && Selection.Entity != State.Camera)
	{
		if (Activity->IsKeyDownHit(KeyCode::KeyCode_DELETE))
		{
			SetStatus("Entity was removed");
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection();
		}

		if (Activity->IsKeyDown(KeyMod_LCTRL) && GetSceneFocus())
		{
			if (Activity->IsKeyDownHit(KeyCode_V) || Activity->IsKeyDown(KeyCode_B))
			{
				SetStatus("Entity was cloned");
				Entity* Entity = Scene->CloneEntities(Selection.Entity);
				SetSelection(Entity ? Inspector_Entity : Inspector_None);
				Selection.Entity = Entity;
			}

			if (Activity->IsKeyDownHit(KeyCode_X))
			{
				SetStatus("Entity was removed");
				Scene->RemoveEntity(Selection.Entity, true);
				SetSelection();
			}
		}
	}

	if (Activity->IsKeyDown(KeyMod_LCTRL))
	{
		if (Activity->IsKeyDown(KeyCode_N))
		{
			Entity* Last = Scene->GetLastEntity();
			if (Last != nullptr && Last != State.Camera)
			{
				if (Selection.Entity == Last)
					SetSelection();

				SetStatus("Last entity was removed");
				Scene->RemoveEntity(Last, true);
			}
		}

		if (Activity->IsKeyDown(KeyCode_X))
		{
			if (Selection.Entity != nullptr)
			{
				SetStatus("Entity was removed");
				Scene->RemoveEntity(Selection.Entity, true);
				SetSelection();
			}
		}
	}
}
void Sandbox::UpdateGUI()
{
	if (!Resource.Layout || !State.GUI)
		return;

	if (Resource.Style != nullptr)
		State.GUI->LoadClasses(Content, Resource.Style);

	GUI::Body* Layout = State.GUI->Load(Content, Resource.Layout);
	Layout->Query<GUI::Body>([this](GUI::Body* Target, bool Active)
	{
		ReactApplicationLoop(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::DrawLabel>("system.draggable", [this](GUI::DrawLabel* Target, bool Active)
	{
		if (State.IsDraggable)
			return true;

		State.IsDraggable = true;
		ReactApplicationDraggable(Target, this);
		State.IsDraggable = false;

		return false;
	});
	Layout->QueryById<GUI::Group>("preview.group", [this](GUI::Group* Target, bool Active)
	{
		Target->SetInputEvents([this](GUI::Widget* Target)
		{
			ReactPreviewGroupInputs(Target, this);
		});
		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.file-open", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuFileOpen(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.file-save", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuFileSave(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.file-cancel", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuFileCancel(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-add", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuEntityAdd(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-remove", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuEntityRemove(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-move", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuEntityMove(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-rotate", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuEntityRotate(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-scale", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuEntityScale(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-reposition", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalPosition());
			State.Gizmo = Selection.Entity->Transform->GetWorld();
			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-rerotate", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalRotation()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalRotation());
			State.Gizmo = Selection.Entity->Transform->GetWorld();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->Transform->Rotation.DepthDirection();

			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-combine", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalPosition());
			Selection.Entity->Transform->GetLocalRotation()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalRotation());
			State.Gizmo = Selection.Entity->Transform->GetWorld();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->Transform->Rotation.DepthDirection();

			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-reset-position", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(0);
			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-reset-rotation", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalRotation()->Set(0);
			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.entity-reset-scale", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalScale()->Set(1);
			GetEntitySync();
		}

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-skin-animator", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSkinAnimator(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-key-animator", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsKeyAnimator(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-emitter-animator", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEmitterAnimator(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-listener", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsListener(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-source", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSource(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-reverb-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsReverbEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-chorus-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsChorusEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-distortion-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsDistortionEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-echo-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEchoEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-flanger-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsFlangerEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-frequency-shifter-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsFrequencyShifterEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-vocal-morpher-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsVocalMorpherEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-pitch-shifter-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsPitchShifterEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-ring-modulator-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsRingModulatorEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-autowah-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsAutowahEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-compressor-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsCompressorEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-equalizer-effect", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEqualizerEffect(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-model", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsModel(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-model", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidModel(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-skin", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSkin(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-skin", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidSkin(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-emitter", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEmitter(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-emitter", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidEmitter(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-decal", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsDecal(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-decal", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidDecal(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-point-light", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsPointLight(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-spot-light", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSpotLight(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-line-light", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLineLight(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-reflection-probe", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsReflectionProbe(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-rigid-body", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsRigidBody(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-soft-body", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSoftBody(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-soft-body", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidSoftBody(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-slider-constraint", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSliderConstraint(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-acceleration", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsAcceleration(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-fly", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsFly(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-free-look", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsFreeLook(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-camera", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsCamera(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-3d-camera", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponents3DCamera(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-scriptable", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsScriptable(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-model-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsModelRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-skin-model-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsSkinRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-element-system-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEmitterRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-depth-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsDepthRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-light-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLightRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-probe-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsProbeRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-limpid-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsLimpidRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-reflections-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsReflectionsRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-emission-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsEmissionRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-glitch-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsGlitchRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-dof-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsDOFRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-ao-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsAORenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-do-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsDORenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-tone-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsToneRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.components-gui-renderer", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactMenuComponentsGUIRenderer(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-add-material", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneAddMaterial(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-import-model", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneImportModel(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-import-skin-animation", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneImportSkinAnimation(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-export-skin-animation", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneExportSkinAnimation(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-export-key-animation", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneExportKeyAnimation(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-import-material", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneImportMaterial(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-export-material", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneExportMaterial(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-materials", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneMaterials(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-settings", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active)
			ReactSceneSettings(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::HeaderItem>("menu.scene-demo", [this](GUI::HeaderItem* Target, bool Active)
	{
		if (Active && !Resource.ScenePath.empty())
			ReactSceneDemo(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Template>("menu.logs-window", [this](GUI::Template* Target, bool Active)
	{
		ReactLogsWindow(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Group>("hierarchy.group", [this](GUI::Group* Target, bool Active)
	{
		Target->SetInputEvents([this](GUI::Widget* Target)
		{
			ReactHierarchyGroupInputs(Target, this);
		});
		return Active;
	});
	Layout->QueryById<GUI::Template>("hierarchy.view", [this](GUI::Template* Target, bool Active)
	{
		ReactHierarchyView(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Button>("hierarchy.toggle", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
			ReactHierarchyToggle(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Edit>("inspector.entity-name", [this](GUI::Edit* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (!Target->IsTextActive())
			Target->SetValue(Selection.Entity->Name);
		else if (Active)
			Selection.Entity->Name = Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Vector>("inspector.entity-position", [this](GUI::Vector* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
		{
			*Selection.Entity->Transform->GetLocalPosition() = Target->GetValue().XYZ();
			GetEntitySync();
		}
		else
			Target->SetValue(Selection.Entity->Transform->GetLocalPosition()->XYZW());

		return Active;
	});
	Layout->QueryById<GUI::Vector>("inspector.entity-scale", [this](GUI::Vector* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
		{
			*Selection.Entity->Transform->GetLocalScale() = Target->GetValue().XYZ();
			GetEntitySync();
		}
		else
			Target->SetValue(Selection.Entity->Transform->GetLocalScale()->XYZW());

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.entity-rotation-x", [this](GUI::Property* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
		{
			Selection.Entity->Transform->GetLocalRotation()->X = Math<float>::Deg2Rad() * Target->GetValue();
			GetEntitySync();
		}
		else
			Target->SetValue(Math<float>::Rad2Deg() * Selection.Entity->Transform->GetLocalRotation()->X);

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.entity-rotation-y", [this](GUI::Property* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
		{
			Selection.Entity->Transform->GetLocalRotation()->Y = Math<float>::Deg2Rad() * Target->GetValue();
			GetEntitySync();
		}
		else
			Target->SetValue(Math<float>::Rad2Deg() * Selection.Entity->Transform->GetLocalRotation()->Y);

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.entity-rotation-z", [this](GUI::Property* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
		{
			Selection.Entity->Transform->GetLocalRotation()->Z = Math<float>::Deg2Rad() * Target->GetValue();
			GetEntitySync();
		}
		else
			Target->SetValue(Math<float>::Rad2Deg() * Selection.Entity->Transform->GetLocalRotation()->Z);

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.entity-tag", [this](GUI::Property* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (Active)
			Selection.Entity->Tag = (uint64_t)Target->GetValue();
		else
			Target->SetValue((float)Selection.Entity->Tag);

		return Active;
	});
	Layout->QueryById<GUI::Checkbox>("inspector.entity-constant-scale", [this](GUI::Checkbox* Target, bool Active)
	{
		if (!Selection.Entity)
			return Active;

		if (!Active)
			Target->SetValue(Selection.Entity->Transform->ConstantScale);
		else
			Selection.Entity->Transform->ConstantScale = Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Template>("inspector.entity", [this](GUI::Template* Target, bool Active)
	{
		ReactInspectorEntity(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::ColorPicker>("inspector.material-emission", [this](GUI::ColorPicker* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
		{
			float W = Selection.Material->Emission.W;
			Selection.Material->Emission = Target->GetValue();
			Selection.Material->Emission.W = W;
		}
		else
			Target->SetValue(Selection.Material->Emission.SetW(1.0f));

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-emission-intensity", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Emission.W = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Emission.W);

		return Active;
	});
	Layout->QueryById<GUI::ColorPicker>("inspector.material-metallic", [this](GUI::ColorPicker* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
		{
			float W = Selection.Material->Metallic.W;
			Selection.Material->Metallic = Target->GetValue();
			Selection.Material->Metallic.W = W;
		}
		else
			Target->SetValue(Selection.Material->Metallic.SetW(1.0f));

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-metallic-intensity", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Metallic.W = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Metallic.W);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-f-roughness", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Roughness.X = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Roughness.X);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-t-roughness", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Roughness.Y = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Roughness.Y);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-f-occlusion", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Occlusion.X = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Occlusion.X);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-t-occlusion", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Occlusion.Y = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Occlusion.Y);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-fresnel", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Fresnel = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Fresnel);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-refraction", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Refraction = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Refraction);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-limpidity", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Limpidity = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Limpidity);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-environment", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Environment = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Environment);

		return Active;
	});
	Layout->QueryById<GUI::Slider>("inspector.material-radius", [this](GUI::Slider* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Selection.Material->Radius = Target->GetValue();
		else
			Target->SetValue(Selection.Material->Radius);

		return Active;
	});
	Layout->QueryById<GUI::Edit>("inspector.material-name", [this](GUI::Edit* Target, bool Active)
	{
		if (!Selection.Material)
			return Active;

		if (Active)
			Scene->SetMaterialName(Selection.Material->Id, Target->GetValue());
		else
			Target->SetValue(Scene->GetMaterialName(Selection.Material->Id));

		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.material-remove", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
		{
			if (Selection.Material != nullptr)
				Scene->RemoveMaterial((uint64_t)Selection.Material->Id);

			SetSelection(Inspector_Materials);
		}

		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.material-copy", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
		{
			if (Selection.Material != nullptr)
				Selection.Material = Scene->AddMaterial(Scene->GetMaterialName(Selection.Material->Id) + "*", *Selection.Material);
		}

		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.material-back", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
			ReactInspectorMaterialBack(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Vector>("inspector.settings-startup-gravity", [this](GUI::Vector* Target, bool Active)
	{
		if (!Active)
		{
			Vector3 Value = Scene->GetConf().Simulator.Gravity;
			Target->SetValue(Vector4(Value.X, Value.Y, Value.Z));
		}
		else
			Scene->GetConf().Simulator.Gravity = Target->GetValue().XYZ();

		return Active;
	});
	Layout->QueryById<GUI::Vector>("inspector.settings-startup-water-normal", [this](GUI::Vector* Target, bool Active)
	{
		if (!Active)
		{
			Vector3 Value = Scene->GetConf().Simulator.WaterNormal;
			Target->SetValue(Vector4(Value.X, Value.Y, Value.Z));
		}
		else
			Scene->GetConf().Simulator.WaterNormal = Target->GetValue().XYZ();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-startup-air-density", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().Simulator.AirDensity);
		else
			Scene->GetConf().Simulator.AirDensity = (float)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-startup-water-density", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().Simulator.WaterDensity);
		else
			Scene->GetConf().Simulator.WaterDensity = (float)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-startup-max-displacement", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().Simulator.MaxDisplacement);
		else
			Scene->GetConf().Simulator.MaxDisplacement = (float)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Checkbox>("inspector.settings-startup-enable-softbodies", [this](GUI::Checkbox* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().Simulator.EnableSoftBody);
		else
			Scene->GetConf().Simulator.EnableSoftBody = Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-renderer-render-quality", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().RenderQuality * 100.0);
		else
			Scene->GetConf().RenderQuality = (float)Target->GetValue() / 100.0f;

		return Active;
	});
	Layout->QueryById<GUI::Checkbox>("inspector.settings-renderer-enable-hdr", [this](GUI::Checkbox* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().EnableHDR);
		else
			Scene->GetConf().EnableHDR = Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-memory-max-entities", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().EntityCount);
		else
			Scene->GetConf().EntityCount = (uint64_t)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-memory-max-components", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetConf().ComponentCount);
		else
			Scene->GetConf().ComponentCount = (uint64_t)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Vector>("inspector.settings-simulator-gravity", [this](GUI::Vector* Target, bool Active)
	{
		if (!Active)
		{
			Vector3 Value = Scene->GetSimulator()->GetGravity();
			Target->SetValue(Vector4(Value.X, Value.Y, Value.Z));
		}
		else
			Scene->GetSimulator()->SetGravity(Target->GetValue().XYZ());

		return Active;
	});
	Layout->QueryById<GUI::Property>("inspector.settings-simulator-time", [this](GUI::Property* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetSimulator()->TimeSpeed);
		else
			Scene->GetSimulator()->TimeSpeed = (float)Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Checkbox>("inspector.settings-simulator-enabled", [this](GUI::Checkbox* Target, bool Active)
	{
		if (!Active)
			Target->SetValue(Scene->GetSimulator()->Active);
		else
			Scene->GetSimulator()->Active = Target->GetValue();

		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.settings-save", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
			ReactInspectorSettingsSave(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Template>("inspector.materials", [this](GUI::Template* Target, bool Active)
	{
		ReactInspectorMaterials(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Template>("inspector.import-model", [this](GUI::Template* Target, bool Active)
	{
		ReactInspectorImportModel(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.import-model-action", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
			ReactInspectorImportModelAction(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Template>("inspector.import-animation", [this](GUI::Template* Target, bool Active)
	{
		ReactInspectorImportAnimation(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Button>("inspector.import-animation-action", [this](GUI::Button* Target, bool Active)
	{
		if (Active)
			ReactInspectorImportAnimationAction(Target, this);

		return Active;
	});
	Layout->QueryById<GUI::Template>("content.hierarchy", [this](GUI::Template* Target, bool Active)
	{
		ReactContentHierarchy(Target, this);
		return Active;
	});
	Layout->QueryById<GUI::Template>("content.assets", [this](GUI::Template* Target, bool Active)
	{
		ReactContentAssets(Target, this);
		return Active;
	});

	SetStatus("GUI events setup done");
}
void Sandbox::UpdateHierarchy()
{
	SetStatus("Project's hierarchy was updated");

	std::string Folder = "";
	if (Selection.Folder != nullptr)
	{
		Folder = Selection.Folder->Path;
		Selection.Folder = nullptr;
	}

	delete State.Asset;
	State.Asset = new FileTree(Resource.CurrentPath);

	if (!Folder.empty())
		Selection.Folder = State.Asset->Find(Folder);
}
void Sandbox::UpdateScene()
{
	UpdateHierarchy();
	SetSelection();

	State.IsCameraActive = true;
	if (State.Asset != nullptr)
		Selection.Folder = State.Asset;

	if (Scene != nullptr)
	{
		Scene->RemoveEntity(State.Camera, false);
		delete Scene;
		Scene = nullptr;
	}

	VM->ClearCache();
	if (!Resource.NextPath.empty())
	{
		ContentMap Args;
		Args["active"] = ContentKey(false);

		Scene = Content->Load<SceneGraph>(Resource.NextPath, &Args);
	}

	if (Scene == nullptr)
	{
		SceneGraph::Desc I;
		I.Device = Renderer;
		I.Queue = Queue;
		I.Cache = Shaders;
		I.Manager = VM;
		I.Simulator.EnableSoftBody = true;
		Scene = new SceneGraph(I);
		Scene->SetActive(false);

		SetStatus("Created new scene from scratch");
	}
	else
		SetStatus("Scene was loaded");

	Scene->AddEntity(State.Camera);
	Scene->SetCamera(State.Camera);
	Scene->GetRenderer()->AddRenderer<Renderers::DepthRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::ProbeRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::ModelRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::SkinRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::SoftBodyRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::EmitterRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::DecalRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::LightRenderer>();
	Scene->GetRenderer()->AddRenderer<Renderers::LimpidRenderer>();
	Resource.ScenePath = Resource.NextPath;
	Resource.NextPath.clear();
}
void Sandbox::UpdateGrid(Timer* Time)
{
	Matrix4x4 Origin[4] =
	{
		Matrix4x4::Identity(),
		Matrix4x4::CreateTranslation({ 0, 0, 2 }),
		Matrix4x4::Create({ 0, 0, 0 }, 1,{ 0, Math<float>::Deg2Rad() * (90), 0 }),
		Matrix4x4::Create({ -2, 0, 0 }, 1,{ 0, Math<float>::Deg2Rad() * (90), 0 })
	};

	Renderer->SetShader(Renderer->GetBasicEffect(), ShaderType_Vertex | ShaderType_Pixel);
	Renderer->SetDepthStencilState(States.DepthStencil);
	Renderer->SetBlendState(States.Blend);

	for (uint32_t i = 0; i < Scene->GetEntityCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (State.Camera == Value)
			continue;

		float Direction = -Value->Transform->Position.LookAtXZ(State.Camera->Transform->Position);
		Renderer->Render.Diffuse = (Value == Selection.Entity) ? 0.5f : 0.05f;
		Renderer->Render.WorldViewProjection = Matrix4x4::Create(Value->Transform->Position, 0.5f, Vector3(0, Direction)) * State.Camera->GetComponent<Components::Camera>()->GetViewProjection();
		Renderer->SetTexture2D(GetIcon(Value), 0);
		Renderer->UpdateBuffer(RenderBufferType_Render);
		Renderer->Draw(6, 0);
	}

	if (Selection.Gizmo != nullptr)
	{
		Renderer->SetRasterizerState(States.NoneRasterizer);
		Vector2 Size = Activity->GetSize();
		Selection.Gizmo->SetScreenDimension(Size.X, Size.Y);
		Selection.Gizmo->SetCameraMatrix(Scene->GetCamera()->As<Components::Camera>()->GetView().Row, Scene->GetCamera()->As<Components::Camera>()->GetProjection().Row);
		Selection.Gizmo->Draw();
		Renderer->SetRasterizerState(States.BackRasterizer);
	}

	for (uint32_t i = 0; i < Scene->GetEntityCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (State.Camera == Value)
			continue;

		if (!State.IsTraceMode)
		{
			if (Value != Selection.Entity && Value != Selection.Might)
				continue;
		}

		if (State.IsPathTracked)
		{
			auto* KeyAnimator = Value->GetComponent<Components::KeyAnimator>();
			if (KeyAnimator != nullptr)
			{
				std::vector<AnimatorKey>* Keys = KeyAnimator->GetClip(KeyAnimator->State.Clip);
				if (Keys != nullptr)
				{
					Matrix4x4 Offset = (Value->Transform->GetRoot() ? Value->Transform->GetRoot()->GetWorld() : Matrix4x4::Identity());
					for (size_t j = 0; j < Keys->size(); j++)
					{
						Renderer->Begin();
						Renderer->Topology(PrimitiveTopology_Line_Strip);
						Renderer->Transform(Offset * Scene->GetCamera()->As<Components::Camera>()->GetViewProjection());
						Renderer->Emit();
						Renderer->Position(Keys->at(j).Position.X, Keys->at(j).Position.Y, -Keys->at(j).Position.Z);
						Renderer->Emit();

						if (KeyAnimator->State.Frame == j)
							Renderer->Color(1, 0, 1, 1);

						if (j + 1 >= Keys->size())
							Renderer->Position(Keys->at(0).Position.X, Keys->at(0).Position.Y, -Keys->at(0).Position.Z);
						else
							Renderer->Position(Keys->at(j + 1).Position.X, Keys->at(j + 1).Position.Y, -Keys->at(j + 1).Position.Z);

						Renderer->End();
					}
				}
			}
		}

		auto* Skin = Value->GetComponent<Components::Skin>();
		if (Skin != nullptr && Skin->GetDrawable() != nullptr)
		{
			for (auto& Joint : Skin->GetDrawable()->Joints)
			{
				Matrix4x4 Offset = Value->Transform->GetWorldUnscaled();
				UpdateJoint(&Skin->Skeleton, &Joint, &Offset);
			}
		}

		Matrix4x4 Transform;
		if (Skin != nullptr)
			Transform = Value->GetComponent<Components::Skin>()->GetBoundingBox();
		else if (Value->GetComponent<Components::Model>())
			Transform = Value->GetComponent<Components::Model>()->GetBoundingBox();
		else
			Transform = Value->Transform->GetWorld();

		for (int j = 0; j < 4; j++)
		{
			Renderer->Begin();
			Renderer->Topology(PrimitiveTopology_Line_Strip);
			Renderer->Transform(Origin[j] * Transform * Scene->GetCamera()->As<Components::Camera>()->GetViewProjection());
			Renderer->Emit();
			if (Value == Selection.Might)
				Renderer->Color(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->Color(1, 1, 1, 0.1f);
			else
				Renderer->Color(1, 1, 1, 0.5f);
			Renderer->Position(1, 1, 1);

			Renderer->Emit();
			if (Value == Selection.Might)
				Renderer->Color(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->Color(1, 1, 1, 0.1f);
			else
				Renderer->Color(1, 1, 1, 0.5f);
			Renderer->Position(-1, 1, 1);

			Renderer->Emit();
			if (Value == Selection.Might)
				Renderer->Color(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->Color(1, 1, 1, 0.1f);
			else
				Renderer->Color(1, 1, 1, 0.5f);
			Renderer->Position(-1, -1, 1);

			Renderer->Emit();
			if (Value == Selection.Might)
				Renderer->Color(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->Color(1, 1, 1, 0.1f);
			else
				Renderer->Color(1, 1, 1, 0.5f);
			Renderer->Position(1, -1, 1);

			Renderer->Emit();
			if (Value == Selection.Might)
				Renderer->Color(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->Color(1, 1, 1, 0.1f);
			else
				Renderer->Color(1, 1, 1, 0.5f);
			Renderer->Position(1, 1, 1);
			Renderer->End();
		}
	}

	if (Selection.Material != nullptr && Scene->GetMaterialById((uint64_t)Selection.Material->Id) != nullptr && Selection.Window == Inspector_Material)
	{
		Matrix4x4 ViewProjection = Scene->GetCamera()->As<Components::Camera>()->GetViewProjection();
		for (uint32_t i = 0; i < Scene->GetEntityCount(); i++)
		{
			Entity* Entity = Scene->GetEntity(i);
			int64_t MaterialId = -1;

			if (Entity->GetComponent<Components::Model>() || Entity->GetComponent<Components::LimpidModel>())
			{
				auto* It = Entity->GetComponent<Components::Model>();
				if (!It)
					It = Entity->GetComponent<Components::LimpidModel>();

				for (auto& Face : It->GetSurfaces())
				{
					if (Face.second.Material != (uint64_t)Selection.Material->Id)
						continue;

					MaterialId = Face.second.Material;
					break;
				}
			}
			else if (Entity->GetComponent<Components::Skin>() || Entity->GetComponent<Components::LimpidSkin>())
			{
				auto* It = Entity->GetComponent<Components::Skin>();
				if (!It)
					It = Entity->GetComponent<Components::LimpidSkin>();

				for (auto& Face : It->GetSurfaces())
				{
					if (Face.second.Material != (uint64_t)Selection.Material->Id)
						continue;

					MaterialId = Face.second.Material;
					break;
				}
			}
			else if (Entity->GetComponent<Components::Emitter>())
				MaterialId = Entity->GetComponent<Components::Emitter>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::LimpidEmitter>())
				MaterialId = Entity->GetComponent<Components::LimpidEmitter>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::SoftBody>())
				MaterialId = Entity->GetComponent<Components::SoftBody>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::LimpidSoftBody>())
				MaterialId = Entity->GetComponent<Components::LimpidSoftBody>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::Decal>())
				MaterialId = Entity->GetComponent<Components::Decal>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::LimpidDecal>())
				MaterialId = Entity->GetComponent<Components::LimpidDecal>()->GetSurface()->Material;

			if (MaterialId < 0 || MaterialId != (int64_t)Selection.Material->Id)
				continue;

			for (const auto& j : Origin)
			{
				Renderer->Begin();
				Renderer->Topology(PrimitiveTopology_Line_Strip);
				Renderer->Transform(j * Scene->GetEntity(i)->Transform->GetWorld() * ViewProjection);
				Renderer->Emit();
				Renderer->Color(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->Position(1.0f, 1.0f, 1.0f);
				Renderer->Emit();
				Renderer->Color(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->Position(-1.0f, 1.0f, 1.0f);
				Renderer->Emit();
				Renderer->Color(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->Position(-1.0f, -1.0f, 1.0f);
				Renderer->Emit();
				Renderer->Color(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->Position(1.0f, -1.0f, 1.0f);
				Renderer->Emit();
				Renderer->Color(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->Position(1.0f, 1.0f, 1.0f);
				Renderer->End();
			}
		}
	}
}
void Sandbox::UpdateJoint(PoseBuffer* Map, Joint* Base, Matrix4x4* World)
{
	PoseNode* Node = Map->GetNode(Base->Index);
	if (!Node || true)
		return;

	Vector3 Position1 = World->Position();
	*World = *World * Map->GetOffset(Node);
	Vector3 Position2 = Node->Position.Transform(*World);

	Renderer->Begin();
	Renderer->Topology(PrimitiveTopology_Line_Strip);
	Renderer->Transform(Scene->GetCamera()->As<Components::Camera>()->GetViewProjection());
	Renderer->Emit();
	Renderer->Position(Position1.X, Position1.Y, -Position1.Z);
	Renderer->Emit();
	Renderer->Position(Position2.X, Position2.Y, -Position2.Z);
	Renderer->End();

	for (auto& Child : Base->Childs)
		UpdateJoint(Map, &Child, World);
}
void Sandbox::SetSelection(Inspector Window)
{
	Selection.Might = nullptr;
	Selection.Entity = nullptr;
	Selection.Material = nullptr;
	Selection.Gizmo = nullptr;
	Selection.Move = 0;
	Selection.Window = Window;

	switch (Window)
	{
		case Inspector_None:
			SetStatus("Selection lost");
			break;
		case Inspector_Entity:
			SetStatus("Entity was selected");
			break;
		case Inspector_Material:
			SetStatus("Material was selected");
			break;
		case Inspector_Settings:
			SetStatus("Settings were selected");
			break;
		case Inspector_Materials:
			SetStatus("Materials were selected");
			break;
		case Inspector_ImportModel:
			SetStatus("Model import was selected");
			break;
		case Inspector_ImportAnimation:
			SetStatus("Animation import was selected");
			break;
	}
}
void Sandbox::SetStatus(const std::string& Status)
{
	State.Status = Status + '.';
	TH_LOG("%s", State.Status.c_str());
}
void Sandbox::GetPathName(std::string& Path)
{
	int64_t Distance = 0;
	if (Path.back() == '/' || Path.back() == '\\')
		Path.erase(Path.size() - 1);

	for (int64_t i = Path.size(); i > 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			Distance = i + 1;
			break;
		}
	}

	if (Distance > 0)
		Path.erase(0, Distance);
}
void Sandbox::GetEntityCell()
{
	if (!GetSceneFocus() || (Selection.Gizmo && Selection.Gizmo->IsActive()))
		return;

	auto* Camera = State.Camera->GetComponent<Components::Camera>();
	bool WantChange = Activity->IsKeyDownHit(KeyCode_CURSORLEFT);
	Ray Cursor = Camera->GetScreenRay(Activity->GetCursorPosition());
	float Distance = -1.0f;
	Entity* Current = nullptr;

	for (uint32_t i = 0; i < Scene->GetEntityCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (Selection.Entity == Value || Value == State.Camera)
			continue;

		float Far = Camera->GetDistance(Value);
		if (Distance > 0.0f && Distance < Far)
			continue;

		Matrix4x4 Transform;
		if (Value->GetComponent<Components::PointLight>() ||
			Value->GetComponent<Components::SpotLight>() ||
			Value->GetComponent<Components::ReflectionProbe>())
			Transform = Value->Transform->GetWorldUnscaled();
		else if (Value->GetComponent<Components::Model>())
			Transform = Value->GetComponent<Components::Model>()->GetBoundingBox();
		else if (Value->GetComponent<Components::Skin>())
			Transform = Value->GetComponent<Components::Skin>()->GetBoundingBox();
		else
			Transform = Value->Transform->GetWorld();

		if (Camera->RayTest(Cursor, Transform))
		{
			Current = Value;
			Distance = Far;
		}
	}

	if (Current != nullptr)
	{
		if (!WantChange)
		{
			if (Selection.Entity != Current)
				Selection.Might = Current;
		}
		else
		{
			SetSelection(Inspector_Entity);
			Selection.Entity = Current;
		}
	}
	else
	{
		Selection.Might = nullptr;
		if (WantChange)
			SetSelection();
	}
}
void Sandbox::GetEntitySync()
{
	Selection.Entity->Transform->Synchronize();

	Components::RigidBody* RigidBody = Selection.Entity->GetComponent<Components::RigidBody>();
	if (RigidBody != nullptr)
	{
		RigidBody->SetTransform(true);
		if (RigidBody->GetBody())
		{
			RigidBody->GetBody()->SetAngularVelocity(0);
			RigidBody->GetBody()->SetLinearVelocity(0);
		}
	}

	Components::SoftBody* SoftBody = Selection.Entity->GetComponent<Components::SoftBody>();
	if (SoftBody != nullptr)
	{
		SoftBody->SetTransform(true);
		if (SoftBody->GetBody())
			SoftBody->GetBody()->SetVelocity(0);
	}

	Components::PointLight* PointLight = Selection.Entity->GetComponent<Components::PointLight>();
	if (PointLight != nullptr)
		PointLight->GetEntity()->Transform->GetLocalScale()->Set(PointLight->GetRange());

	Components::SpotLight* SpotLight = Selection.Entity->GetComponent<Components::SpotLight>();
	if (SpotLight != nullptr)
		SpotLight->GetEntity()->Transform->GetLocalScale()->Set(SpotLight->GetRange());

	Components::ReflectionProbe* ReflectionProbe = Selection.Entity->GetComponent<Components::ReflectionProbe>();
	if (ReflectionProbe != nullptr)
		ReflectionProbe->GetEntity()->Transform->GetLocalScale()->Set(ReflectionProbe->GetRange());

	Components::Decal* Decal = Selection.Entity->GetComponent<Components::Decal>();
	if (Decal != nullptr)
		Decal->GetEntity()->Transform->GetLocalScale()->Set(Decal->GetRange());

	Components::LimpidDecal* LimpidDecal = Selection.Entity->GetComponent<Components::LimpidDecal>();
	if (LimpidDecal != nullptr)
		LimpidDecal->GetEntity()->Transform->GetLocalScale()->Set(LimpidDecal->GetRange());

}
void Sandbox::GetResource(const std::string& Name, const std::function<void(const std::string&)>& Callback)
{
	if (Name == "__got__")
	{
		SetStatus("Resource selected");

		State.Resource.clear();
		State.OnResource = nullptr;
		State.Filename.clear();
		return;
	}

	State.Resource.clear();
	if (Name.empty() || !Callback)
	{
		SetStatus("Resource selection cancelled");

		State.OnResource = nullptr;
		State.Filename.clear();
		return;
	}

	UpdateHierarchy();
	SetStatus("Awaiting " + Name + " resource selection");

	State.OnResource = Callback;
	State.Filename = Name;
}
void Sandbox::GetEntity(const std::string& Name, const std::function<void(Entity*)>& Callback)
{
	if (Name == "__got__")
	{
		SetStatus("Entity selected");

		State.Draggable = nullptr;
		State.OnEntity = nullptr;
		State.Target.clear();
		return;
	}

	State.Draggable = nullptr;
	if (Name.empty() || !Callback)
	{
		SetStatus("Entity selection cancelled");

		State.OnEntity = nullptr;
		State.Target.clear();
		return;
	}

	UpdateHierarchy();
	SetStatus("Awaiting " + Name + " entity selection");

	State.OnEntity = Callback;
	State.Target = Name;
}
void* Sandbox::GetGUI()
{
	return State.GUI;
}
bool Sandbox::GetSceneFocus()
{
	return State.IsSceneFocused && !State.GUI->HasOverflow();
}
bool Sandbox::GetResourceState(const std::string& Name)
{
	return State.Filename == Name;
}
bool Sandbox::GetEntityState(const std::string& Name)
{
	return State.Target == Name;
}
bool Sandbox::GetSelectionState()
{
	return Selection.Entity || Selection.Material;
}
Texture2D* Sandbox::GetIcon(Entity* Value)
{
	if (Value->GetComponent<Components::Camera>())
		return Icons.Camera;

	if (Value->GetComponent<Components::Model>() ||
		Value->GetComponent<Components::LimpidModel>() ||
		Value->GetComponent<Components::Skin>() ||
		Value->GetComponent<Components::LimpidSkin>())
		return Icons.Mesh;

	if (Value->GetComponent<Components::Emitter>() ||
		Value->GetComponent<Components::LimpidEmitter>())
		return Icons.Emitter;

	if (Value->GetComponent<Components::Decal>() ||
		Value->GetComponent<Components::LimpidDecal>())
		return Icons.Decal;

	if (Value->GetComponent<Components::PointLight>() ||
		Value->GetComponent<Components::SpotLight>() ||
		Value->GetComponent<Components::LineLight>())
		return Icons.Light;

	if (Value->GetComponent<Components::ReflectionProbe>())
		return Icons.Probe;

	if (Value->GetComponent<Components::AudioListener>())
		return Icons.Listener;

	if (Value->GetComponent<Components::AudioSource>())
		return Icons.Source;

	if (Value->GetComponent<Components::RigidBody>() ||
		Value->GetComponent<Components::SoftBody>())
		return Icons.Body;

	if (Value->GetComponent<Components::SliderConstraint>() ||
		Value->GetComponent<Components::Acceleration>())
		return Icons.Motion;

	if (Value->GetComponent<Components::KeyAnimator>() ||
		Value->GetComponent<Components::SkinAnimator>() ||
		Value->GetComponent<Components::EmitterAnimator>())
		return Icons.Animation;

	return Icons.Empty;
}
std::string Sandbox::GetLabel(Entity* Value)
{
	if (Value->GetComponent<Components::Camera>())
		return "[Camera]";

	if (Value->GetComponent<Components::Model>())
		return "[Model]";

	if (Value->GetComponent<Components::LimpidModel>())
		return "[Limpid model]";

	if (Value->GetComponent<Components::Skin>())
		return "[Skin]";

	if (Value->GetComponent<Components::LimpidSkin>())
		return "[Limpid skin]";

	if (Value->GetComponent<Components::Emitter>())
		return "[Emitter]";

	if (Value->GetComponent<Components::LimpidEmitter>())
		return "[Limpid emitter]";

	if (Value->GetComponent<Components::Decal>())
		return "[Decal]";

	if (Value->GetComponent<Components::LimpidDecal>())
		return "[Limpid decal]";

	if (Value->GetComponent<Components::PointLight>())
		return "[Point light]";

	if (Value->GetComponent<Components::SpotLight>())
		return "[Spot light]";

	if (Value->GetComponent<Components::LineLight>())
		return "[Line light]";

	if (Value->GetComponent<Components::ReflectionProbe>())
		return "[Reflection probe]";

	if (Value->GetComponent<Components::AudioListener>())
		return "[Audio listener]";

	if (Value->GetComponent<Components::AudioSource>())
		return "[Audio source]";

	if (Value->GetComponent<Components::RigidBody>())
		return "[Rigid body]";

	if (Value->GetComponent<Components::SoftBody>())
		return "[Soft body]";

	if (Value->GetComponent<Components::SliderConstraint>())
		return "[Slider constraint]";

	if (Value->GetComponent<Components::Acceleration>())
		return "[Acceleration]";

	if (Value->GetComponent<Components::KeyAnimator>())
		return "[Key animator]";

	if (Value->GetComponent<Components::SkinAnimator>())
		return "[Skin animator]";

	if (Value->GetComponent<Components::EmitterAnimator>())
		return "[Emitter animator]";

	if (Value->GetComponent<Components::Fly>())
		return "[Fly]";

	if (Value->GetComponent<Components::FreeLook>())
		return "[Free look]";

	if (Value->GetComponent<Components::Scriptable>())
		return "[Scriptable]";

	return "[Empty]";
}
std::string Sandbox::GetName(Entity* Value)
{
	std::string Result;
	if (Value == nullptr)
		Result = "[Empty] Any";
	else if (Value->Name.empty())
		Result = GetLabel(Value);
	else
		Result = GetLabel(Value) + " " + Value->Name;

	auto* Scriptable = Value->GetComponent<Components::Scriptable>();
	if (Scriptable != nullptr && !Scriptable->GetSource().empty())
	{
		const std::string& Module = Scriptable->GetName();
		Result += " " + (Module.empty() ? "anonymous" : Module);
	}
	else if (Value->Name.empty())
		Result += " Any (" + std::to_string(Value->Id) + ")";

	return Result;
}
std::string Sandbox::GetPascal(const std::string& Value)
{
	std::string Result;
	Result.reserve(Value.size() * 2);

	for (auto& Char : Value)
	{
		if (!Result.empty() && isupper(Char))
			Result.append(1, ' ').append(1, tolower(Char));
		else
			Result.append(1, Char);
	}

	return Result;
}