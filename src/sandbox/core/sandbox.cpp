#include "sandbox.h"
#include "demo.h"
#include "../controller/resolvers.h"
#include "../controller/components.h"
#include "../controller/renderers.h"
#include "../controller/effects.h"

Sandbox::Sandbox(Application::Desc* Conf, const std::string& Path) : Application(Conf)
{
	Resource.NextPath = Path;
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
	if (Key == KeyCode::CURSORLEFT && Selection.Gizmo)
	{
		if (!Pressed)
		{
			Activity->SetGrabbing(false);
			Selection.Gizmo->OnMouseUp((unsigned int)State.Cursor.X, (unsigned int)State.Cursor.Y);
		}
		else if (Selection.Gizmo->OnMouseDown((unsigned int)State.Cursor.X, (unsigned int)State.Cursor.Y))
			Activity->SetGrabbing(true);
	}
}
void Sandbox::WindowEvent(WindowState NewState, int X, int Y)
{
	switch (NewState)
	{
		case WindowState::Resize:
			Renderer->ResizeBuffers((unsigned int)X, (unsigned int)Y);
			if (Scene != nullptr)
				Scene->ResizeBuffers();

			SetStatus("Buffer resize was submitted");
			break;
		case WindowState::Close:
			Activity->Message.Setup(AlertType::Warning, "Sandbox", "Do you really want to exit?");
			Activity->Message.Button(AlertConfirm::Escape, "No", 1);
			Activity->Message.Button(AlertConfirm::Return, "Yes", 2);
			Activity->Message.Result([this](int Button)
			{
				if (Button == 2)
					Stop();
			});
			break;
		default:
			break;
	}
}
void Sandbox::ScriptHook(VMGlobal* Global)
{
}
void Sandbox::Initialize()
{
	States.DepthStencil = Renderer->GetDepthStencilState("none");
	States.NoneRasterizer = Renderer->GetRasterizerState("cull-none");
	States.BackRasterizer = Renderer->GetRasterizerState("cull-back");
	States.Blend = Renderer->GetBlendState("additive");
	States.Layout = Renderer->GetInputLayout("shape-vertex");
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
	State.IsCaptured = false;
	State.Directory = nullptr;
	State.GUI = new GUI::Context(Renderer);
	State.GUI->SetMountCallback([this](GUI::Context*)
	{
		if (!State.IsMounted)
		{
			SetViewModel();
			State.IsMounted = true;
		}
	});
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

	if (!State.GUI->Inject("system/conf.xml"))
	{
		TH_ERR("could not load GUI");
		return Stop();
	}

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
void Sandbox::Dispatch(Timer* Time)
{
#ifdef _DEBUG
	if (Activity->IsKeyDownHit(KeyCode::F5))
	{
		if (State.GUI != nullptr)
		{
			State.GUI->Deconstruct();
			State.GUI->ClearCache();
			State.GUI->Inject("system/conf.xml");
		}
	}
#endif
	if (Activity->IsKeyDownHit(KeyCode::F6))
		State.IsInteractive = !State.IsInteractive;

	if (!State.IsCameraActive && Scene->GetCamera()->GetEntity() == State.Camera)
		State.IsCameraActive = true;

	State.Camera->GetComponent<Components::Fly>()->SetActive(GetSceneFocus());
	State.Camera->GetComponent<Components::FreeLook>()->SetActive(GetSceneFocus());
	if (!Scene->IsActive() && GetSceneFocus())
	{
		for (auto& Item : *State.Camera)
			Item.second->Update(Time);
	}

	if (State.IsInteractive)
	{
		State.Frames = (float)Time->GetFrameCount();
		if (!Resource.NextPath.empty())
		{
			UpdateScene();
			Scene->Dispatch(Time);
			return;
		}

		UpdateSystem();
		if (State.GUI != nullptr)
			State.GUI->UpdateEvents(Activity);
	}

	Scene->Dispatch(Time);
	if (State.IsCaptured)
	{
		State.IsCaptured = false;
		return;
	}

	if (!State.IsInteractive)
		return;

	if (Activity->IsKeyDownHit(KeyCode::F1))
	{
		State.IsTraceMode = !State.IsTraceMode;
		if (!State.IsTraceMode)
			SetStatus("Tracing mode: off");
		else
			SetStatus("Tracing mode: on");
	}

	if (!State.Camera || Scene->GetCamera()->GetEntity() != State.Camera || !State.IsCameraActive)
		return;

	if (Selection.Entity != nullptr)
	{
		if (Activity->IsKeyDownHit(KeyCode::DELETEKEY))
		{
			SetStatus("Entity was removed");
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection(Inspector_None);
		}

		if (GetSceneFocus())
		{
			if (Activity->IsKeyDownHit(KeyCode::D1))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 0];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Movement gizmo selected");
			}
			else if (Activity->IsKeyDownHit(KeyCode::D2))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 1];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Rotation gizmo selected");
			}
			else if (Activity->IsKeyDownHit(KeyCode::D3))
			{
				Selection.Gizmo = Resource.Gizmo[Selection.Move = 2];
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
				Selection.Gizmo->SetDisplayScale(State.GizmoScale);
				Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
				SetStatus("Scale gizmo selected");
			}
		}
	}

	if (Selection.Entity != nullptr)
	{
		if (Selection.Gizmo != nullptr)
		{
			Transform::Spacing Space;
			Space.Position = State.Gizmo.Position();
			Space.Rotation = State.Gizmo.Rotation();
			Selection.Entity->GetTransform()->Localize(Space);

			if (Selection.Gizmo->IsActive())
			{
				switch (Selection.Move)
				{
					case 1:
						Selection.Entity->GetTransform()->SetRotation(Space.Rotation);
						break;
					case 2:
						Selection.Entity->GetTransform()->SetScale(State.Gizmo.Scale());
						break;
					default:
						Selection.Entity->GetTransform()->SetPosition(Space.Position);
						break;
				}
				GetEntitySync();
			}
			else
			{
				State.Gizmo = Selection.Entity->GetTransform()->GetBias();
				Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
			}
		}
		else
		{
			State.Gizmo = Selection.Entity->GetTransform()->GetBias();
			Selection.Gizmo = Resource.Gizmo[Selection.Move];
			Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
			Selection.Gizmo->SetDisplayScale(State.GizmoScale);
			Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
		}
	}

	GetEntityCell();
	if (Selection.Entity != nullptr && Selection.Entity != State.Camera)
	{
		if (Activity->IsKeyDownHit(KeyCode::DELETEKEY))
		{
			SetStatus("Entity was removed");
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection(Inspector_None);
		}

		if (Activity->IsKeyDown(KeyMod::LCTRL) && GetSceneFocus())
		{
			if (Activity->IsKeyDownHit(KeyCode::V) || Activity->IsKeyDown(KeyCode::B))
			{
				SetStatus("Entity was cloned");
				Scene->CloneEntity(Selection.Entity, [this](SceneGraph* Scene, Entity* Result)
				{
					SetSelection(Result ? Inspector_Entity : Inspector_None, Result);
				});
			}

			if (Activity->IsKeyDownHit(KeyCode::X))
			{
				SetStatus("Entity was removed");
				Scene->RemoveEntity(Selection.Entity, true);
				SetSelection(Inspector_None);
			}
		}
	}

	if (Activity->IsKeyDown(KeyMod::LCTRL))
	{
		if (Activity->IsKeyDown(KeyCode::N))
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

		if (Activity->IsKeyDown(KeyCode::X))
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
void Sandbox::Publish(Timer* Time)
{
	Renderer->Clear(0, 0, 0);
	Renderer->ClearDepth();

	Scene->Publish(Time);
	if (State.IsInteractive && State.Camera == Scene->GetCamera()->GetEntity())
	{
		Scene->SetMRT(TargetType::Main, false);
		UpdateGrid(Time);
	}

	Scene->Submit();
	if (State.IsInteractive && State.GUI != nullptr)
		State.GUI->RenderLists(Renderer->GetRenderTarget()->GetTarget());

	Renderer->Submit();
}
void Sandbox::UpdateProject()
{
	SetStatus("Project's hierarchy was updated");
	std::string Directory = "";
	if (Selection.Directory != nullptr)
	{
		Directory = Selection.Directory->Path;
		Selection.Directory = nullptr;
	}

	TH_RELEASE(State.Directory);
	State.Directory = new FileTree(Resource.CurrentPath);
	if (!State.Directories)
		return;

	State.Directories->Clear();
	SetContents(State.Directory);
	State.Directories->SortTree();
}
void Sandbox::UpdateScene()
{
	SetSelection(Inspector_None);
	State.IsCameraActive = true;
	State.Entities->Clear();
	State.Materials->Clear();

	if (Scene != nullptr)
		TH_CLEAR(Scene);

	VM->ClearCache();
	if (!Resource.NextPath.empty())
	{
		VariantArgs Args;
		Args["active"] = Var::Boolean(false);

		Scene = Content->Load<SceneGraph>(Resource.NextPath, Args);
	}

	if (Scene == nullptr)
	{
		SceneGraph::Desc I = SceneGraph::Desc::Get(this);
		I.Simulator.EnableSoftBody = true;

		Scene = new SceneGraph(I);
		Scene->SetActive(false);

		SetStatus("Created new scene from scratch");
	}
	else
		SetStatus("Scene was loaded");

	Scene->SetListener("mutation", std::bind(&Sandbox::UpdateMutation, this, std::placeholders::_1, std::placeholders::_2));
	State.Camera = new ::Entity(Scene);
	State.Camera->AddComponent<Components::Camera>();
	State.Camera->AddComponent<Components::FreeLook>();

	auto* Fly = State.Camera->AddComponent<Components::Fly>();
	Fly->SpeedDown *= 0.25f;
	Fly->SpeedNormal *= 0.35f;

	Scene->AddEntity(State.Camera);
	Scene->SetCamera(State.Camera);

	auto* fRenderer = Scene->GetRenderer();
	fRenderer->AddRenderer<Renderers::Model>();
	fRenderer->AddRenderer<Renderers::Skin>();
	fRenderer->AddRenderer<Renderers::SoftBody>();
	fRenderer->AddRenderer<Renderers::Emitter>();
	fRenderer->AddRenderer<Renderers::Decal>();
	fRenderer->AddRenderer<Renderers::Lighting>();
	//fRenderer->AddRenderer<Renderers::Transparency>();

	Resource.ScenePath = Resource.NextPath;
	Resource.NextPath.clear();
}
void Sandbox::UpdateGrid(Timer* Time)
{
	Matrix4x4 Origin[4] =
	{
		Matrix4x4::Identity(),
		Matrix4x4::CreateTranslation({ 0, 0, -2 }),
		Matrix4x4::Create({ 0, 0, 0 }, 1,{ 0, Math<float>::Deg2Rad() * (90), 0 }),
		Matrix4x4::Create({ -2, 0, 0 }, 1,{ 0, Math<float>::Deg2Rad() * (90), 0 })
	};

	Renderer->SetDepthStencilState(States.DepthStencil);
	Renderer->SetBlendState(States.Blend);
	Renderer->SetRasterizerState(States.BackRasterizer);
	Renderer->SetInputLayout(States.Layout);
	Renderer->SetShader(Renderer->GetBasicEffect(), TH_VS | TH_PS);
	Renderer->SetVertexBuffer(Cache.Primitives->GetQuad(), 0);

	for (uint32_t i = 0; i < Scene->GetEntitiesCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (State.Camera == Value)
			continue;

		float Direction = -Value->GetTransform()->GetPosition().LookAtXZ(State.Camera->GetTransform()->GetPosition());
		Renderer->Render.TexCoord = (Value == Selection.Entity ? 0.5f : 0.05f);
		Renderer->Render.WorldViewProj = Matrix4x4::Create(Value->GetTransform()->GetPosition(), 0.5f, Vector3(0, Direction))* State.Camera->GetComponent<Components::Camera>()->GetViewProjection();
		Renderer->SetTexture2D(GetIcon(Value), 1, TH_PS);
		Renderer->UpdateBuffer(RenderBufferType::Render);
		Renderer->Draw(6, 0);
	}

	if (Selection.Gizmo != nullptr)
	{
		Renderer->SetRasterizerState(States.NoneRasterizer);
		Vector2 Size = Activity->GetSize();
		Selection.Gizmo->SetScreenDimension((int)Size.X, (int)Size.Y);
		Selection.Gizmo->SetCameraMatrix(((Components::Camera*)Scene->GetCamera())->GetView().Row, ((Components::Camera*)Scene->GetCamera())->GetProjection().Row);
		Selection.Gizmo->Draw();
		Renderer->SetRasterizerState(States.BackRasterizer);
	}

	for (uint32_t i = 0; i < Scene->GetEntitiesCount(); i++)
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
					Matrix4x4 Offset = (Value->GetTransform()->GetRoot() ? Value->GetTransform()->GetRoot()->GetBias() : Matrix4x4::Identity());
					for (size_t j = 0; j < Keys->size(); j++)
					{
						auto& Pos = Keys->at(j).Position;
						Renderer->Begin();
						Renderer->Topology(PrimitiveTopology::Line_Strip);
						Renderer->Transform(Offset * ((Components::Camera*)Scene->GetCamera())->GetViewProjection());
						Renderer->Emit();
						Renderer->Position(Pos.X, Pos.Y, -Pos.Z);
						Renderer->Emit();

						if (KeyAnimator->State.Frame == j)
							Renderer->Color(1, 0, 1, 1);

						if (j + 1 >= Keys->size())
						{
							auto& xPos = Keys->at(0).Position;
							Renderer->Position(xPos.X, xPos.Y, -xPos.Z);
						}
						else
						{
							auto& xPos = Keys->at(j + 1).Position;
							Renderer->Position(xPos.X, xPos.Y, -xPos.Z);
						}

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
				Matrix4x4 Offset = Value->GetTransform()->GetBiasUnscaled();
				UpdateJoint(&Skin->Skeleton, &Joint, &Offset);
			}
		}

		Matrix4x4 Transform;
		if (Skin != nullptr)
			Transform = Value->GetComponent<Components::Skin>()->GetBoundingBox();
		else if (Value->GetComponent<Components::Model>())
			Transform = Value->GetComponent<Components::Model>()->GetBoundingBox();
		else if (Value->GetComponent<Components::SoftBody>())
			Transform = Value->GetComponent<Components::SoftBody>()->GetBoundingBox();
		else if (Value->GetComponent<Components::PointLight>() || Value->GetComponent<Components::SpotLight>() || Value->GetComponent<Components::LineLight>())
			Transform = Value->GetTransform()->GetBiasUnscaled();
		else
			Transform = Value->GetTransform()->GetBias();

		for (int j = 0; j < 4; j++)
		{
			Renderer->Begin();
			Renderer->Topology(PrimitiveTopology::Line_Strip);
			Renderer->Transform(Origin[j] * Transform * ((Components::Camera*)Scene->GetCamera())->GetViewProjection());
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

	if (Selection.Material != nullptr && Selection.Window == Inspector_Material)
	{
		Matrix4x4 ViewProjection = ((Components::Camera*)Scene->GetCamera())->GetViewProjection();
		for (uint32_t i = 0; i < Scene->GetEntitiesCount(); i++)
		{
			Entity* Entity = Scene->GetEntity(i);
			Material* Source = nullptr;

			if (Entity->GetComponent<Components::Model>())
			{
				auto* It = Entity->GetComponent<Components::Model>();
				for (auto& Face : It->GetMaterials())
				{
					if (Face.second == Selection.Material)
					{
						Source = Face.second;
						break;
					}
				}
			}
			else if (Entity->GetComponent<Components::Skin>())
			{
				auto* It = Entity->GetComponent<Components::Skin>();
				for (auto& Face : It->GetMaterials())
				{
					if (Face.second == Selection.Material)
					{
						Source = Face.second;
						break;
					}
				}
			}
			else if (Entity->GetComponent<Components::Emitter>())
				Source = Entity->GetComponent<Components::Emitter>()->GetMaterial();
			else if (Entity->GetComponent<Components::SoftBody>())
				Source = Entity->GetComponent<Components::SoftBody>()->GetMaterial();
			else if (Entity->GetComponent<Components::Decal>())
				Source = Entity->GetComponent<Components::Decal>()->GetMaterial();

			if (Source != Selection.Material)
				continue;

			for (const auto& j : Origin)
			{
				Renderer->Begin();
				Renderer->Topology(PrimitiveTopology::Line_Strip);
				Renderer->Transform(j * Scene->GetEntity(i)->GetTransform()->GetBias() * ViewProjection);
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
	Renderer->Topology(PrimitiveTopology::Line_Strip);
	Renderer->Transform(((Components::Camera*)Scene->GetCamera())->GetViewProjection());
	Renderer->Emit();
	Renderer->Position(Position1.X, Position1.Y, -Position1.Z);
	Renderer->Emit();
	Renderer->Position(Position2.X, Position2.Y, -Position2.Z);
	Renderer->End();

	for (auto& Child : Base->Childs)
		UpdateJoint(Map, &Child, World);
}
void Sandbox::UpdateMutation(const std::string& Name, VariantArgs& Args)
{
	if (Args.find("entity") != Args.end())
	{
		Entity* Base = (Entity*)Args["entity"].GetPointer();
		if (!Base || Base == State.Camera || !State.Entities)
			return;

		for (size_t i = 0; i < State.Entities->GetSize(); i++)
		{
			GUI::DataNode& Node = State.Entities->At(i);
			if (GUI::IElement::ToPointer(Node.At(1).GetString()) == (void*)Base)
			{
				State.Entities->Remove(i);
				break;
			}
		}

		if (Args["type"].IsString("pop") || (!Args["type"].IsString("set") && !Args["type"].IsString("push")))
			return;

		auto Top = std::make_pair(GetEntityIndex(Base), (size_t)GetEntityNesting(Base));

		VariantList Item;
		Item.emplace_back(std::move(Var::Integer(Top.second)));
		Item.emplace_back(std::move(Var::String(GUI::IElement::FromPointer((void*)Base))));
		Item.emplace_back(std::move(Var::String(GetName(Base))));
		State.Entities->Add(Item, &Top);
	}
	else if (Args.find("parent") != Args.end())
	{
		Entity* Base = (Entity*)Args["child"].GetPointer();
		if (!Base || Base == State.Camera || !State.Entities)
			return;

		auto Top = std::make_pair(GetEntityIndex(Base), (size_t)GetEntityNesting(Base));

		VariantList Item;
		Item.emplace_back(std::move(Var::Integer(GetEntityNesting(Base))));
		Item.emplace_back(std::move(Var::String(GUI::IElement::FromPointer((void*)Base))));
		Item.emplace_back(std::move(Var::String(GetName(Base))));

		for (size_t i = 0; i < State.Entities->GetSize(); i++)
		{
			GUI::DataNode& Node = State.Entities->At(i);
			if (GUI::IElement::ToPointer(Node.At(1).GetString()) == (void*)Base)
			{
				Node.Replace(Item, &Top);
				State.Entities->SortTree();
				SetMutation(Base, Args["type"].GetString());
				return;
			}
		}

		State.Entities->Add(Item, &Top);
	}
	else if (Args.find("component") != Args.end())
	{
		Component* Child = (Component*)Args["component"].GetPointer();
		if (!Child || Child->GetEntity() == State.Camera || !State.Entities)
			return;

		Entity* Base = Child->GetEntity();
		for (size_t i = 0; i < State.Entities->GetSize(); i++)
		{
			GUI::DataNode& Node = State.Entities->At(i);
			if (GUI::IElement::ToPointer(Node.At(1).GetString()) == (void*)Base)
			{
				State.Entities->Remove(i);
				break;
			}
		}

		auto Top = std::make_pair(GetEntityIndex(Base), (size_t)GetEntityNesting(Base));

		VariantList Item;
		Item.emplace_back(std::move(Var::Integer(Top.second)));
		Item.emplace_back(std::move(Var::String(GUI::IElement::FromPointer((void*)Base))));
		Item.emplace_back(std::move(Var::String(GetName(Base))));
		State.Entities->Add(Item, &Top);
	}
	else if (Args.find("material") != Args.end())
	{
		Material* Base = (Material*)Args["material"].GetPointer();
		if (!Base || !State.Materials)
			return;

		for (size_t i = 0; i < State.Materials->GetSize(); i++)
		{
			GUI::DataNode& Node = State.Materials->At(i);
			if (Node.At(0).GetInteger() == Base->Slot)
			{
				State.Materials->Remove(i);
				break;
			}
		}

		if (!Args["type"].IsString("push") && !Args["type"].IsString("set"))
			return;

		VariantList Item;
		Item.emplace_back(std::move(Var::Integer(Base->Slot)));
		Item.emplace_back(std::move(Var::String(Base->GetName().empty() ? "Material #" + Base->Slot : Base->GetName())));
		Item.emplace_back(std::move(Var::String(GUI::IElement::FromPointer((void*)Base))));
		State.Materials->Add(Item);
	}
}
void Sandbox::UpdateSystem()
{
	State.System->SetBoolean("scene_active", Scene->IsActive());
	State.System->SetBoolean("sl_resource", !State.Filename.empty());
	State.System->SetInteger("sl_material", Selection.Material ? Selection.Material->Slot : -1);
	State.System->SetString("sl_entity", GUI::IElement::FromPointer((void*)Selection.Entity));
	State.System->SetString("sl_status", State.Status);

	switch (Selection.Window)
	{
		case Inspector_Entity:
			State.System->SetString("sl_window", "entity");
			InspectEntity();
			break;
		case Inspector_Material:
			State.System->SetString("sl_window", "material");
			InspectMaterial();
			break;
		case Inspector_Settings:
			State.System->SetString("sl_window", "settings");
			InspectSettings();
			break;
		case Inspector_Materials:
			State.System->SetString("sl_window", "materials");
			break;
		case Inspector_ImportModel:
			State.System->SetString("sl_window", "import-model");
			InspectImporter();
			break;
		case Inspector_ImportAnimation:
			State.System->SetString("sl_window", "import-animation");
			InspectImporter();
			break;
		case Inspector_None:
		default:
			State.System->SetString("sl_window", "none");
			break;
	}
}
void Sandbox::InspectEntity()
{
	static Entity* LastBase = nullptr;
	if (!Selection.Entity)
		return;

	Entity* Base = Selection.Entity;
	bool Changed = (LastBase != Base);
	if (Changed)
		LastBase = Base;

	std::string Name = Base->GetName();
	if (State.GUI->GetElementById(0, "ent_name").CastFormString(&Name))
		Base->SetName(Name);

	auto& Space = Base->GetTransform()->GetSpacing();
	if (State.GUI->GetElementById(0, "ent_pos_x").CastFormFloat(&Space.Position.X) ||
		State.GUI->GetElementById(0, "ent_pos_y").CastFormFloat(&Space.Position.Y) ||
		State.GUI->GetElementById(0, "ent_pos_z").CastFormFloat(&Space.Position.Z) ||
		State.GUI->GetElementById(0, "ent_rot_x").CastFormFloat(&Space.Rotation.X, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_rot_y").CastFormFloat(&Space.Rotation.Y, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_rot_z").CastFormFloat(&Space.Rotation.Z, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById(0, "ent_scale_x").CastFormFloat(&Space.Scale.X) ||
		State.GUI->GetElementById(0, "ent_scale_y").CastFormFloat(&Space.Scale.Y) ||
		State.GUI->GetElementById(0, "ent_scale_z").CastFormFloat(&Space.Scale.Z))
	{
		Base->GetTransform()->MakeDirty();
		GetEntitySync();
	}

	bool Scaling = Base->GetTransform()->HasScaling();
	State.GUI->GetElementById(0, "ent_tag").CastFormInt64(&Base->Tag);
	if (State.GUI->GetElementById(0, "ent_const_scale").CastFormBoolean(&Scaling))
		Base->GetTransform()->SetScaling(Scaling);

	if (State.System->SetBoolean("sl_cmp_model", Base->GetComponent<Components::Model>() != nullptr)->GetBoolean())
		ComponentModel(State.GUI, Base->GetComponent<Components::Model>(), Changed);

	if (State.System->SetBoolean("sl_cmp_skin", Base->GetComponent<Components::Skin>() != nullptr)->GetBoolean())
		ComponentSkin(State.GUI, Base->GetComponent<Components::Skin>(), Changed);

	if (State.System->SetBoolean("sl_cmp_emitter", Base->GetComponent<Components::Emitter>() != nullptr)->GetBoolean())
		ComponentEmitter(State.GUI, Base->GetComponent<Components::Emitter>(), Changed);

	if (State.System->SetBoolean("sl_cmp_soft_body", Base->GetComponent<Components::SoftBody>() != nullptr)->GetBoolean())
		ComponentSoftBody(State.GUI, Base->GetComponent<Components::SoftBody>(), Changed);

	if (State.System->SetBoolean("sl_cmp_decal", Base->GetComponent<Components::Decal>() != nullptr)->GetBoolean())
		ComponentDecal(State.GUI, Base->GetComponent<Components::Decal>(), Changed);

	if (State.System->SetBoolean("sl_cmp_skin_animator", Base->GetComponent<Components::SkinAnimator>() != nullptr)->GetBoolean())
		ComponentSkinAnimator(State.GUI, Base->GetComponent<Components::SkinAnimator>(), Changed);

	if (State.System->SetBoolean("sl_cmp_key_animator", Base->GetComponent<Components::KeyAnimator>() != nullptr)->GetBoolean())
		ComponentKeyAnimator(State.GUI, Base->GetComponent<Components::KeyAnimator>(), Changed);

	if (State.System->SetBoolean("sl_cmp_emitter_animator", Base->GetComponent<Components::EmitterAnimator>() != nullptr)->GetBoolean())
		ComponentEmitterAnimator(State.GUI, Base->GetComponent<Components::EmitterAnimator>(), Changed);

	if (State.System->SetBoolean("sl_cmp_rigid_body", Base->GetComponent<Components::RigidBody>() != nullptr)->GetBoolean())
		ComponentRigidBody(State.GUI, Base->GetComponent<Components::RigidBody>(), Changed);

	if (State.System->SetBoolean("sl_cmp_acceleration", Base->GetComponent<Components::Acceleration>() != nullptr)->GetBoolean())
		ComponentAcceleration(State.GUI, Base->GetComponent<Components::Acceleration>(), Changed);

	if (State.System->SetBoolean("sl_cmp_slider_constraint", Base->GetComponent<Components::SliderConstraint>() != nullptr)->GetBoolean())
		ComponentSliderConstraint(State.GUI, Base->GetComponent<Components::SliderConstraint>(), Changed);

	if (State.System->SetBoolean("sl_cmp_free_look", Base->GetComponent<Components::FreeLook>() != nullptr)->GetBoolean())
		ComponentFreeLook(State.GUI, Base->GetComponent<Components::FreeLook>(), Changed);

	if (State.System->SetBoolean("sl_cmp_fly", Base->GetComponent<Components::Fly>() != nullptr)->GetBoolean())
		ComponentFly(State.GUI, Base->GetComponent<Components::Fly>(), Changed);

	if (State.System->SetBoolean("sl_cmp_audio_source", Base->GetComponent<Components::AudioSource>() != nullptr)->GetBoolean())
	{
		auto* Source = Base->GetComponent<Components::AudioSource>();
		State.System->SetBoolean("sl_cmp_audio_source_compressor", Source->GetSource()->GetEffect<Effects::Compressor>() != nullptr);

		if (State.System->SetBoolean("sl_cmp_audio_source_reverb", Source->GetSource()->GetEffect<Effects::Reverb>() != nullptr)->GetBoolean())
			EffectReverb(State.GUI, Source->GetSource()->GetEffect<Effects::Reverb>());

		if (State.System->SetBoolean("sl_cmp_audio_source_chorus", Source->GetSource()->GetEffect<Effects::Chorus>() != nullptr)->GetBoolean())
			EffectChorus(State.GUI, Source->GetSource()->GetEffect<Effects::Chorus>());

		if (State.System->SetBoolean("sl_cmp_audio_source_distortion", Source->GetSource()->GetEffect<Effects::Distortion>() != nullptr)->GetBoolean())
			EffectDistortion(State.GUI, Source->GetSource()->GetEffect<Effects::Distortion>());

		if (State.System->SetBoolean("sl_cmp_audio_source_echo", Source->GetSource()->GetEffect<Effects::Echo>() != nullptr)->GetBoolean())
			EffectEcho(State.GUI, Source->GetSource()->GetEffect<Effects::Echo>());

		if (State.System->SetBoolean("sl_cmp_audio_source_flanger", Source->GetSource()->GetEffect<Effects::Flanger>() != nullptr)->GetBoolean())
			EffectFlanger(State.GUI, Source->GetSource()->GetEffect<Effects::Flanger>());

		if (State.System->SetBoolean("sl_cmp_audio_source_frequency_shifter", Source->GetSource()->GetEffect<Effects::FrequencyShifter>() != nullptr)->GetBoolean())
			EffectFrequencyShifter(State.GUI, Source->GetSource()->GetEffect<Effects::FrequencyShifter>());

		if (State.System->SetBoolean("sl_cmp_audio_source_vocal_morpher", Source->GetSource()->GetEffect<Effects::VocalMorpher>() != nullptr)->GetBoolean())
			EffectVocalMorpher(State.GUI, Source->GetSource()->GetEffect<Effects::VocalMorpher>());

		if (State.System->SetBoolean("sl_cmp_audio_source_pitch_shifter", Source->GetSource()->GetEffect<Effects::PitchShifter>() != nullptr)->GetBoolean())
			EffectPitchShifter(State.GUI, Source->GetSource()->GetEffect<Effects::PitchShifter>());

		if (State.System->SetBoolean("sl_cmp_audio_source_ring_modulator", Source->GetSource()->GetEffect<Effects::RingModulator>() != nullptr)->GetBoolean())
			EffectRingModulator(State.GUI, Source->GetSource()->GetEffect<Effects::RingModulator>());

		if (State.System->SetBoolean("sl_cmp_audio_source_autowah", Source->GetSource()->GetEffect<Effects::Autowah>() != nullptr)->GetBoolean())
			EffectAutowah(State.GUI, Source->GetSource()->GetEffect<Effects::Autowah>());

		if (State.System->SetBoolean("sl_cmp_audio_source_equalizer", Source->GetSource()->GetEffect<Effects::Equalizer>() != nullptr)->GetBoolean())
			EffectEqualizer(State.GUI, Source->GetSource()->GetEffect<Effects::Equalizer>());

		ComponentAudioSource(State.GUI, Source, Changed);
	}

	if (State.System->SetBoolean("sl_cmp_audio_listener", Base->GetComponent<Components::AudioListener>() != nullptr)->GetBoolean())
		ComponentAudioListener(State.GUI, Base->GetComponent<Components::AudioListener>(), Changed);

	if (State.System->SetBoolean("sl_cmp_point_light", Base->GetComponent<Components::PointLight>() != nullptr)->GetBoolean())
		ComponentPointLight(State.GUI, Base->GetComponent<Components::PointLight>(), Changed);

	if (State.System->SetBoolean("sl_cmp_spot_light", Base->GetComponent<Components::SpotLight>() != nullptr)->GetBoolean())
		ComponentSpotLight(State.GUI, Base->GetComponent<Components::SpotLight>(), Changed);

	if (State.System->SetBoolean("sl_cmp_line_light", Base->GetComponent<Components::LineLight>() != nullptr)->GetBoolean())
		ComponentLineLight(State.GUI, Base->GetComponent<Components::LineLight>(), Changed);

	if (State.System->SetBoolean("sl_cmp_surface_light", Base->GetComponent<Components::SurfaceLight>() != nullptr)->GetBoolean())
		ComponentSurfaceLight(State.GUI, Base->GetComponent<Components::SurfaceLight>(), Changed);

	if (State.System->SetBoolean("sl_cmp_illuminator", Base->GetComponent<Components::Illuminator>() != nullptr)->GetBoolean())
		ComponentIlluminator(State.GUI, Base->GetComponent<Components::Illuminator>(), Changed);

	if (State.System->SetBoolean("sl_cmp_camera", Base->GetComponent<Components::Camera>() != nullptr)->GetBoolean())
	{
		auto* Camera = Base->GetComponent<Components::Camera>();
		auto* fRenderer = Camera->GetRenderer();
		State.System->SetInteger("sl_cmp_camera_model", fRenderer->GetOffset<Renderers::Model>());
		State.System->SetInteger("sl_cmp_camera_skin", fRenderer->GetOffset<Renderers::Skin>());
		State.System->SetInteger("sl_cmp_camera_soft_body", fRenderer->GetOffset<Renderers::SoftBody>());
		State.System->SetInteger("sl_cmp_camera_decal", fRenderer->GetOffset<Renderers::Decal>());
		State.System->SetInteger("sl_cmp_camera_emitter", fRenderer->GetOffset<Renderers::Emitter>());
		State.System->SetInteger("sl_cmp_camera_gui", fRenderer->GetOffset<Renderers::UserInterface>());
		State.System->SetInteger("sl_cmp_camera_transparency", fRenderer->GetOffset<Renderers::Transparency>());

		if (State.System->SetInteger("sl_cmp_camera_lighting", fRenderer->GetOffset<Renderers::Lighting>())->GetInteger() >= 0)
			RendererLighting(State.GUI, fRenderer->GetRenderer<Renderers::Lighting>());

		if (State.System->SetInteger("sl_cmp_camera_ssr", fRenderer->GetOffset<Renderers::SSR>())->GetInteger() >= 0)
			RendererSSR(State.GUI, fRenderer->GetRenderer<Renderers::SSR>());

		if (State.System->SetInteger("sl_cmp_camera_ssao", fRenderer->GetOffset<Renderers::SSAO>())->GetInteger() >= 0)
			RendererSSAO(State.GUI, fRenderer->GetRenderer<Renderers::SSAO>());

		if (State.System->SetInteger("sl_cmp_camera_motionblur", fRenderer->GetOffset<Renderers::MotionBlur>())->GetInteger() >= 0)
			RendererMotionBlur(State.GUI, fRenderer->GetRenderer<Renderers::MotionBlur>());

		if (State.System->SetInteger("sl_cmp_camera_bloom", fRenderer->GetOffset<Renderers::Bloom>())->GetInteger() >= 0)
			RendererBloom(State.GUI, fRenderer->GetRenderer<Renderers::Bloom>());

		if (State.System->SetInteger("sl_cmp_camera_dof", fRenderer->GetOffset<Renderers::DoF>())->GetInteger() >= 0)
			RendererDoF(State.GUI, fRenderer->GetRenderer<Renderers::DoF>());

		if (State.System->SetInteger("sl_cmp_camera_tone", fRenderer->GetOffset<Renderers::Tone>())->GetInteger() >= 0)
			RendererTone(State.GUI, fRenderer->GetRenderer<Renderers::Tone>());

		if (State.System->SetInteger("sl_cmp_camera_glitch", fRenderer->GetOffset<Renderers::Glitch>())->GetInteger() >= 0)
			RendererGlitch(State.GUI, fRenderer->GetRenderer<Renderers::Glitch>());

		ComponentCamera(State.GUI, Camera, Changed);
	}

	if (State.System->SetBoolean("sl_cmp_scriptable", Base->GetComponent<Components::Scriptable>() != nullptr)->GetBoolean())
		ComponentScriptable(State.GUI, Base->GetComponent<Components::Scriptable>(), Changed);
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
	if (!Base)
		return;

	ResolveTexture2D(State.GUI, "mat_diffuse_source", Base->GetDiffuseMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetDiffuseMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_normal_source", Base->GetNormalMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetNormalMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_metallic_source", Base->GetMetallicMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetMetallicMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_roughness_source", Base->GetRoughnessMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetRoughnessMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_height_source", Base->GetHeightMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetHeightMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_occlusion_source", Base->GetOcclusionMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetOcclusionMap(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_emission_source", Base->GetEmissionMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetEmissionMap(New);
	}, false);

	ResolveColor3(State.GUI, "mat_diffuse", &Base->Surface.Diffuse);
	if (State.GUI->GetElementById(0, "mat_cemn").CastFormColor(&Base->Surface.Emission, false))
		State.GUI->GetElementById(0, "mat_cemn_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Surface.Emission.X * 255.0f), (unsigned int)(Base->Surface.Emission.Y * 255.0f), (unsigned int)(Base->Surface.Emission.Z * 255.0f)).R());

	if (State.GUI->GetElementById(0, "mat_cmet").CastFormColor(&Base->Surface.Metallic, false))
		State.GUI->GetElementById(0, "mat_cmet_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Surface.Metallic.X * 255.0f), (unsigned int)(Base->Surface.Metallic.Y * 255.0f), (unsigned int)(Base->Surface.Metallic.Z * 255.0f)).R());

	std::string Name = Base->GetName();
	if (State.GUI->GetElementById(0, "mat_name").CastFormString(&Name))
		Base->SetName(Name);

	State.GUI->GetElementById(0, "mat_scat_x").CastFormFloat(&Base->Surface.Scatter.X);
	State.GUI->GetElementById(0, "mat_scat_y").CastFormFloat(&Base->Surface.Scatter.Y);
	State.GUI->GetElementById(0, "mat_scat_z").CastFormFloat(&Base->Surface.Scatter.Z);
	State.GUI->GetElementById(0, "mat_memn").CastFormFloat(&Base->Surface.Emission.W);
	State.GUI->GetElementById(0, "mat_mmet").CastFormFloat(&Base->Surface.Metallic.W);
	State.GUI->GetElementById(0, "mat_rs").CastFormFloat(&Base->Surface.Roughness.X);
	State.GUI->GetElementById(0, "mat_mrs").CastFormFloat(&Base->Surface.Roughness.Y);
	State.GUI->GetElementById(0, "mat_occ").CastFormFloat(&Base->Surface.Occlusion.X);
	State.GUI->GetElementById(0, "mat_mocc").CastFormFloat(&Base->Surface.Occlusion.Y);
	State.GUI->GetElementById(0, "mat_fres").CastFormFloat(&Base->Surface.Fresnel);
	State.GUI->GetElementById(0, "mat_refr").CastFormFloat(&Base->Surface.Refraction);
	State.GUI->GetElementById(0, "mat_alpha").CastFormFloat(&Base->Surface.Transparency);
	State.GUI->GetElementById(0, "mat_env").CastFormFloat(&Base->Surface.Environment);
	State.GUI->GetElementById(0, "mat_rad").CastFormFloat(&Base->Surface.Radius);
	State.GUI->GetElementById(0, "mat_ht_amnt").CastFormFloat(&Base->Surface.Height);
	State.GUI->GetElementById(0, "mat_ht_bias").CastFormFloat(&Base->Surface.Bias);
}
void Sandbox::SetViewModel()
{
	SetStatus("GUI view-model setup done");
	if (!State.GUI)
		return;

	State.System = State.GUI->SetDataModel("system");
	State.Entities = State.System->SetArray("entities");
	State.Materials = State.System->SetArray("materials");
	State.Models = State.System->SetArray("models");
	State.Skins = State.System->SetArray("skins");
	State.Directories = State.System->SetArray("directories");
	State.Files = State.System->SetArray("files");
	State.System->SetBoolean("scene_active", false);
	State.System->SetString("sl_status", "");
	State.System->SetString("sl_window", "none");
	State.System->SetString("sl_entity", GUI::IElement::FromPointer(nullptr));
	State.System->SetInteger("sl_material", -1);
	State.System->SetBoolean("sl_resource", false);
	State.System->SetBoolean("sl_cmp_model", false);
	State.System->SetString("sl_cmp_model_mesh", "");
	State.System->SetBoolean("sl_cmp_model_source", false);
	State.System->SetBoolean("sl_cmp_model_assigned", false);
	State.System->SetBoolean("sl_cmp_skin", false);
	State.System->SetString("sl_cmp_skin_mesh", "");
	State.System->SetBoolean("sl_cmp_skin_source", false);
	State.System->SetBoolean("sl_cmp_skin_assigned", false);
	State.System->SetInteger("sl_cmp_skin_joint", -1);
	State.System->SetInteger("sl_cmp_skin_joints", -1);
	State.System->SetBoolean("sl_cmp_emitter", false);
	State.System->SetBoolean("sl_cmp_soft_body", false);
	State.System->SetBoolean("sl_cmp_soft_body_source", false);
	State.System->SetBoolean("sl_cmp_decal", false);
	State.System->SetBoolean("sl_cmp_skin_animator", false);
	State.System->SetInteger("sl_cmp_skin_animator_joint", -1);
	State.System->SetInteger("sl_cmp_skin_animator_joints", -1);
	State.System->SetInteger("sl_cmp_skin_animator_frame", -1);
	State.System->SetInteger("sl_cmp_skin_animator_frames", -1);
	State.System->SetInteger("sl_cmp_skin_animator_clip", -1);
	State.System->SetInteger("sl_cmp_skin_animator_clips", -1);
	State.System->SetBoolean("sl_cmp_key_animator", false);
	State.System->SetInteger("sl_cmp_key_animator_frame", -1);
	State.System->SetInteger("sl_cmp_key_animator_frames", -1);
	State.System->SetInteger("sl_cmp_key_animator_clip", -1);
	State.System->SetInteger("sl_cmp_key_animator_clips", -1);
	State.System->SetBoolean("sl_cmp_emitter_animator", false);
	State.System->SetBoolean("sl_cmp_rigid_body", false);
	State.System->SetBoolean("sl_cmp_rigid_body_source", false);
	State.System->SetBoolean("sl_cmp_rigid_body_from_source", false);
	State.System->SetBoolean("sl_cmp_acceleration", false);
	State.System->SetBoolean("sl_cmp_slider_constraint", false);
	State.System->SetBoolean("sl_cmp_slider_constraint_entity", false);
	State.System->SetBoolean("sl_cmp_free_look", false);
	State.System->SetBoolean("sl_cmp_fly", false);
	State.System->SetBoolean("sl_cmp_audio_source", false);
	State.System->SetBoolean("sl_cmp_audio_source_clip", false);
	State.System->SetFloat("sl_cmp_audio_source_length", 0.0f);
	State.System->SetBoolean("sl_cmp_audio_source_reverb", false);
	State.System->SetBoolean("sl_cmp_audio_source_chorus", false);
	State.System->SetBoolean("sl_cmp_audio_source_distortion", false);
	State.System->SetBoolean("sl_cmp_audio_source_echo", false);
	State.System->SetBoolean("sl_cmp_audio_source_flanger", false);
	State.System->SetBoolean("sl_cmp_audio_source_frequency_shifter", false);
	State.System->SetBoolean("sl_cmp_audio_source_vocal_morpher", false);
	State.System->SetBoolean("sl_cmp_audio_source_pitch_shifter", false);
	State.System->SetBoolean("sl_cmp_audio_source_ring_modulator", false);
	State.System->SetBoolean("sl_cmp_audio_source_autowah", false);
	State.System->SetBoolean("sl_cmp_audio_source_compressor", false);
	State.System->SetBoolean("sl_cmp_audio_source_equalizer", false);
	State.System->SetBoolean("sl_cmp_audio_listener", false);
	State.System->SetBoolean("sl_cmp_point_light", false);
	State.System->SetBoolean("sl_cmp_spot_light", false);
	State.System->SetBoolean("sl_cmp_line_light", false);
	State.System->SetInteger("sl_cmp_line_light_cascades", -1);
	State.System->SetBoolean("sl_cmp_surface_light", false);
	State.System->SetBoolean("sl_cmp_illuminator", false);
	State.System->SetBoolean("sl_cmp_camera", false);
	State.System->SetInteger("sl_cmp_camera_model", -1);
	State.System->SetInteger("sl_cmp_camera_skin", -1);
	State.System->SetInteger("sl_cmp_camera_soft_body", -1);
	State.System->SetInteger("sl_cmp_camera_decal", -1);
	State.System->SetInteger("sl_cmp_camera_emitter", -1);
	State.System->SetInteger("sl_cmp_camera_depth", -1);
	State.System->SetInteger("sl_cmp_camera_lighting", -1);
	State.System->SetInteger("sl_cmp_camera_environment", -1);
	State.System->SetInteger("sl_cmp_camera_transparency", -1);
	State.System->SetInteger("sl_cmp_camera_ssr", -1);
	State.System->SetInteger("sl_cmp_camera_ssao", -1);
	State.System->SetInteger("sl_cmp_camera_motionblur", -1);
	State.System->SetInteger("sl_cmp_camera_bloom", -1);
	State.System->SetInteger("sl_cmp_camera_dof", -1);
	State.System->SetInteger("sl_cmp_camera_tone", -1);
	State.System->SetInteger("sl_cmp_camera_glitch", -1);
	State.System->SetInteger("sl_cmp_camera_gui", -1);
	State.System->SetBoolean("sl_cmp_scriptable", false);
	State.System->SetBoolean("sl_cmp_scriptable_source", false);
	State.System->SetCallback("set_parent", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GUI::IElement Target = (Rml::Element*)Event.GetPointer("drag_element");
		if (!Target.IsValid())
			return;

		Entity* Child = (Entity*)GUI::IElement::ToPointer(Target.GetAttribute("entity"));
		if (!Child)
			return;

		if (Args.size() == 1)
		{
			Entity* Base = (Entity*)GUI::IElement::ToPointer(Args[0].GetBlob());
			if (Base != nullptr)
				Child->SetRoot(Base);
			else
				Child->SetRoot(nullptr);
			Event.StopImmediatePropagation();
		}
		else if (Args.empty())
			Child->SetRoot(nullptr);
	});
	State.System->SetCallback("set_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Args.size() != 1)
			return;

		Entity* Base = (Entity*)GUI::IElement::ToPointer(Args[0].GetBlob());
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
	State.System->SetCallback("set_controls", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Args.size() == 1)
			State.IsSceneFocused = Args[0].GetBoolean();
	});
	State.System->SetCallback("set_menu", [](GUI::IEvent& Event, const VariantList& Args)
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
	State.System->SetCallback("set_directory", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (State.Directory != nullptr)
			SetDirectory((FileTree*)GUI::IElement::ToPointer(Args[0].GetBlob()));
	});
	State.System->SetCallback("set_file", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Args.size() != 1)
			return;

		std::string fResource = Args[0].Serialize();
		if (fResource.empty() || !State.OnResource)
			return;

		auto Callback = State.OnResource;
		GetResource("__got__", nullptr);

		Callback(fResource);
	});
	State.System->SetCallback("set_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Args.size() != 1)
			return;

		SetSelection(Inspector_Material, Scene->GetMaterial(Args[0].GetInteger()));
	});
	State.System->SetCallback("save_settings", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Scene->Configure(Scene->GetConf());
	});
	State.System->SetCallback("switch_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (!Scene->IsActive())
		{
			VariantArgs Args;
			Args["type"] = Var::String("XML");

			Scene->RemoveEntity(State.Camera, false);
			Content->Save<SceneGraph>("./system/cache.xml", Scene, Args);
			Scene->AddEntity(State.Camera);
			Scene->SetActive(true);
			Scene->ScriptHook();

			auto* Cameras = Scene->GetComponents<Components::Camera>();
			for (auto It = Cameras->Begin(); It != Cameras->End(); It++)
			{
				Components::Camera* Base = (Components::Camera*)*It;
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
			this->Resource.NextPath = "./system/cache.xml";
	});
	State.System->SetCallback("import_model_action", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		std::string From;
		if (!OS::Input::Open("Import mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &From))
			return;

		if (!OS::File::IsExists(From.c_str()))
			return;

		Processors::Model* Processor = (Processors::Model*)Content->GetProcessor<Model>();
		if (Processor != nullptr)
		{
			Document* Doc = Processor->Import(From, State.MeshImportOpts);
			if (Doc != nullptr)
			{
				std::string To;
				if (!OS::Input::Save("Save mesh", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized mesh (*.xml, *.json, *.jsonb)", &To))
					return;

				VariantArgs Args;
				if (Parser(&To).EndsWith(".jsonb"))
					Args["type"] = Var::String("JSONB");
				else if (Parser(&To).EndsWith(".json"))
					Args["type"] = Var::String("JSON");
				else
					Args["type"] = Var::String("XML");

				Content->Save<Document>(To, Doc, Args);
				TH_RELEASE(Doc);
				this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Mesh was imported");
			}
			else
				this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Mesh is unsupported");
		}
		else
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Mesh cannot be imported");

		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("import_skin_animation_action", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		std::string From;
		if (!OS::Input::Open("Import animation from mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &From))
			return;

		if (!OS::File::IsExists(From.c_str()))
			return;

		Processors::SkinModel* Processor = (Processors::SkinModel*)Content->GetProcessor<Model>();
		if (Processor != nullptr)
		{
			Document* Doc = Processor->ImportAnimation(From, State.MeshImportOpts);
			if (Doc != nullptr)
			{
				std::string To;
				if (!OS::Input::Save("Save animation", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized skin animation (*.xml, *.json, *.jsonb)", &To))
					return;

				VariantArgs Args;
				if (Parser(&To).EndsWith(".jsonb"))
					Args["type"] = Var::String("JSONB");
				else if (Parser(&To).EndsWith(".json"))
					Args["type"] = Var::String("JSON");
				else
					Args["type"] = Var::String("XML");

				Content->Save<Document>(To, Doc, Args);
				TH_RELEASE(Doc);
				this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Animation was imported");
			}
			else
				this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Animation is unsupported");
		}
		else
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Animation cannot be imported");

		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("update_project", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		UpdateProject();
	});
	State.System->SetCallback("remove_cmp", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Args.size() != 1)
			return;

		if (Selection.Entity != nullptr)
			Selection.Entity->RemoveComponent(TH_COMPONENT_HASH(Args[0].Serialize()));
	});
	State.System->SetCallback("open_materials", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_Materials);
	});
	State.System->SetCallback("remove_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Material != nullptr)
			Scene->RemoveMaterial(Selection.Material);
		SetSelection(Inspector_Materials);
	});
	State.System->SetCallback("copy_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Material != nullptr)
			Selection.Material = Scene->CloneMaterial(Selection.Material, Selection.Material->GetName() + "*");
	});
	State.System->SetCallback("open_settings", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_Settings);
	});
	State.System->SetCallback("add_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_Material, Scene->AddMaterial(new Material(nullptr), "Material " + std::to_string(Scene->GetMaterialsCount() + 1)));
	});
	State.System->SetCallback("import_model", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_ImportModel);
	});
	State.System->SetCallback("import_skin_animation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_ImportAnimation);
	});
	State.System->SetCallback("export_skin_animation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (!Selection.Entity || !Selection.Entity->GetComponent<Components::SkinAnimator>())
		{
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Select entity with skin animator to export");
			this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
			return;
		}

		std::string Path;
		if (!OS::Input::Save("Save skin animation", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized skin animation (*.xml, *.json, *.jsonb)", &Path))
			return;

		Document* Result = Var::Set::Object();
		Result->Key = "skin-animation";

		auto* Animator = Selection.Entity->GetComponent<Components::SkinAnimator>();
		NMake::Pack(Result, Animator->Clips);

		VariantArgs Map;
		if (Parser(&Path).EndsWith(".jsonb"))
			Map["type"] = Var::String("JSONB");
		else if (Parser(&Path).EndsWith(".json"))
			Map["type"] = Var::String("JSON");
		else
			Map["type"] = Var::String("XML");

		if (!Content->Save<Document>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Skin animation cannot be saved");
		else
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Skin animation was saved");

		TH_RELEASE(Result);
		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("export_key_animation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (!Selection.Entity || !Selection.Entity->GetComponent<Components::KeyAnimator>())
		{
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Select entity with key animator to export");
			this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
			return;
		}

		std::string Path;
		if (!OS::Input::Save("Save key animation", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized key animation (*.xml, *.json, *.jsonb)", &Path))
			return;

		Document* Result = Var::Set::Object();
		Result->Key = "key-animation";

		auto* Animator = Selection.Entity->GetComponent<Components::KeyAnimator>();
		NMake::Pack(Result, Animator->Clips);

		VariantArgs Map;
		if (Parser(&Path).EndsWith(".jsonb"))
			Map["type"] = Var::String("JSONB");
		else if (Parser(&Path).EndsWith(".json"))
			Map["type"] = Var::String("JSON");
		else
			Map["type"] = Var::String("XML");

		if (!Content->Save<Document>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Key animation cannot be saved");
		else
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Key animation was saved");

		TH_RELEASE(Result);
		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("import_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GetResource("material", [this](const std::string& File)
		{
			Material* Result = Content->Load<Material>(File);
			if (!Result || !Scene->AddMaterial(Result))
				return;

			SetSelection(Inspector_Material, Result);
		});
	});
	State.System->SetCallback("deploy_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (!Resource.ScenePath.empty())
		{
			this->Activity->Message.Setup(AlertType::Warning, "Sandbox", "Editor's state will be flushed before start");
			this->Activity->Message.Button(AlertConfirm::Escape, "No", 1);
			this->Activity->Message.Button(AlertConfirm::Return, "Yes", 2);
			this->Activity->Message.Result([this](int Button)
			{
				if (Button == 2)
				{
					Demo::SetSource(Resource.ScenePath);
					Stop();
				}
			});
		}
		else
		{
			this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Save the scene to deploy it later");
			this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
			this->Activity->Message.Result(nullptr);
		}
	});
	State.System->SetCallback("open_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GetResource("scene", [this](const std::string& File)
		{
			this->Resource.NextPath = File;
		});
	});
	State.System->SetCallback("close_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		std::string Path;
		if (!OS::Input::Save("Save scene", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized scene (*.xml, *.json, *.jsonb)", &Path))
			return;

		VariantArgs Map;
		if (Parser(&Path).EndsWith(".jsonb"))
			Map["type"] = Var::String("JSONB");
		else if (Parser(&Path).EndsWith(".json"))
			Map["type"] = Var::String("JSON");
		else
			Map["type"] = Var::String("XML");

		Scene->RemoveEntity(State.Camera, false);
		Content->Save<SceneGraph>(Path, Scene, Map);
		Scene->AddEntity(State.Camera);

		if (!Scene->IsActive())
			Scene->SetCamera(State.Camera);

		this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Scene was saved");
		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("cancel_file", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GetResource("", nullptr);
	});
	State.System->SetCallback("add_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Entity* Value = new Entity(Scene);
		if (Scene->AddEntity(Value))
			SetSelection(Inspector_Entity, Value);
	});
	State.System->SetCallback("remove_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Scene->RemoveEntity(Selection.Entity, true);
			SetSelection(Inspector_None);
		}
	});
	State.System->SetCallback("move_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 0];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	State.System->SetCallback("rotate_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 1];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	State.System->SetCallback("scale_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Selection.Gizmo = Resource.Gizmo[Selection.Move = 2];
		Selection.Gizmo->SetEditMatrix(State.Gizmo.Row);
		Selection.Gizmo->SetDisplayScale(State.GizmoScale);
		Selection.Gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	State.System->SetCallback("place_position", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetPosition(Scene->GetCamera()->GetEntity()->GetTransform()->GetPosition());
			State.Gizmo = Selection.Entity->GetTransform()->GetBias();
			GetEntitySync();
		}
	});
	State.System->SetCallback("place_rotation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetRotation(Scene->GetCamera()->GetEntity()->GetTransform()->GetRotation());
			State.Gizmo = Selection.Entity->GetTransform()->GetBias();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->GetTransform()->GetRotation().dDirection();

			GetEntitySync();
		}
	});
	State.System->SetCallback("place_combine", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetPosition(Scene->GetCamera()->GetEntity()->GetTransform()->GetPosition());
			Selection.Entity->GetTransform()->SetRotation(Scene->GetCamera()->GetEntity()->GetTransform()->GetRotation());
			State.Gizmo = Selection.Entity->GetTransform()->GetBias();

			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSync().Direction = -Scene->GetCamera()->GetEntity()->GetTransform()->GetRotation().dDirection();

			GetEntitySync();
		}
	});
	State.System->SetCallback("reset_position", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetPosition(0);
			GetEntitySync();
		}
	});
	State.System->SetCallback("reset_rotation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetRotation(0);
			GetEntitySync();
		}
	});
	State.System->SetCallback("reset_scale", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Selection.Entity->GetTransform()->SetScale(1);
			GetEntitySync();
		}
	});
	State.System->SetCallback("add_cmp_skin_animator", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SkinAnimator>();
	});
	State.System->SetCallback("add_cmp_key_animator", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::KeyAnimator>();
	});
	State.System->SetCallback("add_cmp_emitter_animator", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::EmitterAnimator>();
	});
	State.System->SetCallback("add_cmp_listener", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::AudioListener>();
	});
	State.System->SetCallback("add_cmp_source", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::AudioSource>();
	});
	State.System->SetCallback("add_cmp_reverb", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Reverb());
		}
	});
	State.System->SetCallback("add_cmp_chorus", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Chorus());
		}
	});
	State.System->SetCallback("add_cmp_distortion", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Distortion());
		}
	});
	State.System->SetCallback("add_cmp_echo", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Echo());
		}
	});
	State.System->SetCallback("add_cmp_flanger", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Flanger());
		}
	});
	State.System->SetCallback("add_cmp_frequency_shifter", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::FrequencyShifter());
		}
	});
	State.System->SetCallback("add_cmp_vocal_morpher", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::VocalMorpher());
		}
	});
	State.System->SetCallback("add_cmp_pitch_shifter", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::PitchShifter());
		}
	});
	State.System->SetCallback("add_cmp_ring_modulator", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::RingModulator());
		}
	});
	State.System->SetCallback("add_cmp_autowah", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Autowah());
		}
	});
	State.System->SetCallback("add_cmp_compressor", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Compressor());
		}
	});
	State.System->SetCallback("add_cmp_equalizer", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::AudioSource>();

			Source->GetSource()->AddEffect(new Effects::Equalizer());
		}
	});
	State.System->SetCallback("add_cmp_model", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Model>();
	});
	State.System->SetCallback("add_cmp_skin", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Skin>();
	});
	State.System->SetCallback("add_cmp_emitter", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Emitter>();
	});
	State.System->SetCallback("add_cmp_decal", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Decal>();
	});
	State.System->SetCallback("add_cmp_point_light", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::PointLight>();
	});
	State.System->SetCallback("add_cmp_spot_light", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SpotLight>();
	});
	State.System->SetCallback("add_cmp_line_light", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::LineLight>();
	});
	State.System->SetCallback("add_cmp_surface_light", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SurfaceLight>();
	});
	State.System->SetCallback("add_cmp_illuminator", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Illuminator>();
	});
	State.System->SetCallback("add_cmp_rigid_body", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::RigidBody>();
	});
	State.System->SetCallback("add_cmp_soft_body", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SoftBody>();
	});
	State.System->SetCallback("add_cmp_slider_constraint", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::SliderConstraint>();
	});
	State.System->SetCallback("add_cmp_acceleration", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Acceleration>();
	});
	State.System->SetCallback("add_cmp_fly", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Fly>();
	});
	State.System->SetCallback("add_cmp_free_look", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::FreeLook>();
	});
	State.System->SetCallback("add_cmp_camera", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Camera>();
	});
	State.System->SetCallback("add_cmp_3d_camera", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (!Selection.Entity)
			return;

		Components::Camera* Camera = Selection.Entity->AddComponent<Components::Camera>();
		auto* fRenderer = Camera->GetRenderer();
		fRenderer->AddRenderer<Renderers::Model>();
		fRenderer->AddRenderer<Renderers::Skin>();
		fRenderer->AddRenderer<Renderers::SoftBody>();
		fRenderer->AddRenderer<Renderers::Emitter>();
		fRenderer->AddRenderer<Renderers::Decal>();
		fRenderer->AddRenderer<Renderers::Lighting>();
		fRenderer->AddRenderer<Renderers::Transparency>();
	});
	State.System->SetCallback("add_cmp_scriptable", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
			Selection.Entity->AddComponent<Components::Scriptable>();
	});
	State.System->SetCallback("add_rndr_model", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Model>();
		}
	});
	State.System->SetCallback("add_rndr_skin", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Skin>();
		}
	});
	State.System->SetCallback("add_rndr_soft_body", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SoftBody>();
		}
	});
	State.System->SetCallback("add_rndr_decal", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Decal>();
		}
	});
	State.System->SetCallback("add_rndr_emitter", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Emitter>();
		}
	});
	State.System->SetCallback("add_rndr_lighting", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Lighting>();
		}
	});
	State.System->SetCallback("add_rndr_transparency", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Transparency>();
		}
	});
	State.System->SetCallback("add_rndr_ssr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSR>();
		}
	});
	State.System->SetCallback("add_rndr_ssao", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSAO>();
		}
	});
	State.System->SetCallback("add_rndr_motionblur", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::MotionBlur>();
		}
	});
	State.System->SetCallback("add_rndr_bloom", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Bloom>();
		}
	});
	State.System->SetCallback("add_rndr_dof", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::DoF>();
		}
	});
	State.System->SetCallback("add_rndr_tone", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Tone>();
		}
	});
	State.System->SetCallback("add_rndr_glitch", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::Glitch>();
		}
	});
	State.System->SetCallback("add_rndr_gui", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::UserInterface>();
		}
	});
	State.System->SetCallback("up_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->MoveRenderer(TH_COMPONENT_HASH(Args[0].Serialize()), -1);
		}
	});
	State.System->SetCallback("down_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->MoveRenderer(TH_COMPONENT_HASH(Args[0].Serialize()), 1);
		}
	});
	State.System->SetCallback("remove_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->RemoveRenderer(TH_COMPONENT_HASH(Args[0].Serialize()));
		}
	});
	State.System->SetCallback("toggle_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
			{
				auto* fRenderer = Source->GetRenderer()->GetRenderer(TH_COMPONENT_HASH(Args[0].Serialize()));
				if (fRenderer != nullptr)
				{
					fRenderer->Active = !fRenderer->Active;
					if (fRenderer->Active)
					{
						fRenderer->Deactivate();
						fRenderer->Activate();
					}
				}
			}
		}
	});
	State.System->SetCallback("remove_aefx", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::AudioSource>();
			if (Source != nullptr)
				Source->GetSource()->RemoveEffectById(TH_COMPONENT_HASH(Args[0].Serialize()));
		}
	});
}
void Sandbox::SetDirectory(FileTree* Base)
{
	Selection.Directory = Base;
	if (!Selection.Directory)
		return;

	State.Files->Clear();
	for (auto& Next : Selection.Directory->Files)
	{
		std::string Name = Next;
		GetPathName(Name);

		VariantList Item;
		Item.emplace_back(std::move(Var::String(Name)));
		Item.emplace_back(std::move(Var::String(Next)));
		State.Files->Add(Item);
	}
}
void Sandbox::SetContents(FileTree* Base, int64_t Depth)
{
	bool IsRoot = (Base == State.Directory);
	size_t Index = 1;

	for (auto* Next : Base->Directories)
	{
		if (IsRoot && (Parser(&Next->Path).EndsWith("system") || Parser(&Next->Path).EndsWith("assembly")))
			continue;

		auto Top = std::make_pair((void*)(uintptr_t)Index++, (size_t)Depth);
		std::string Result = Next->Path;
		GetPathName(Result);

		VariantList Item;
		Item.emplace_back(std::move(Var::String(Result)));
		Item.emplace_back(std::move(Var::String(GUI::IElement::FromPointer((void*)Next))));
		Item.emplace_back(std::move(Var::Integer(Depth)));
		State.Directories->Add(Item);

		float D = (float)Depth + 1;
		SetContents(Next, Depth + 1);
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
			SetMetadata(Selection.Entity);
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
void Sandbox::SetMutation(Entity* Parent, const char* Type)
{
	if (!Parent)
		return;

	for (size_t i = 0; i < Parent->GetChildsCount(); i++)
	{
		Entity* Child = Parent->GetChild(i);
		Scene->Mutate(Parent, Child, Type);
		SetMutation(Child, Type);
	}
}
void Sandbox::SetMetadata(Entity* Source)
{
	if (!Source)
		return;

	auto* Model = Source->GetComponent<Components::Model>();
	if (State.Models != nullptr && Model != nullptr)
	{
		State.Models->Clear();
		if (Model->GetDrawable())
		{
			for (auto* Buffer : Model->GetDrawable()->Meshes)
			{
				VariantList Item;
				Item.emplace_back(Var::String(GUI::IElement::FromPointer(Buffer)));
				Item.emplace_back(Var::String(Buffer->Name.empty() ? "Unknown" : Buffer->Name));
				State.Models->Add(Item);
			}
		}
	}

	auto* Skin = Source->GetComponent<Components::Skin>();
	if (State.Skins != nullptr && Skin != nullptr)
	{
		State.Skins->Clear();
		if (Model->GetDrawable())
		{
			for (auto* Buffer : Skin->GetDrawable()->Meshes)
			{
				VariantList Item;
				Item.emplace_back(Var::String(GUI::IElement::FromPointer(Buffer)));
				Item.emplace_back(Var::String(Buffer->Name.empty() ? "Unknown" : Buffer->Name));
				State.Skins->Add(Item);
			}
		}
	}
}
void Sandbox::GetPathName(std::string& Path)
{
	int64_t Distance = 0;
	if (Path.back() == '/' || Path.back() == '\\')
		Path.erase(Path.size() - 1);

	for (size_t i = Path.size(); i > 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			Distance = (int64_t)i + 1;
			break;
		}
	}

	if (Distance > 0)
		Path.erase(0, (size_t)Distance);
}
void Sandbox::GetEntityCell()
{
	if (!GetSceneFocus() || (Selection.Gizmo && Selection.Gizmo->IsActive()))
		return;

	auto* Camera = State.Camera->GetComponent<Components::Camera>();
	bool WantChange = Activity->IsKeyDownHit(KeyCode::CURSORLEFT);
	Ray Cursor = Camera->GetScreenRay(Activity->GetCursorPosition());
	float Distance = -1.0f;
	Entity* Current = nullptr;

	for (uint32_t i = 0; i < Scene->GetEntitiesCount(); i++)
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
			Value->GetComponent<Components::SurfaceLight>())
			Transform = Value->GetTransform()->GetBiasUnscaled();
		else if (Value->GetComponent<Components::Model>())
			Transform = Value->GetComponent<Components::Model>()->GetBoundingBox();
		else if (Value->GetComponent<Components::Skin>())
			Transform = Value->GetComponent<Components::Skin>()->GetBoundingBox();
		else if (Value->GetComponent<Components::SoftBody>())
			Transform = Value->GetComponent<Components::SoftBody>()->GetBoundingBox();
		else
			Transform = Value->GetTransform()->GetBias();

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
		PointLight->GetEntity()->GetTransform()->SetScale(PointLight->GetRange());

	Components::SpotLight* SpotLight = Selection.Entity->GetComponent<Components::SpotLight>();
	if (SpotLight != nullptr)
		SpotLight->GetEntity()->GetTransform()->SetScale(SpotLight->GetRange());

	Components::SurfaceLight* SurfaceLight = Selection.Entity->GetComponent<Components::SurfaceLight>();
	if (SurfaceLight != nullptr)
		SurfaceLight->GetEntity()->GetTransform()->SetScale(SurfaceLight->GetRange());

	Components::Decal* Decal = Selection.Entity->GetComponent<Components::Decal>();
	if (Decal != nullptr)
		Decal->GetEntity()->GetTransform()->SetScale(Decal->GetRange());
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

	if (Value->GetComponent<Components::SurfaceLight>() ||
		Value->GetComponent<Components::Illuminator>())
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
uint64_t Sandbox::GetEntityNesting(Entity* Value)
{
	uint64_t Top = 0;
	if (!Value)
		return Top;

	Entity* Base = Value;
	while ((Base = Base->GetParent()) != nullptr)
		Top++;

	return Top;
}
void* Sandbox::GetEntityIndex(Entity* Value)
{
	if (!Value)
		return nullptr;

	if (Value->GetParent() != nullptr)
		return (void*)Value->GetParent();

	return (void*)Value;
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

	if (Value->GetComponent<Components::SurfaceLight>())
		return "[Surface light]";

	if (Value->GetComponent<Components::Illuminator>())
		return "[Illuminator]";

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
		return "[Empty] unknown";

	if (Value->GetName().empty())
		Result = GetLabel(Value);
	else
		Result = GetLabel(Value) + " " + Value->GetName();

	auto* Scriptable = Value->GetComponent<Components::Scriptable>();
	if (Scriptable != nullptr && !Scriptable->GetSource().empty())
	{
		const std::string& Module = Scriptable->GetName();
		Result += " " + (Module.empty() ? "anonymous" : Module);
	}
	else if (Value->GetName().empty())
		Result += " unnamed";

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
