#include "sandbox.h"
#include "demo.h"
#include "../controller/resolvers.h"
#include "../controller/components.h"
#include "../controller/renderers.h"
#include "../controller/effects.h"

Sandbox::Sandbox(Application::Desc* Conf) : Application(Conf)
{
}
Sandbox::~Sandbox()
{
	TH_RELEASE(State.GUI);
	TH_RELEASE(State.Directory);
	TH_RELEASE(Icons.Empty);
	TH_RELEASE(Icons.Animation);
	TH_RELEASE(Icons.Body);
	TH_RELEASE(Icons.Camera);
	TH_RELEASE(Icons.Decal);
	TH_RELEASE(Icons.Mesh);
	TH_RELEASE(Icons.Motion);
	TH_RELEASE(Icons.Light);
	TH_RELEASE(Icons.Probe);
	TH_RELEASE(Icons.Listener);
	TH_RELEASE(Icons.Source);
	TH_RELEASE(Icons.Emitter);
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
			Activity->SetGrabbing(false);
			Selection.Gizmo->OnMouseUp(State.Cursor.X, State.Cursor.Y);
		}
		else if (Selection.Gizmo->OnMouseDown(State.Cursor.X, State.Cursor.Y))
			Activity->SetGrabbing(true);
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
}
void Sandbox::Initialize(Application::Desc* Conf)
{
	State.Camera = new ::Entity(nullptr);
	State.Camera->AddComponent<Components::Camera>();
	State.Camera->AddComponent<Components::FreeLook>();

	auto* Fly = State.Camera->AddComponent<Components::Fly>();
	Fly->SpeedDown *= 0.25f;
	Fly->SpeedNormal *= 0.35f;

	States.DepthStencil = Renderer->GetDepthStencilState("none");
	States.NoneRasterizer = Renderer->GetRasterizerState("cull-none");
	States.BackRasterizer = Renderer->GetRasterizerState("cull-back");
	States.Blend = Renderer->GetBlendState("additive");
	States.Sampler = Renderer->GetSamplerState("trilinear-x16");
	Icons.Empty = Content->Load<Texture2D>("system/img/empty.png");
	Icons.Animation = Content->Load<Texture2D>("system/img/animation.png");
	Icons.Body = Content->Load<Texture2D>("system/img/body.png");
	Icons.Camera = Content->Load<Texture2D>("system/img/camera.png");
	Icons.Decal = Content->Load<Texture2D>("system/img/decal.png");
	Icons.Mesh = Content->Load<Texture2D>("system/img/mesh.png");
	Icons.Motion = Content->Load<Texture2D>("system/img/motion.png");
	Icons.Light = Content->Load<Texture2D>("system/img/light.png");
	Icons.Probe = Content->Load<Texture2D>("system/img/probe.png");
	Icons.Listener = Content->Load<Texture2D>("system/img/listener.png");
	Icons.Source = Content->Load<Texture2D>("system/img/source.png");
	Icons.Emitter = Content->Load<Texture2D>("system/img/emitter.png");
	Resource.CurrentPath = Content->GetEnvironment();
	Resource.NextPath = Demo::GetSource();
	Resource.Gizmo[0] = CreateMoveGizmo();
	Resource.Gizmo[1] = CreateRotateGizmo();
	Resource.Gizmo[2] = CreateScaleGizmo();
	Resource.Gizmo[1]->SetAxisMask(IGizmo::AXIS_X | IGizmo::AXIS_Y | IGizmo::AXIS_Z);
	State.IsInteractive = true;
	State.IsPathTracked = false;
	State.IsCameraActive = true;
	State.IsTraceMode = false;
	State.IsDraggable = false;
	State.IsOutdated = false;
	State.IsDragHovered = false;
	State.IsCaptured = false;
	State.Directory = nullptr;
	State.GUI = new GUI::Context(Renderer);
	State.GUI->SetMountCallback([this](GUI::Context*)
	{
		SetViewModel();
	});
	State.GUI->Inject("system/conf.xml");
	State.Draggable = nullptr;
	State.GizmoScale = 1.25f;
	State.Status = "Ready";
	State.ElementsLimit = 1024;
	State.MeshImportOpts =
		Processors::MeshOpt_CalcTangentSpace |
		Processors::MeshOpt_GenSmoothNormals |
		Processors::MeshOpt_JoinIdenticalVertices |
		Processors::MeshOpt_ImproveCacheLocality |
		Processors::MeshOpt_LimitBoneWeights |
		Processors::MeshOpt_RemoveRedundantMaterials |
		Processors::MeshOpt_SplitLargeMeshes |
		Processors::MeshOpt_Triangulate |
		Processors::MeshOpt_GenUVCoords |
		Processors::MeshOpt_SortByPType |
		Processors::MeshOpt_RemoveDegenerates |
		Processors::MeshOpt_RemoveInvalidData |
		Processors::MeshOpt_RemoveInstances |
		Processors::MeshOpt_ValidateDataStructure |
		Processors::MeshOpt_OptimizeMeshes |
		Processors::MeshOpt_TransformUVCoords | 0;
	Selection.Directory = nullptr;

	Demo::SetSource("");
	UpdateScene();
	UpdateProject();

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
		State.GUI->RenderLists(Renderer->GetRenderTarget()->GetTarget());

	Renderer->Submit();
}
void Sandbox::Update(Timer* Time)
{
#ifdef _DEBUG
	if (Activity->IsKeyDownHit(KeyCode::KeyCode_F5))
	{
#ifndef HTML_DEBUG
		if (State.GUI != nullptr)
		{
			State.GUI->Deconstruct();
			State.GUI->ClearCache();
			State.GUI->Inject("system/conf.xml");
		}
#endif
	}

	if (Activity->IsKeyDownHit(KeyCode::KeyCode_F6))
		State.IsInteractive = !State.IsInteractive;
#endif
	if (!State.IsCameraActive && Scene->GetCamera()->GetEntity() == State.Camera)
		State.IsCameraActive = true;

	State.Camera->GetComponent<Components::Fly>()->SetActive(GetSceneFocus());
	State.Camera->GetComponent<Components::FreeLook>()->SetActive(GetSceneFocus());
	if (!Scene->IsActive() && GetSceneFocus())
	{
		for (auto It = State.Camera->First(); It != State.Camera->Last(); It++)
			It->second->Update(Time);
	}

	if (State.IsInteractive)
	{
		State.Frames = (float)Time->GetFrameCount();
		if (!Resource.NextPath.empty())
			return UpdateScene();

		UpdateSystem();
		if (State.IsOutdated)
		{
			UpdateHierarchy();
			State.IsOutdated = false;
		}

		if (State.GUI != nullptr)
			State.GUI->UpdateEvents(Activity);
	}

	Scene->Update(Time);
	Scene->Simulation(Time);
	Scene->Synchronize(Time);

	if (State.IsCaptured)
	{
		State.IsCaptured = false;
		return;
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
			SetSelection(Inspector_None);
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
			SetSelection(Inspector_None);
		}

		if (Activity->IsKeyDown(KeyMod_LCTRL) && GetSceneFocus())
		{
			if (Activity->IsKeyDownHit(KeyCode_V) || Activity->IsKeyDown(KeyCode_B))
			{
				SetStatus("Entity was cloned");
				Entity* Entity = Scene->CloneEntities(Selection.Entity);
				SetSelection(Entity ? Inspector_Entity : Inspector_None, Entity);
			}

			if (Activity->IsKeyDownHit(KeyCode_X))
			{
				SetStatus("Entity was removed");
				Scene->RemoveEntity(Selection.Entity, true);
				SetSelection(Inspector_None);
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
					SetSelection(Inspector_None);

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
				SetSelection(Inspector_None);
			}
		}
	}
}
void Sandbox::UpdateHierarchy()
{
	SetStatus("Scene's hierarchy was updated");
	GUI::DataRow* Root = Models.Hierarchy->Get();
	Root->Update();
}
void Sandbox::UpdateProject()
{
	SetStatus("Project's hierarchy was updated");
	Models.Surfaces->Update(nullptr);

	std::string Directory = "";
	if (Selection.Directory != nullptr)
	{
		Directory = Selection.Directory->Path;
		Selection.Directory = nullptr;
	}

	TH_RELEASE(State.Directory);
	State.Directory = new FileTree(Resource.CurrentPath);

	GUI::DataRow* Root = Models.Project->Get();
	Root->SetTarget(State.Directory);

	if (!Directory.empty())
		SetDirectory(Models.Project->Get()->GetTarget<FileTree>()->Find(Directory));
}
void Sandbox::UpdateScene()
{
	SetSelection(Inspector_None);
	State.IsCameraActive = true;

	if (Scene != nullptr)
	{
		Scene->RemoveEntity(State.Camera, false);
		TH_CLEAR(Scene);
	}

	VM->ClearCache();
	if (!Resource.NextPath.empty())
	{
		PropertyArgs Args;
		Args["active"] = Property(false);

		Scene = Content->Load<SceneGraph>(Resource.NextPath, Args);
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

	Scene->AddEventListener("hierarachy", "mutation", [this](Event* Message)
	{
		State.IsOutdated = true;
	});

	Scene->AddEntity(State.Camera);
	Scene->SetCamera(State.Camera);
	Scene->GetRenderer()->AddRenderer<Renderers::Depth>();
	Scene->GetRenderer()->AddRenderer<Renderers::Environment>();
	Scene->GetRenderer()->AddRenderer<Renderers::Model>();
	Scene->GetRenderer()->AddRenderer<Renderers::Skin>();
	Scene->GetRenderer()->AddRenderer<Renderers::SoftBody>();
	Scene->GetRenderer()->AddRenderer<Renderers::Emitter>();
	Scene->GetRenderer()->AddRenderer<Renderers::Decal>();
	Scene->GetRenderer()->AddRenderer<Renderers::Lighting>();
	Scene->GetRenderer()->AddRenderer<Renderers::Transparency>();

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
	Renderer->SetRasterizerState(States.BackRasterizer);

	for (uint32_t i = 0; i < Scene->GetEntityCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (State.Camera == Value)
			continue;

		float Direction = -Value->Transform->Position.LookAtXZ(State.Camera->Transform->Position);
		Renderer->Render.Diffuse = (Value == Selection.Entity) ? 0.5f : 0.05f;
		Renderer->Render.WorldViewProjection = Matrix4x4::Create(Value->Transform->Position, 0.5f, Vector3(0, Direction)) * State.Camera->GetComponent<Components::Camera>()->GetViewProjection();
		Renderer->SetTexture2D(GetIcon(Value), 1);
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

			if (Entity->GetComponent<Components::Model>())
			{
				auto* It = Entity->GetComponent<Components::Model>();
				for (auto& Face : It->GetSurfaces())
				{
					if (Face.second.Material != (uint64_t)Selection.Material->Id)
						continue;

					MaterialId = Face.second.Material;
					break;
				}
			}
			else if (Entity->GetComponent<Components::Skin>())
			{
				auto* It = Entity->GetComponent<Components::Skin>();
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
			else if (Entity->GetComponent<Components::SoftBody>())
				MaterialId = Entity->GetComponent<Components::SoftBody>()->GetSurface()->Material;
			else if (Entity->GetComponent<Components::Decal>())
				MaterialId = Entity->GetComponent<Components::Decal>()->GetSurface()->Material;

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
void Sandbox::UpdateSystem()
{
	Models.System->SetInteger("files_count", Selection.Directory ? Selection.Directory->Files.size() : 0);
	Models.System->SetInteger("entities_count", Scene->HasEntity(State.Camera) ? Scene->GetEntityCount() - 1 : Scene->GetEntityCount());
	Models.System->SetInteger("materials_count", Scene->GetMaterialCount());
	Models.System->SetBoolean("scene_active", Scene->IsActive());
	Models.System->SetBoolean("sl_resource", !State.Filename.empty());
	Models.System->SetInteger("sl_material", Selection.Material ? Selection.Material->Id : -1);
	Models.System->SetInteger("sl_entity", Selection.Entity ? Selection.Entity->Id : -1);

	switch (Selection.Window)
	{
		case Inspector_Entity:
			Models.System->SetString("sl_window", "entity");
			InspectEntity();
			break;
		case Inspector_Material:
			Models.System->SetString("sl_window", "material");
			InspectMaterial();
			break;
		case Inspector_Settings:
			Models.System->SetString("sl_window", "settings");
			InspectSettings();
			break;
		case Inspector_Materials:
			Models.System->SetString("sl_window", "materials");
			break;
		case Inspector_ImportModel:
			Models.System->SetString("sl_window", "import-model");
			InspectImporter();
			break;
		case Inspector_ImportAnimation:
			Models.System->SetString("sl_window", "import-animation");
			InspectImporter();
			break;
		case Inspector_None:
		default:
			Models.System->SetString("sl_window", "none");
			break;
	}
}
void Sandbox::InspectEntity()
{
	Entity* Base = Selection.Entity;
	if (!Selection.Entity)
		return;

	if (State.GUI->GetElementById(0, "ent_name").CastFormString(&Base->Name))
		Models.Hierarchy->Update(Base);

	if (State.GUI->GetElementById(0, "ent_pos_x").CastFormFloat(&Base->Transform->GetLocalPosition()->X) ||
		State.GUI->GetElementById(0, "ent_pos_y").CastFormFloat(&Base->Transform->GetLocalPosition()->Y) ||
		State.GUI->GetElementById(0, "ent_pos_z").CastFormFloat(&Base->Transform->GetLocalPosition()->Z) ||
		State.GUI->GetElementById(0, "ent_rot_x").CastFormFloat(&Base->Transform->GetLocalRotation()->X, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_rot_y").CastFormFloat(&Base->Transform->GetLocalRotation()->Y, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_rot_z").CastFormFloat(&Base->Transform->GetLocalRotation()->Z, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_scale_x").CastFormFloat(&Base->Transform->GetLocalScale()->X) ||
		State.GUI->GetElementById(0, "ent_scale_y").CastFormFloat(&Base->Transform->GetLocalScale()->Y) ||
		State.GUI->GetElementById(0, "ent_scale_z").CastFormFloat(&Base->Transform->GetLocalScale()->Z))
		GetEntitySync();

	State.GUI->GetElementById(0, "ent_tag").CastFormInt64(&Base->Tag);
	State.GUI->GetElementById(0, "ent_const_scale").CastFormBoolean(&Base->Transform->ConstantScale);

	if (Models.System->SetBoolean("sl_cmp_model", Base->GetComponent<Components::Model>() != nullptr)->GetBoolean())
		ComponentModel(State.GUI, Base->GetComponent<Components::Model>());

	if (Models.System->SetBoolean("sl_cmp_skin", Base->GetComponent<Components::Skin>() != nullptr)->GetBoolean())
		ComponentSkin(State.GUI, Base->GetComponent<Components::Skin>());

	if (Models.System->SetBoolean("sl_cmp_emitter", Base->GetComponent<Components::Emitter>() != nullptr)->GetBoolean())
		ComponentEmitter(State.GUI, Base->GetComponent<Components::Emitter>());

	if (Models.System->SetBoolean("sl_cmp_soft_body", Base->GetComponent<Components::SoftBody>() != nullptr)->GetBoolean())
		ComponentSoftBody(State.GUI, Base->GetComponent<Components::SoftBody>());

	if (Models.System->SetBoolean("sl_cmp_decal", Base->GetComponent<Components::Decal>() != nullptr)->GetBoolean())
		ComponentDecal(State.GUI, Base->GetComponent<Components::Decal>());

	if (Models.System->SetBoolean("sl_cmp_skin_animator", Base->GetComponent<Components::SkinAnimator>() != nullptr)->GetBoolean())
		ComponentSkinAnimator(State.GUI, Base->GetComponent<Components::SkinAnimator>());

	if (Models.System->SetBoolean("sl_cmp_key_animator", Base->GetComponent<Components::KeyAnimator>() != nullptr)->GetBoolean())
		ComponentKeyAnimator(State.GUI, Base->GetComponent<Components::KeyAnimator>());

	if (Models.System->SetBoolean("sl_cmp_emitter_animator", Base->GetComponent<Components::EmitterAnimator>() != nullptr)->GetBoolean())
		ComponentEmitterAnimator(State.GUI, Base->GetComponent<Components::EmitterAnimator>());

	if (Models.System->SetBoolean("sl_cmp_rigid_body", Base->GetComponent<Components::RigidBody>() != nullptr)->GetBoolean())
		ComponentRigidBody(State.GUI, Base->GetComponent<Components::RigidBody>());

	if (Models.System->SetBoolean("sl_cmp_acceleration", Base->GetComponent<Components::Acceleration>() != nullptr)->GetBoolean())
		ComponentAcceleration(State.GUI, Base->GetComponent<Components::Acceleration>());

	if (Models.System->SetBoolean("sl_cmp_slider_constraint", Base->GetComponent<Components::SliderConstraint>() != nullptr)->GetBoolean())
		ComponentSliderConstraint(State.GUI, Base->GetComponent<Components::SliderConstraint>());

	if (Models.System->SetBoolean("sl_cmp_free_look", Base->GetComponent<Components::FreeLook>() != nullptr)->GetBoolean())
		ComponentFreeLook(State.GUI, Base->GetComponent<Components::FreeLook>());

	if (Models.System->SetBoolean("sl_cmp_fly", Base->GetComponent<Components::Fly>() != nullptr)->GetBoolean())
		ComponentFly(State.GUI, Base->GetComponent<Components::Fly>());

	if (Models.System->SetBoolean("sl_cmp_audio_source", Base->GetComponent<Components::AudioSource>() != nullptr)->GetBoolean())
	{
		auto* Source = Base->GetComponent<Components::AudioSource>();
		Models.System->SetBoolean("sl_cmp_audio_source_compressor", Source->GetSource()->GetEffect<Effects::Compressor>() != nullptr);

		if (Models.System->SetBoolean("sl_cmp_audio_source_reverb", Source->GetSource()->GetEffect<Effects::Reverb>() != nullptr)->GetBoolean())
			EffectReverb(State.GUI, Source->GetSource()->GetEffect<Effects::Reverb>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_chorus", Source->GetSource()->GetEffect<Effects::Chorus>() != nullptr)->GetBoolean())
			EffectChorus(State.GUI, Source->GetSource()->GetEffect<Effects::Chorus>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_distortion", Source->GetSource()->GetEffect<Effects::Distortion>() != nullptr)->GetBoolean())
			EffectDistortion(State.GUI, Source->GetSource()->GetEffect<Effects::Distortion>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_echo", Source->GetSource()->GetEffect<Effects::Echo>() != nullptr)->GetBoolean())
			EffectEcho(State.GUI, Source->GetSource()->GetEffect<Effects::Echo>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_flanger", Source->GetSource()->GetEffect<Effects::Flanger>() != nullptr)->GetBoolean())
			EffectFlanger(State.GUI, Source->GetSource()->GetEffect<Effects::Flanger>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_frequency_shifter", Source->GetSource()->GetEffect<Effects::FrequencyShifter>() != nullptr)->GetBoolean())
			EffectFrequencyShifter(State.GUI, Source->GetSource()->GetEffect<Effects::FrequencyShifter>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_vocal_morpher", Source->GetSource()->GetEffect<Effects::VocalMorpher>() != nullptr)->GetBoolean())
			EffectVocalMorpher(State.GUI, Source->GetSource()->GetEffect<Effects::VocalMorpher>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_pitch_shifter", Source->GetSource()->GetEffect<Effects::PitchShifter>() != nullptr)->GetBoolean())
			EffectPitchShifter(State.GUI, Source->GetSource()->GetEffect<Effects::PitchShifter>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_ring_modulator", Source->GetSource()->GetEffect<Effects::RingModulator>() != nullptr)->GetBoolean())
			EffectRingModulator(State.GUI, Source->GetSource()->GetEffect<Effects::RingModulator>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_autowah", Source->GetSource()->GetEffect<Effects::Autowah>() != nullptr)->GetBoolean())
			EffectAutowah(State.GUI, Source->GetSource()->GetEffect<Effects::Autowah>());

		if (Models.System->SetBoolean("sl_cmp_audio_source_equalizer", Source->GetSource()->GetEffect<Effects::Equalizer>() != nullptr)->GetBoolean())
			EffectEqualizer(State.GUI, Source->GetSource()->GetEffect<Effects::Equalizer>());

		ComponentAudioSource(State.GUI, Source);
	}

	if (Models.System->SetBoolean("sl_cmp_audio_listener", Base->GetComponent<Components::AudioListener>() != nullptr)->GetBoolean())
		ComponentAudioListener(State.GUI, Base->GetComponent<Components::AudioListener>());

	if (Models.System->SetBoolean("sl_cmp_point_light", Base->GetComponent<Components::PointLight>() != nullptr)->GetBoolean())
		ComponentPointLight(State.GUI, Base->GetComponent<Components::PointLight>());

	if (Models.System->SetBoolean("sl_cmp_spot_light", Base->GetComponent<Components::SpotLight>() != nullptr)->GetBoolean())
		ComponentSpotLight(State.GUI, Base->GetComponent<Components::SpotLight>());

	if (Models.System->SetBoolean("sl_cmp_line_light", Base->GetComponent<Components::LineLight>() != nullptr)->GetBoolean())
		ComponentLineLight(State.GUI, Base->GetComponent<Components::LineLight>());

	if (Models.System->SetBoolean("sl_cmp_reflection_probe", Base->GetComponent<Components::ReflectionProbe>() != nullptr)->GetBoolean())
		ComponentReflectionProbe(State.GUI, Base->GetComponent<Components::ReflectionProbe>());

	if (Models.System->SetBoolean("sl_cmp_camera", Base->GetComponent<Components::Camera>() != nullptr)->GetBoolean())
	{
		auto* Camera = Base->GetComponent<Components::Camera>();
		Models.System->SetInteger("sl_cmp_camera_model", Camera->GetRenderer()->GetOffset<Renderers::Model>());
		Models.System->SetInteger("sl_cmp_camera_skin", Camera->GetRenderer()->GetOffset<Renderers::Skin>());
		Models.System->SetInteger("sl_cmp_camera_soft_body", Camera->GetRenderer()->GetOffset<Renderers::SoftBody>());
		Models.System->SetInteger("sl_cmp_camera_decal", Camera->GetRenderer()->GetOffset<Renderers::Decal>());
		Models.System->SetInteger("sl_cmp_camera_emitter", Camera->GetRenderer()->GetOffset<Renderers::Emitter>());
		Models.System->SetInteger("sl_cmp_camera_gui", Camera->GetRenderer()->GetOffset<Renderers::UserInterface>());
		Models.System->SetInteger("sl_cmp_camera_transparency", Camera->GetRenderer()->GetOffset<Renderers::Transparency>());

		if (Models.System->SetInteger("sl_cmp_camera_depth", Camera->GetRenderer()->GetOffset<Renderers::Depth>())->GetInteger() >= 0)
			RendererDepth(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Depth>());
		
		if (Models.System->SetInteger("sl_cmp_camera_lighting", Camera->GetRenderer()->GetOffset<Renderers::Lighting>())->GetInteger() >= 0)
			RendererLighting(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Lighting>());

		if (Models.System->SetInteger("sl_cmp_camera_environment", Camera->GetRenderer()->GetOffset<Renderers::Environment>())->GetInteger() >= 0)
			RendererEnvironment(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Environment>());

		if (Models.System->SetInteger("sl_cmp_camera_ssr", Camera->GetRenderer()->GetOffset<Renderers::SSR>())->GetInteger() >= 0)
			RendererSSR(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::SSR>());

		if (Models.System->SetInteger("sl_cmp_camera_ssao", Camera->GetRenderer()->GetOffset<Renderers::SSAO>())->GetInteger() >= 0)
			RendererSSAO(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::SSAO>());

		if (Models.System->SetInteger("sl_cmp_camera_ssdo", Camera->GetRenderer()->GetOffset<Renderers::SSDO>())->GetInteger() >= 0)
			RendererSSDO(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::SSDO>());

		if (Models.System->SetInteger("sl_cmp_camera_bloom", Camera->GetRenderer()->GetOffset<Renderers::Bloom>())->GetInteger() >= 0)
			RendererBloom(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Bloom>());

		if (Models.System->SetInteger("sl_cmp_camera_dof", Camera->GetRenderer()->GetOffset<Renderers::DoF>())->GetInteger() >= 0)
			RendererDoF(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::DoF>());

		if (Models.System->SetInteger("sl_cmp_camera_tone", Camera->GetRenderer()->GetOffset<Renderers::Tone>())->GetInteger() >= 0)
			RendererTone(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Tone>());

		if (Models.System->SetInteger("sl_cmp_camera_glitch", Camera->GetRenderer()->GetOffset<Renderers::Glitch>())->GetInteger() >= 0)
			RendererGlitch(State.GUI, Camera->GetRenderer()->GetRenderer<Renderers::Glitch>());

		ComponentCamera(State.GUI, Camera);
	}

	if (Models.System->SetBoolean("sl_cmp_scriptable", Base->GetComponent<Components::Scriptable>() != nullptr)->GetBoolean())
		ComponentScriptable(State.GUI, Base->GetComponent<Components::Scriptable>());
}
void Sandbox::InspectSettings()
{
	SceneGraph::Desc& Conf = Scene->GetConf();
	State.GUI->GetElementById(0, "sc_start_grav_x").CastFormFloat(&Conf.Simulator.Gravity.X);
	State.GUI->GetElementById(0, "sc_start_grav_y").CastFormFloat(&Conf.Simulator.Gravity.Y);
	State.GUI->GetElementById(0, "sc_start_grav_z").CastFormFloat(&Conf.Simulator.Gravity.Z);
	State.GUI->GetElementById(0, "sc_wet_norm_x").CastFormFloat(&Conf.Simulator.WaterNormal.X);
	State.GUI->GetElementById(0, "sc_wet_norm_y").CastFormFloat(&Conf.Simulator.WaterNormal.Y);
	State.GUI->GetElementById(0, "sc_wet_norm_z").CastFormFloat(&Conf.Simulator.WaterNormal.Z);
	State.GUI->GetElementById(0, "sc_air_dens").CastFormFloat(&Conf.Simulator.AirDensity);
	State.GUI->GetElementById(0, "sc_wet_dens").CastFormFloat(&Conf.Simulator.WaterDensity);
	State.GUI->GetElementById(0, "sc_wet_off").CastFormFloat(&Conf.Simulator.WaterOffset);
	State.GUI->GetElementById(0, "sc_max_disp").CastFormFloat(&Conf.Simulator.MaxDisplacement);
	State.GUI->GetElementById(0, "sc_softs").CastFormBoolean(&Conf.Simulator.EnableSoftBody);
	State.GUI->GetElementById(0, "sc_gp_qual").CastFormFloat(&Conf.RenderQuality, 100.0f);
	State.GUI->GetElementById(0, "sc_gp_hdr").CastFormBoolean(&Conf.EnableHDR);
	State.GUI->GetElementById(0, "sc_max_ents").CastFormUInt64(&Conf.EntityCount);
	State.GUI->GetElementById(0, "sc_max_comps").CastFormUInt64(&Conf.ComponentCount);

	Vector3 Gravity = Scene->GetSimulator()->GetGravity();
	if (State.GUI->GetElementById(0, "sc_sim_grav_x").CastFormFloat(&Gravity.X) ||
		State.GUI->GetElementById(0, "sc_sim_grav_y").CastFormFloat(&Gravity.Y) ||
		State.GUI->GetElementById(0, "sc_sim_grav_z").CastFormFloat(&Gravity.Z))
		Scene->GetSimulator()->SetGravity(Gravity);

	State.GUI->GetElementById(0, "sc_sim_time").CastFormFloat(&Scene->GetSimulator()->TimeSpeed);
	State.GUI->GetElementById(0, "sc_sim").CastFormBoolean(&Scene->GetSimulator()->Active);
}
void Sandbox::InspectImporter()
{
	State.GUI->GetElementById(0, "im_jiv").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_JoinIdenticalVertices);
	State.GUI->GetElementById(0, "im_tri").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_Triangulate);
	State.GUI->GetElementById(0, "im_cts").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_CalcTangentSpace);
	State.GUI->GetElementById(0, "im_rc").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_RemoveComponent);
	State.GUI->GetElementById(0, "im_mn").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_GenNormals);
	State.GUI->GetElementById(0, "im_msn").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_GenSmoothNormals);
	State.GUI->GetElementById(0, "im_slm").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_SplitLargeMeshes);
	State.GUI->GetElementById(0, "im_ptv").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_PreTransformVertices);
	State.GUI->GetElementById(0, "im_vds").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_ValidateDataStructure);
	State.GUI->GetElementById(0, "im_mlh").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_MakeLeftHanded);
	State.GUI->GetElementById(0, "im_icl").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_ImproveCacheLocality);
	State.GUI->GetElementById(0, "im_rrm").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_RemoveRedundantMaterials);
	State.GUI->GetElementById(0, "im_fin").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_FixInfacingNormals);
	State.GUI->GetElementById(0, "im_sbt").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_SortByPType);
	State.GUI->GetElementById(0, "im_rd").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_RemoveDegenerates);
	State.GUI->GetElementById(0, "im_ri").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_RemoveInstances);
	State.GUI->GetElementById(0, "im_rid").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_RemoveInvalidData);
	State.GUI->GetElementById(0, "im_muv").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_GenUVCoords);
	State.GUI->GetElementById(0, "im_tuv").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_TransformUVCoords);
	State.GUI->GetElementById(0, "im_om").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_OptimizeMeshes);
	State.GUI->GetElementById(0, "im_og").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_OptimizeGraph);
	State.GUI->GetElementById(0, "im_fuv").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_FlipUVs);
	State.GUI->GetElementById(0, "im_fwo").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_FlipWindingOrder);
	State.GUI->GetElementById(0, "im_sbb").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_SplitByBoneCount);
	State.GUI->GetElementById(0, "im_dbn").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_Debone);
	State.GUI->GetElementById(0, "im_gs").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_GlobalScale);
	State.GUI->GetElementById(0, "im_fmn").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_ForceGenNormals);
	State.GUI->GetElementById(0, "im_et").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_EmbedTextures);
	State.GUI->GetElementById(0, "im_dn").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_DropNormals);
	State.GUI->GetElementById(0, "im_cbb").CastFormFlag32(&State.MeshImportOpts, Processors::MeshOpt_GenBoundingBoxes);
}
void Sandbox::InspectMaterial()
{
	Material* Base = Selection.Material;
	if (!Selection.Material)
		return;

	std::string Name = Scene->GetMaterialName(Base->Id);
	if (State.GUI->GetElementById(0, "mat_name").CastFormString(&Name))
		Scene->SetMaterialName(Base->Id, Name);

	if (State.GUI->GetElementById(0, "mat_cemn").CastFormColor(&Base->Emission, false))
		State.GUI->GetElementById(0, "mat_cemn_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Emission.X * 255.0f), (unsigned int)(Base->Emission.Y * 255.0f), (unsigned int)(Base->Emission.Z * 255.0f)).R());

	if (State.GUI->GetElementById(0, "mat_cmet").CastFormColor(&Base->Metallic, false))
		State.GUI->GetElementById(0, "mat_cmet_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Metallic.X * 255.0f), (unsigned int)(Base->Metallic.Y * 255.0f), (unsigned int)(Base->Metallic.Z * 255.0f)).R());

	State.GUI->GetElementById(0, "mat_memn").CastFormFloat(&Base->Emission.W);
	State.GUI->GetElementById(0, "mat_mmet").CastFormFloat(&Base->Metallic.W);
	State.GUI->GetElementById(0, "mat_rs").CastFormFloat(&Base->Roughness.X);
	State.GUI->GetElementById(0, "mat_mrs").CastFormFloat(&Base->Roughness.Y);
	State.GUI->GetElementById(0, "mat_occ").CastFormFloat(&Base->Occlusion.X);
	State.GUI->GetElementById(0, "mat_mocc").CastFormFloat(&Base->Occlusion.Y);
	State.GUI->GetElementById(0, "mat_fres").CastFormFloat(&Base->Fresnel);
	State.GUI->GetElementById(0, "mat_refr").CastFormFloat(&Base->Refraction);
	State.GUI->GetElementById(0, "mat_alpha").CastFormFloat(&Base->Transparency);
	State.GUI->GetElementById(0, "mat_env").CastFormFloat(&Base->Environment);
	State.GUI->GetElementById(0, "mat_rad").CastFormFloat(&Base->Radius);
}
void Sandbox::SetViewModel()
{
	SetStatus("GUI view-model setup done");
	if (!State.GUI)
		return;

	Models.System = State.GUI->SetDataModel("system");
	Models.System->SetInteger("entities_count", 0);
	Models.System->SetInteger("materials_count", 0);
	Models.System->SetInteger("files_count", 0);
	Models.System->SetBoolean("scene_active", false);
	Models.System->SetString("sl_window", "none");
	Models.System->SetInteger("sl_entity", -1);
	Models.System->SetInteger("sl_material", -1);
	Models.System->SetBoolean("sl_resource", false);
	Models.System->SetBoolean("sl_cmp_model", false);
	Models.System->SetBoolean("sl_cmp_model_source", false);
	Models.System->SetBoolean("sl_cmp_model_assigned", false);
	Models.System->SetBoolean("sl_cmp_skin", false);
	Models.System->SetBoolean("sl_cmp_skin_source", false);
	Models.System->SetBoolean("sl_cmp_skin_assigned", false);
	Models.System->SetInteger("sl_cmp_skin_joint", -1);
	Models.System->SetInteger("sl_cmp_skin_joints", -1);
	Models.System->SetBoolean("sl_cmp_emitter", false);
	Models.System->SetBoolean("sl_cmp_soft_body", false);
	Models.System->SetBoolean("sl_cmp_soft_body_source", false);
	Models.System->SetBoolean("sl_cmp_decal", false);
	Models.System->SetBoolean("sl_cmp_skin_animator", false);
	Models.System->SetInteger("sl_cmp_skin_animator_joint", -1);
	Models.System->SetInteger("sl_cmp_skin_animator_joints", -1);
	Models.System->SetInteger("sl_cmp_skin_animator_frame", -1);
	Models.System->SetInteger("sl_cmp_skin_animator_frames", -1);
	Models.System->SetInteger("sl_cmp_skin_animator_clip", -1);
	Models.System->SetInteger("sl_cmp_skin_animator_clips", -1);
	Models.System->SetBoolean("sl_cmp_key_animator", false);
	Models.System->SetInteger("sl_cmp_key_animator_frame", -1);
	Models.System->SetInteger("sl_cmp_key_animator_frames", -1);
	Models.System->SetInteger("sl_cmp_key_animator_clip", -1);
	Models.System->SetInteger("sl_cmp_key_animator_clips", -1);
	Models.System->SetBoolean("sl_cmp_emitter_animator", false);
	Models.System->SetBoolean("sl_cmp_rigid_body", false);
	Models.System->SetBoolean("sl_cmp_rigid_body_source", false);
	Models.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
	Models.System->SetBoolean("sl_cmp_acceleration", false);
	Models.System->SetBoolean("sl_cmp_slider_constraint", false);
	Models.System->SetBoolean("sl_cmp_slider_constraint_entity", false);
	Models.System->SetBoolean("sl_cmp_free_look", false);
	Models.System->SetBoolean("sl_cmp_fly", false);
	Models.System->SetBoolean("sl_cmp_audio_source", false);
	Models.System->SetBoolean("sl_cmp_audio_source_clip", false);
	Models.System->SetFloat("sl_cmp_audio_source_length", 0.0f);
	Models.System->SetBoolean("sl_cmp_audio_source_reverb", false);
	Models.System->SetBoolean("sl_cmp_audio_source_chorus", false);
	Models.System->SetBoolean("sl_cmp_audio_source_distortion", false);
	Models.System->SetBoolean("sl_cmp_audio_source_echo", false);
	Models.System->SetBoolean("sl_cmp_audio_source_flanger", false);
	Models.System->SetBoolean("sl_cmp_audio_source_frequency_shifter", false);
	Models.System->SetBoolean("sl_cmp_audio_source_vocal_morpher", false);
	Models.System->SetBoolean("sl_cmp_audio_source_pitch_shifter", false);
	Models.System->SetBoolean("sl_cmp_audio_source_ring_modulator", false);
	Models.System->SetBoolean("sl_cmp_audio_source_autowah", false);
	Models.System->SetBoolean("sl_cmp_audio_source_compressor", false);
	Models.System->SetBoolean("sl_cmp_audio_source_equalizer", false);
	Models.System->SetBoolean("sl_cmp_audio_listener", false);
	Models.System->SetBoolean("sl_cmp_point_light", false);
	Models.System->SetBoolean("sl_cmp_spot_light", false);
	Models.System->SetBoolean("sl_cmp_line_light", false);
	Models.System->SetBoolean("sl_cmp_reflection_probe", false);
	Models.System->SetBoolean("sl_cmp_camera", false);
	Models.System->SetInteger("sl_cmp_camera_model", -1);
	Models.System->SetInteger("sl_cmp_camera_skin", -1);
	Models.System->SetInteger("sl_cmp_camera_soft_body", -1);
	Models.System->SetInteger("sl_cmp_camera_decal", -1);
	Models.System->SetInteger("sl_cmp_camera_emitter", -1);
	Models.System->SetInteger("sl_cmp_camera_depth", -1);
	Models.System->SetInteger("sl_cmp_camera_lighting", -1);
	Models.System->SetInteger("sl_cmp_camera_environment", -1);
	Models.System->SetInteger("sl_cmp_camera_transparency", -1);
	Models.System->SetInteger("sl_cmp_camera_ssr", -1);
	Models.System->SetInteger("sl_cmp_camera_ssao", -1);
	Models.System->SetInteger("sl_cmp_camera_ssdo", -1);
	Models.System->SetInteger("sl_cmp_camera_bloom", -1);
	Models.System->SetInteger("sl_cmp_camera_dof", -1);
	Models.System->SetInteger("sl_cmp_camera_tone", -1);
	Models.System->SetInteger("sl_cmp_camera_glitch", -1);
	Models.System->SetInteger("sl_cmp_camera_gui", -1);
	Models.System->SetBoolean("sl_cmp_scriptable", false);
	Models.System->SetBoolean("sl_cmp_scriptable_source", false);
	Models.System->SetCallback("set_parent", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		GUI::IElement Target = (Rml::Element*)Event.GetPointer("drag_element");
		if (!Target.IsValid())
			return;

		Entity* Child = Scene->GetEntity(atoi(Target.GetAttribute("entity-id").c_str()));
		if (!Child)
			return;

		if (Args.size() == 1)
		{
			Entity* Base = Scene->GetEntity(Args[0].GetInteger());
			if (Base != nullptr)
				Child->Transform->SetRoot(Base->Transform);
			else
				Child->Transform->SetRoot(nullptr);

			Event.StopImmediatePropagation();
		}
		else if (Args.empty())
			Child->Transform->SetRoot(nullptr);

		State.IsOutdated = true;
	});
	Models.System->SetCallback("set_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() != 1)
			return;

		Entity* Base = Scene->GetEntity(Args[0].GetInteger());
		if (State.OnEntity)
		{
			auto Callback = State.OnEntity;
			GetEntity("__got__", nullptr);

			Callback(Base);
		}
		else if (Selection.Entity != Base)
			SetSelection(Inspector_Entity, Base);
		else
			SetSelection(Inspector_None);
	});
	Models.System->SetCallback("set_controls", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() == 1)
			State.IsSceneFocused = Args[0].GetBoolean();
	});
	Models.System->SetCallback("set_menu", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() != 1)
			return;

		GUI::IElement Target = Event.GetCurrentElement();
		GUI::IElement Current = Event.GetTargetElement();
		bool Toggle = false;

		if (Target.GetElement() != Current.GetElement())
		{
			if (Event.GetType() != "click")
				return;

			if (Current.GetTagName() != "button")
			{
				if (Current.GetTagName() != "p")
					return;

				Toggle = true;
			}
		}

		std::vector<GUI::IElement> Tabs = Target.QuerySelectorAll(".tab");
		bool Enabled = Args[0].GetBoolean();

		for (auto& Tab : Tabs)
		{
			if (Toggle)
				Tab.SetClass("transfer", !Tab.IsClassSet("transfer"));
			else if (Enabled)
				Tab.SetClass("transfer", true);
			else
				Tab.SetClass("transfer", false);
		}
	});
	Models.System->SetCallback("set_directory", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetDirectory(Models.Project->Get()->GetTarget<FileTree>()->Find(Args[0].ToString()));
	});
	Models.System->SetCallback("set_file", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() != 1)
			return;

		std::string Resource = Args[0].ToString();
		if (Resource.empty() || !State.OnResource)
			return;

		auto Callback = State.OnResource;
		GetResource("__got__", nullptr);

		Callback(Resource);
	});
	Models.System->SetCallback("set_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() != 1)
			return;

		SetSelection(Inspector_Material, Scene->GetMaterialById(Args[0].GetInteger()));
	});
	Models.System->SetCallback("save_settings", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SceneGraph::Desc& Conf = Scene->GetConf();
		Conf.Device = Renderer;
		Conf.Queue = Queue;

		Scene->Configure(Conf);
	});
	Models.System->SetCallback("switch_scene", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Scene->IsActive())
		{
			PropertyArgs Args;
			Args["type"] = Property("XML");

			Scene->RemoveEntity(State.Camera, false);
			Content->Save<SceneGraph>("./system/cache.xml", Scene, Args);
			Scene->AddEntity(State.Camera);
			Scene->SetActive(true);
			Scene->ScriptHook();

			auto* Cameras = Scene->GetComponents<Components::Camera>();
			for (auto It = Cameras->Begin(); It != Cameras->End(); It++)
			{
				Components::Camera* Base = (*It)->As<Components::Camera>();
				if (Base->GetEntity() == State.Camera)
					continue;

				if (Base->IsActive())
				{
					Scene->SetCamera(Base->GetEntity());
					State.IsCameraActive = false;
					break;
				}
			}
		}
		else
		{
			this->Resource.NextPath = "./system/cache.xml";
			UpdateScene();
		}
	});
	Models.System->SetCallback("import_model_action", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		std::string From;
		if (!OS::WantFileOpen("Import mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d", "", false, &From))
			return;

		if (!OS::FileExists(From.c_str()))
			return;

		Processors::Model* Processor = Content->GetProcessor<Model>()->As<Processors::Model>();
		if (Processor != nullptr)
		{
			Document* Doc = Processor->Import(From, State.MeshImportOpts);
			if (Doc != nullptr)
			{
				std::string To;
				if (!OS::WantFileSave("Save mesh", Content->GetEnvironment(), "*.xml,*.json,*.tmv", "Serialized mesh (*.xml, *.json, *.tmv)", &To))
					return;

				PropertyArgs Args;
				if (Stroke(&To).EndsWith(".tmv"))
					Args["type"] = Property("BIN");
				else if (Stroke(&To).EndsWith(".json"))
					Args["type"] = Property("JSON");
				else
					Args["type"] = Property("XML");

				Content->Save<Document>(To, Doc, Args);
				TH_RELEASE(Doc);

				State.IsOutdated = true;
				this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Mesh was imported");
			}
			else
				this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Mesh is unsupported");
		}
		else
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Mesh cannot be imported");

		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("import_skin_animation_action", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		std::string From;
		if (!OS::WantFileOpen("Import animation from mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d", "", false, &From))
			return;

		if (!OS::FileExists(From.c_str()))
			return;

		Processors::SkinModel* Processor = Content->GetProcessor<Model>()->As<Processors::SkinModel>();
		if (Processor != nullptr)
		{
			Document* Doc = Processor->ImportAnimation(From, State.MeshImportOpts);
			if (Doc != nullptr)
			{
				std::string To;
				if (!OS::WantFileSave("Save animation", Content->GetEnvironment(), "*.xml,*.json,*.tsc", "Serialized skin animation (*.xml, *.json, *.tsc)", &To))
					return;

				PropertyArgs Args;
				if (Stroke(&To).EndsWith(".tsc"))
					Args["type"] = Property("BIN");
				else if (Stroke(&To).EndsWith(".json"))
					Args["type"] = Property("JSON");
				else
					Args["type"] = Property("XML");

				Content->Save<Document>(To, Doc, Args);
				TH_RELEASE(Doc);

				State.IsOutdated = true;
				this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Animation was imported");
			}
			else
				this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Animation is unsupported");
		}
		else
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Animation cannot be imported");

		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("update_hierarchy", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		State.IsOutdated = true;
	});
	Models.System->SetCallback("update_project", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		UpdateProject();
	});
	Models.System->SetCallback("remove_cmp", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Args.size() != 1)
			return;

		if (Selection.Entity != nullptr)
			Selection.Entity->RemoveComponent(TH_COMPONENT_HASH(Args[0].ToString()));
	});
	Models.System->SetCallback("open_materials", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetSelection(Inspector_Materials);
		Models.Materials->Update(nullptr);
	});
	Models.System->SetCallback("remove_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Material != nullptr)
			Scene->RemoveMaterial((uint64_t)Selection.Material->Id);

		SetSelection(Inspector_Materials);
		Models.Materials->Update(nullptr);
		Models.Surfaces->Update(nullptr);
	});
	Models.System->SetCallback("copy_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Material != nullptr)
			Selection.Material = Scene->AddMaterial(Scene->GetMaterialName(Selection.Material->Id) + "*", *Selection.Material);
	});
	Models.System->SetCallback("open_settings", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetSelection(Inspector_Settings);
	});
	Models.System->SetCallback("add_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetSelection(Inspector_Material, Scene->AddMaterial("Material", Material()));
		Models.Surfaces->Update(nullptr);
	});
	Models.System->SetCallback("import_model", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetSelection(Inspector_ImportModel);
	});
	Models.System->SetCallback("import_skin_animation", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		SetSelection(Inspector_ImportAnimation);
	});
	Models.System->SetCallback("export_skin_animation", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Selection.Entity || !Selection.Entity->GetComponent<Components::SkinAnimator>())
		{
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select entity with skin animator to export");
			this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
			return;
		}

		std::string Path;
		if (!OS::WantFileSave("Save skin animation", Content->GetEnvironment(), "*.xml,*.json,*.tsc", "Serialized skin animation (*.xml, *.json, *.tsc)", &Path))
			return;

		Document* Result = new Document();
		Result->Name = "skin-animation";

		auto* Animator = Selection.Entity->GetComponent<Components::SkinAnimator>();
		if (!NMake::Pack(Result, Animator->Clips))
			return;

		PropertyArgs Map;
		if (Stroke(&Path).EndsWith(".tsc"))
			Map["type"] = Property("BIN");
		else if (Stroke(&Path).EndsWith(".json"))
			Map["type"] = Property("JSON");
		else
			Map["type"] = Property("XML");

		if (!Content->Save<Document>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Skin animation cannot be saved");
		else
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Skin animation was saved");

		TH_RELEASE(Result);
		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("export_key_animation", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Selection.Entity || !Selection.Entity->GetComponent<Components::KeyAnimator>())
		{
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select entity with key animator to export");
			this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
			return;
		}

		std::string Path;
		if (!OS::WantFileSave("Save key animation", Content->GetEnvironment(), "*.xml,*.json,*.tkc", "Serialized key animation (*.xml, *.json, *.tsc)", &Path))
			return;

		Document* Result = new Document();
		Result->Name = "key-animation";

		auto* Animator = Selection.Entity->GetComponent<Components::KeyAnimator>();
		if (!NMake::Pack(Result, Animator->Clips))
			return;

		PropertyArgs Map;
		if (Stroke(&Path).EndsWith(".tkc"))
			Map["type"] = Property("BIN");
		else if (Stroke(&Path).EndsWith(".json"))
			Map["type"] = Property("JSON");
		else
			Map["type"] = Property("XML");

		if (!Content->Save<Document>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Key animation cannot be saved");
		else
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Key animation was saved");

		TH_RELEASE(Result);
		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("import_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		GetResource("material", [this](const std::string& File)
		{
			Document* Result = Content->Load<Document>(File);
			if (!Result)
				return;

			Material New;
			if (!NMake::Unpack(Result, &New))
				return;

			std::string Name = File;
			GetPathName(Name);

			SetSelection(Inspector_Material, Scene->AddMaterial(Name, New));
		});
	});
	Models.System->SetCallback("export_material", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Selection.Material)
		{
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Select material to export");
			this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
			return;
		}

		std::string Path;
		if (!OS::WantFileSave("Save material", Content->GetEnvironment(), "*.xml,*.json,*.tmd", "Serialized material (*.xml, *.json, *.tmd)", &Path))
			return;

		Document* Result = new Document();
		Result->Name = "material";

		if (!NMake::Pack(Result, *Selection.Material))
			return;

		PropertyArgs Map;
		if (Stroke(&Path).EndsWith(".tmd"))
			Map["type"] = Property("BIN");
		else if (Stroke(&Path).EndsWith(".json"))
			Map["type"] = Property("JSON");
		else
			Map["type"] = Property("XML");

		if (!Content->Save<Document>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Material cannot be saved");
		else
			this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Material was saved");

		delete Result;
		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("deploy_scene", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Resource.ScenePath.empty())
		{
			this->Activity->Message.Setup(AlertType_Warning, "Sandbox", "Editor's state will be flushed before start");
			this->Activity->Message.Button(AlertConfirm_Escape, "No", 1);
			this->Activity->Message.Button(AlertConfirm_Return, "Yes", 2);
			this->Activity->Message.Result([this](int Button)
			{
				if (Button == 2)
				{
					Demo::SetSource(Resource.ScenePath);
					Restate(ApplicationState_Terminated);
				}
			});
		}
		else
		{
			this->Activity->Message.Setup(AlertType_Error, "Sandbox", "Save the scene to deploy it later");
			this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
		}
	});
	Models.System->SetCallback("open_scene", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		GetResource("scene", [this](const std::string& File)
		{
			this->Resource.NextPath = File;
		});
	});
	Models.System->SetCallback("close_scene", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		std::string Path;
		if (!OS::WantFileSave("Save scene", Content->GetEnvironment(), "*.xml,*.json,*.tsg", "Serialized scene (*.xml, *.json, *.tsg)", &Path))
			return;

		PropertyArgs Map;
		if (Stroke(&Path).EndsWith(".tsg"))
			Map["type"] = Property("BIN");
		else if (Stroke(&Path).EndsWith(".json"))
			Map["type"] = Property("JSON");
		else
			Map["type"] = Property("XML");

		Scene->RemoveEntity(State.Camera, false);
		Content->Save<SceneGraph>(Path, Scene, Map);
		Scene->AddEntity(State.Camera);

		if (!Scene->IsActive())
			Scene->SetCamera(State.Camera);

		UpdateHierarchy();
		this->Activity->Message.Setup(AlertType_Info, "Sandbox", "Scene was saved");
		this->Activity->Message.Button(AlertConfirm_Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	Models.System->SetCallback("cancel_file", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		GetResource("", nullptr);
	});
	Models.System->SetCallback("add_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		Entity* Value = new Entity(Scene);
		if (Scene->AddEntity(Value))
			SetSelection(Inspector_Entity, Value);
	});
	Models.System->SetCallback("remove_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection(Inspector_None);
		}
	});
	Models.System->SetCallback("move_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 0];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	Models.System->SetCallback("rotate_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 1];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	Models.System->SetCallback("scale_entity", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 2];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	Models.System->SetCallback("place_position", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalPosition());
			State.Gizmo = Selection.Entity->Transform->GetWorld();
			GetEntitySync();
		}
	});
	Models.System->SetCallback("place_rotation", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalRotation()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalRotation());
			State.Gizmo = Selection.Entity->Transform->GetWorld();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->Transform->Rotation.DepthDirection();

			GetEntitySync();
		}
	});
	Models.System->SetCallback("place_combine", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalPosition());
			Selection.Entity->Transform->GetLocalRotation()->Set(*Scene->GetCamera()->GetEntity()->Transform->GetLocalRotation());
			State.Gizmo = Selection.Entity->Transform->GetWorld();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->Transform->Rotation.DepthDirection();

			GetEntitySync();
		}
	});
	Models.System->SetCallback("reset_position", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalPosition()->Set(0);
			GetEntitySync();
		}
	});
	Models.System->SetCallback("reset_rotation", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalRotation()->Set(0);
			GetEntitySync();
		}
	});
	Models.System->SetCallback("reset_scale", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->Transform->GetLocalScale()->Set(1);
			GetEntitySync();
		}
	});
	Models.System->SetCallback("add_cmp_skin_animator", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SkinAnimator>();
	});
	Models.System->SetCallback("add_cmp_key_animator", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::KeyAnimator>();
	});
	Models.System->SetCallback("add_cmp_emitter_animator", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::EmitterAnimator>();
	});
	Models.System->SetCallback("add_cmp_listener", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::AudioListener>();
	});
	Models.System->SetCallback("add_cmp_source", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::AudioSource>();
	});
	Models.System->SetCallback("add_cmp_reverb", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Reverb());
		}
	});
	Models.System->SetCallback("add_cmp_chorus", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Chorus());
		}
	});
	Models.System->SetCallback("add_cmp_distortion", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Distortion());
		}
	});
	Models.System->SetCallback("add_cmp_echo", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Echo());
		}
	});
	Models.System->SetCallback("add_cmp_flanger", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Flanger());
		}
	});
	Models.System->SetCallback("add_cmp_frequency_shifter", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::FrequencyShifter());
		}
	});
	Models.System->SetCallback("add_cmp_vocal_morpher", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::VocalMorpher());
		}
	});
	Models.System->SetCallback("add_cmp_pitch_shifter", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::PitchShifter());
		}
	});
	Models.System->SetCallback("add_cmp_ring_modulator", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::RingModulator());
		}
	});
	Models.System->SetCallback("add_cmp_autowah", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Autowah());
		}
	});
	Models.System->SetCallback("add_cmp_compressor", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Compressor());
		}
	});
	Models.System->SetCallback("add_cmp_equalizer", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Equalizer());
		}
	});
	Models.System->SetCallback("add_cmp_model", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Model>();
	});
	Models.System->SetCallback("add_cmp_skin", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Skin>();
	});
	Models.System->SetCallback("add_cmp_emitter", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Emitter>();
	});
	Models.System->SetCallback("add_cmp_decal", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Decal>();
	});
	Models.System->SetCallback("add_cmp_point_light", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::PointLight>();
	});
	Models.System->SetCallback("add_cmp_spot_light", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SpotLight>();
	});
	Models.System->SetCallback("add_cmp_line_light", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::LineLight>();
	});
	Models.System->SetCallback("add_cmp_reflection_probe", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::ReflectionProbe>();
	});
	Models.System->SetCallback("add_cmp_rigid_body", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::RigidBody>();
	});
	Models.System->SetCallback("add_cmp_soft_body", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SoftBody>();
	});
	Models.System->SetCallback("add_cmp_slider_constraint", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SliderConstraint>();
	});
	Models.System->SetCallback("add_cmp_acceleration", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Acceleration>();
	});
	Models.System->SetCallback("add_cmp_fly", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Fly>();
	});
	Models.System->SetCallback("add_cmp_free_look", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::FreeLook>();
	});
	Models.System->SetCallback("add_cmp_camera", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Camera>();
	});
	Models.System->SetCallback("add_cmp_3d_camera", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (!Selection.Entity)
			return;

		Components::Camera* Camera = Selection.Entity->AddComponent<Components::Camera>();
		Camera->GetRenderer()->AddRenderer<Renderers::Depth>();
		Camera->GetRenderer()->AddRenderer<Renderers::Environment>();
		Camera->GetRenderer()->AddRenderer<Renderers::Model>();
		Camera->GetRenderer()->AddRenderer<Renderers::Skin>();
		Camera->GetRenderer()->AddRenderer<Renderers::SoftBody>();
		Camera->GetRenderer()->AddRenderer<Renderers::Emitter>();
		Camera->GetRenderer()->AddRenderer<Renderers::Decal>();
		Camera->GetRenderer()->AddRenderer<Renderers::Lighting>();
		Camera->GetRenderer()->AddRenderer<Renderers::Transparency>();
	});
	Models.System->SetCallback("add_cmp_scriptable", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Scriptable>();
	});
	Models.System->SetCallback("add_rndr_model", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Model>();
		}
	});
	Models.System->SetCallback("add_rndr_skin", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Skin>();
		}
	});
	Models.System->SetCallback("add_rndr_soft_body", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SoftBody>();
		}
	});
	Models.System->SetCallback("add_rndr_decal", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Decal>();
		}
	});
	Models.System->SetCallback("add_rndr_emitter", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Emitter>();
		}
	});
	Models.System->SetCallback("add_rndr_depth", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Depth>();
		}
	});
	Models.System->SetCallback("add_rndr_lighting", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Lighting>();
		}
	});
	Models.System->SetCallback("add_rndr_environment", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Environment>();
		}
	});
	Models.System->SetCallback("add_rndr_transparency", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Transparency>();
		}
	});
	Models.System->SetCallback("add_rndr_ssr", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSR>();
		}
	});
	Models.System->SetCallback("add_rndr_ssao", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSAO>();
		}
	});
	Models.System->SetCallback("add_rndr_ssdo", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSDO>();
		}
	});
	Models.System->SetCallback("add_rndr_bloom", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Bloom>();
		}
	});
	Models.System->SetCallback("add_rndr_dof", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::DoF>();
		}
	});
	Models.System->SetCallback("add_rndr_tone", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Tone>();
		}
	});
	Models.System->SetCallback("add_rndr_glitch", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Glitch>();
		}
	});
	Models.System->SetCallback("add_rndr_gui", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::UserInterface>();
		}
	});
	Models.System->SetCallback("up_rndr", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->MoveRenderer(TH_COMPONENT_HASH(Args[0].ToString()), -1);
		}
	});
	Models.System->SetCallback("down_rndr", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->MoveRenderer(TH_COMPONENT_HASH(Args[0].ToString()), 1);
		}
	});
	Models.System->SetCallback("remove_rndr", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->RemoveRenderer(TH_COMPONENT_HASH(Args[0].ToString()));
		}
	});
	Models.System->SetCallback("toggle_rndr", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
			{
				auto* Renderer = Source->GetRenderer()->GetRenderer(TH_COMPONENT_HASH(Args[0].ToString()));
				if (Renderer != nullptr)
				{
					Renderer->Active = !Renderer->Active;
					if (Renderer->Active)
					{
						Renderer->Deactivate();
						Renderer->Activate();
					}
				}
			}
		}
	});
	Models.System->SetCallback("remove_aefx", [this](GUI::IEvent& Event, const PropertyList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSource()->RemoveEffectById(TH_COMPONENT_HASH(Args[0].ToString()));
		}
	});

	Models.Files = State.GUI->SetDataSource("files");
	Models.Files->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 2)
			return;

		Result = "<p class=\"file\" data-event-click=\"set_file('" + Data[1] + "')\">" + Data[0] + "</p>";
	});
	Models.Files->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		IFile* Target = Node->GetTarget<IFile>();
		if (Column == "name")
			Result = Target->Name;
		else if (Column == "path")
			Result = Target->Path;
	});
	Models.Files->SetDestroyCallback([](void* Target)
	{
		delete (IFile*)Target;
	});

	Models.Project = State.GUI->SetDataSource("project");
	Models.Project->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 4)
			return;

		int Depth = atoi(Data[2].c_str());
		for (int i = 0; i < Depth; ++i)
			Result += "<spacer />";

		if (Data[3] != "0")
		{
			Result += "<datagridexpand>";
			Result += "<button data-event-click=\"set_directory(\'" + Data[0] + "\')\"><span />" + Data[1] + "</button>";
			Result += "</datagridexpand>";
		}
		else
			Result += "<button data-event-click=\"set_directory(\'" + Data[0] + "\')\">" + Data[1] + "</button>";
	});
	Models.Project->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		FileTree* Target = Node->GetTarget<FileTree>();
		if (Column == "name")
		{
			Result = Target->Path;
			GetPathName(Result);
		}
		else if (Column == "path")
			Result = Target->Path;
	});
	Models.Project->SetChangeCallback([this](GUI::DataRow* Node)
	{
		FileTree* Target = Node->GetTarget<FileTree>();
		if (Target != nullptr)
		{
			bool IsRoot = (Node == Models.Project->Get());
			for (auto& Item : Target->Directories)
			{
				if (IsRoot && Stroke(&Item->Path).EndsWith("system"))
					continue;

				Node->AddChild(Item);
			}
		}
	});

	Models.Hierarchy = State.GUI->SetDataSource("hierarchy");
	Models.Hierarchy->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 4)
			return;

		int Depth = atoi(Data[2].c_str());
		for (int i = 0; i < Depth; ++i)
			Result += "<spacer />";

		if (Data[3] != "0")
		{
			Result += "<datagridexpand>";
			Result += "<div class=\"draggable\" entity-id=\"" + Data[0] + "\" data-event-dragdrop=\"set_parent(" + Data[0] + ")\"><button class=\"hierarchy-button\" data-class-selected=\"sl_entity == " + Data[0] + "\" data-event-click=\"set_entity(" + Data[0] + ")\"><span />" + Data[1] + "</button></div>";
			Result += "</datagridexpand>";
		}
		else
			Result += "<div class=\"draggable\" entity-id=\"" + Data[0] + "\" data-event-dragdrop=\"set_parent(" + Data[0] + ")\"><button class=\"hierarchy-button\" data-class-selected=\"sl_entity == " + Data[0] + "\" data-event-click=\"set_entity(" + Data[0] + ")\">" + Data[1] + "</button></div>";
	});
	Models.Hierarchy->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		if (Column == "name")
			Result = GetName(Node->GetTarget<Entity>());
		else if (Column == "id")
			Result = std::to_string(Node->GetTarget<Entity>()->Id);
	});
	Models.Hierarchy->SetChangeCallback([this](GUI::DataRow* Node)
	{
		Entity* Target = Node->GetTarget<Entity>();
		if (Target != nullptr)
		{
			auto* Childs = Target->Transform->GetChilds();
			if (Childs != nullptr)
			{
				for (auto& Child : *Childs)
					Node->AddChild(Child->Ptr<Entity>());
			}
		}
		else if (Scene != nullptr)
		{
			for (uint64_t i = 0; i < Scene->GetEntityCount(); i++)
			{
				Entity* Next = Scene->GetEntity(i);
				if (Next != State.Camera && !Next->Transform->GetRoot())
					Node->AddChild(Next);
			}
		}
	});

	Models.Materials = State.GUI->SetDataSource("materials");
	Models.Materials->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 2)
			return;
		
		Result = "<button class=\"selection\" data-event-click=\"set_material(" + Data[0] + ")\">" + GUI::Subsystem::EscapeHTML(Data[1]) + "</button>";
	});
	Models.Materials->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		Material* Target = Node->GetTarget<Material>();
		if (Column == "name")
		{
			Result = Scene->GetMaterialName(Target->Id);
			if (Result.empty())
				Result = "Material " + std::to_string((uint64_t)Target->Id);
			else
				Result += " " + std::to_string((uint64_t)Target->Id);
		}
		else if (Column == "id")
			Result = std::to_string((int64_t)Target->Id);
	});
	Models.Materials->SetChangeCallback([this](GUI::DataRow* Node)
	{
		Material* Target = Node->GetTarget<Material>();
		if (Target != nullptr)
			return;

		for (uint64_t i = 0; i < Scene->GetMaterialCount(); i++)
			Node->AddChild(new Material(*Scene->GetMaterialById(i)));
	});
	Models.Materials->SetDestroyCallback([](void* Target)
	{
		delete (Material*)Target;
	});

	Models.Surfaces = State.GUI->SetDataSource("surfaces");
	Models.Surfaces->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 2)
			return;

		Result = GUI::Subsystem::EscapeHTML(Data[1]);
	});
	Models.Surfaces->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		Material* Target = Node->GetTarget<Material>();
		if (Target != nullptr)
		{
			if (Column == "name")
			{
				Result = Scene->GetMaterialName(Target->Id);
				if (Result.empty())
					Result = "Material " + std::to_string((uint64_t)Target->Id);
				else
					Result += " " + std::to_string((uint64_t)Target->Id);
			}
			else if (Column == "id")
				Result = std::to_string((int64_t)Target->Id);
		}
		else if (Column == "name")
			Result = "None";
		else if (Column == "id")
			Result = "-1";
	});
	Models.Surfaces->SetChangeCallback([this](GUI::DataRow* Node)
	{
		Material* Target = Node->GetTarget<Material>();
		if (Target != nullptr || Node->GetParent() != nullptr)
			return;

		Node->AddChild(nullptr);
		if (!Scene)
			return;

		for (uint64_t i = 0; i < Scene->GetMaterialCount(); i++)
			Node->AddChild(new Material(*Scene->GetMaterialById(i)));
	});
	Models.Surfaces->SetDestroyCallback([](void* Target)
	{
		delete (Material*)Target;
	});

	Models.Model = State.GUI->SetDataSource("model");
	Models.Model->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 2)
			return;

		Result = GUI::Subsystem::EscapeHTML(Data[1]);
	});
	Models.Model->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		MeshBuffer* Target = Node->GetTarget<MeshBuffer>();
		if (!Selection.Entity)
			return;

		auto* Base = Selection.Entity->GetComponent<Components::Model>();
		if (!Base)
			return;

		if (Target != nullptr)
		{
			if (Column == "name")
				Result = (Target->Name.empty() ? "Unknown" : Target->Name);
			else if (Column == "id")
				Result = std::to_string((intptr_t)(void*)Target);
		}
		else if (Column == "name")
			Result = "None";
		else if (Column == "id")
			Result = "0";
	});
	Models.Model->SetChangeCallback([this](GUI::DataRow* Node)
	{
		MeshBuffer* Target = Node->GetTarget<MeshBuffer>();
		if (!Selection.Entity || Target != nullptr || Node->GetParent() != nullptr)
			return;

		auto* Base = Selection.Entity->GetComponent<Components::Model>();
		if (!Base)
			return;

		Node->AddChild(nullptr);
		if (!Base->GetDrawable())
			return;

		for (auto It : Base->GetDrawable()->Meshes)
			Node->AddChild(It);
	});

	Models.Skin = State.GUI->SetDataSource("skin");
	Models.Skin->SetFormatCallback([](const std::vector<std::string>& Data, std::string& Result)
	{
		if (Data.size() != 2)
			return;

		Result = GUI::Subsystem::EscapeHTML(Data[1]);
	});
	Models.Skin->SetColumnCallback([this](GUI::DataRow* Node, const std::string& Column, std::string& Result)
	{
		SkinMeshBuffer* Target = Node->GetTarget<SkinMeshBuffer>();
		if (!Selection.Entity)
			return;

		auto* Base = Selection.Entity->GetComponent<Components::Skin>();
		if (!Base)
			return;

		if (Target != nullptr)
		{
			if (Column == "name")
				Result = (Target->Name.empty() ? "Unknown" : Target->Name);
			else if (Column == "id")
				Result = std::to_string((intptr_t)(void*)Target);
		}
		else if (Column == "name")
			Result = "None";
		else if (Column == "id")
			Result = "0";
	});
	Models.Skin->SetChangeCallback([this](GUI::DataRow* Node)
	{
		SkinMeshBuffer* Target = Node->GetTarget<SkinMeshBuffer>();
		if (!Selection.Entity || Target != nullptr || Node->GetParent() != nullptr)
			return;

		auto* Base = Selection.Entity->GetComponent<Components::Skin>();
		if (!Base)
			return;

		Node->AddChild(nullptr);
		if (!Base->GetDrawable())
			return;

		for (auto It : Base->GetDrawable()->Meshes)
			Node->AddChild(It);
	});
}
void Sandbox::SetDirectory(FileTree* Base)
{
	Selection.Directory = Base;
	if (!Selection.Directory)
		return;

	Models.Files->Get()->RemoveChilds();
	for (auto& Item : Selection.Directory->Files)
	{
		std::string Name = Item;
		GetPathName(Name);

		IFile* File = new IFile();
		File->Name = Name;
		File->Path = Item;

		Models.Files->Get()->AddChild(File);
	}
}
void Sandbox::SetSelection(Inspector Window, void* Object)
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
			Selection.Entity = (Entity*)Object;
			break;
		case Inspector_Material:
			SetStatus("Material was selected");
			Selection.Material = (Material*)Object;
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
			SetSelection(Inspector_Entity, Current);
	}
	else
	{
		Selection.Might = nullptr;
		if (WantChange)
			SetSelection(Inspector_None);
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
}
void Sandbox::GetResource(const std::string& Name, const std::function<void(const std::string&)>& Callback)
{
	if (Name == "__got__")
	{
		SetStatus("Resource selected");

		State.OnResource = nullptr;
		State.Filename.clear();
		return;
	}

	if (Name.empty() || !Callback)
	{
		SetStatus("Resource selection cancelled");

		State.OnResource = nullptr;
		State.Filename.clear();
		return;
	}

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
	return State.IsSceneFocused && !State.GUI->IsInputFocused();
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
		Value->GetComponent<Components::Skin>())
		return Icons.Mesh;

	if (Value->GetComponent<Components::Emitter>())
		return Icons.Emitter;

	if (Value->GetComponent<Components::Decal>())
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

	if (Value->GetComponent<Components::Skin>())
		return "[Skin]";

	if (Value->GetComponent<Components::Emitter>())
		return "[Emitter]";

	if (Value->GetComponent<Components::Decal>())
		return "[Decal]";

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
		Result = "[Empty] unknown";
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
		Result += " unnamed " + std::to_string(Value->Id);

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