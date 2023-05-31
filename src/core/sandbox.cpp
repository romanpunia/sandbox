#include "sandbox.h"
#include "demo.h"
#include "../controller/resolvers.h"
#include "../controller/components.h"
#include "../controller/renderers.h"
#include "../controller/effects.h"

Sandbox::Sandbox(Application::Desc* Conf, const String& Path) : Application(Conf)
{
	OS::Directory::SetWorking(OS::Directory::GetModule().c_str());
	Resource.NextPath = Path;
#ifdef _DEBUG
	Console::Get()->Show();
#endif
}
Sandbox::~Sandbox()
{
	SetLogging(false);
	VI_RELEASE(State.GUI);
	VI_RELEASE(State.Directory);
	VI_RELEASE(Icons.Empty);
	VI_RELEASE(Icons.Animation);
	VI_RELEASE(Icons.Body);
	VI_RELEASE(Icons.Camera);
	VI_RELEASE(Icons.Decal);
	VI_RELEASE(Icons.Mesh);
	VI_RELEASE(Icons.Motion);
	VI_RELEASE(Icons.Light);
	VI_RELEASE(Icons.Probe);
	VI_RELEASE(Icons.Listener);
	VI_RELEASE(Icons.Source);
	VI_RELEASE(Icons.Emitter);
	VI_RELEASE(Icons.Sandbox);
	VI_RELEASE(Favicons.Sandbox);
	delete (CGizmoTransformMove*)Resource.Gizmo[0];
	delete (CGizmoTransformRotate*)Resource.Gizmo[1];
	delete (CGizmoTransformScale*)Resource.Gizmo[2];
}
void Sandbox::KeyEvent(KeyCode Key, KeyMod, int, int, bool Pressed)
{
	if (Key == KeyCode::CursorLeft && Selection.Gizmo)
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
void Sandbox::Initialize()
{
	States.DepthStencil = Renderer->GetDepthStencilState("none");
	States.NoneRasterizer = Renderer->GetRasterizerState("cull-none");
	States.BackRasterizer = Renderer->GetRasterizerState("cull-back");
	States.Blend = Renderer->GetBlendState("additive");
	States.Layout = Renderer->GetInputLayout("shape-vertex");
	Icons.Sandbox = Content->Load<Texture2D>("editor/img/sandbox.png");
	Icons.Empty = Content->Load<Texture2D>("editor/img/empty.png");
	Icons.Animation = Content->Load<Texture2D>("editor/img/animation.png");
	Icons.Body = Content->Load<Texture2D>("editor/img/body.png");
	Icons.Camera = Content->Load<Texture2D>("editor/img/camera.png");
	Icons.Decal = Content->Load<Texture2D>("editor/img/decal.png");
	Icons.Mesh = Content->Load<Texture2D>("editor/img/mesh.png");
	Icons.Motion = Content->Load<Texture2D>("editor/img/motion.png");
	Icons.Light = Content->Load<Texture2D>("editor/img/light.png");
	Icons.Probe = Content->Load<Texture2D>("editor/img/probe.png");
	Icons.Listener = Content->Load<Texture2D>("editor/img/listener.png");
	Icons.Source = Content->Load<Texture2D>("editor/img/source.png");
	Icons.Emitter = Content->Load<Texture2D>("editor/img/emitter.png");
	Favicons.Sandbox = Renderer->CreateSurface(Icons.Sandbox);
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
	State.MeshImportOpts = (uint32_t)Processors::MeshPreset::Default;
	Selection.Directory = nullptr;

	if (!State.GUI->Initialize("editor/conf.xml"))
	{
		VI_ERR("could not load GUI");
		return Stop();
	}

	Resource.NextPath = "./scenes/demo.xml";
	ErrorHandling::SetFlag(LogOption::Async, false);
	Demo::SetSource("");
	SetLogging(true);
	UpdateScene();
	UpdateProject();
	SetStatus("Initialization done");

	Activity->SetIcon(Favicons.Sandbox);
	Activity->Callbacks.CursorMove = [this](int X, int Y, int RX, int RY)
	{
		State.Cursor = Activity->GetCursorPosition();
		if (Selection.Gizmo)
			Selection.Gizmo->OnMouseMove(X, Y);
	};
}
void Sandbox::Dispatch(Timer* Time)
{
#ifdef _DEBUG
	if (Activity->IsKeyDownHit(KeyCode::F5))
	{
		if (State.GUI != nullptr)
		{
			State.GUI->ClearDocuments();
			State.GUI->ClearStyles();
			State.GUI->Initialize("editor/conf.xml");
		}
	}
#endif
	if (Activity->IsKeyDownHit(KeyCode::F6))
		State.IsInteractive = !State.IsInteractive;

	if (!State.IsCameraActive && Scene->GetCamera()->GetEntity() == State.Camera)
		State.IsCameraActive = true;

	if (State.Camera != nullptr)
	{
		bool Active = (!Scene->IsActive() && GetSceneFocus());
		State.Camera->GetComponent<Components::Fly>()->SetActive(Active);
		State.Camera->GetComponent<Components::FreeLook>()->SetActive(Active);
		if (Active)
		{
			for (auto& Item : *State.Camera)
				Item.second->Update(Time);
		}
	}

	if (State.IsInteractive)
	{
		State.Frames = (float)Time->GetFrames();
		if (!Resource.NextPath.empty())
			return UpdateScene();

		UpdateSystem();
		if (State.GUI != nullptr)
			State.GUI->UpdateEvents(Activity);
	}

	if (!State.IsCaptured)
	{
		if (State.IsInteractive)
		{
			if (Activity->IsKeyDownHit(KeyCode::F1))
			{
				State.IsTraceMode = !State.IsTraceMode;
				if (!State.IsTraceMode)
					SetStatus("Tracing mode: off");
				else
					SetStatus("Tracing mode: on");
			}

			if (State.Camera != nullptr && Scene->GetCamera()->GetEntity() == State.Camera && State.IsCameraActive)
			{
				if (Selection.Entity != nullptr)
				{
					if (Activity->IsKeyDownHit(KeyCode::Delete))
					{
						SetStatus("Entity was removed");
						Scene->DeleteEntity(Selection.Entity);
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
						Space.Rotation = State.Gizmo.RotationEuler();
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
					if (Activity->IsKeyDownHit(KeyCode::Delete))
					{
						SetStatus("Entity was removed");
						Scene->DeleteEntity(Selection.Entity);
						SetSelection(Inspector_None);
					}

					if (Activity->IsKeyDown(KeyMod::LeftControl) && GetSceneFocus())
					{
						if (Activity->IsKeyDownHit(KeyCode::V) || Activity->IsKeyDown(KeyCode::B))
						{
							SetStatus("Entity was cloned");
							Entity* Result = Scene->CloneEntity(Selection.Entity);
							SetSelection(Result ? Inspector_Entity : Inspector_None, Result);
						}

						if (Activity->IsKeyDownHit(KeyCode::X))
						{
							SetStatus("Entity was removed");
							Scene->DeleteEntity(Selection.Entity);
							SetSelection(Inspector_None);
						}
					}
				}

				if (Activity->IsKeyDown(KeyMod::LeftControl))
				{
					if (Activity->IsKeyDown(KeyCode::N))
					{
						Entity* Last = Scene->GetLastEntity();
						if (Last != nullptr && Last != State.Camera)
						{
							if (Selection.Entity == Last)
								SetSelection(Inspector_None);

							SetStatus("Last entity was removed");
							Scene->DeleteEntity(Last);
						}
					}

					if (Activity->IsKeyDown(KeyCode::X))
					{
						if (Selection.Entity != nullptr)
						{
							SetStatus("Entity was removed");
							Scene->DeleteEntity(Selection.Entity);
							SetSelection(Inspector_None);
						}
					}
				}
			}
		}
	}
	else
		State.IsCaptured = false;

	Scene->Dispatch(Time);
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
void Sandbox::LoadCamera()
{
	State.IsCameraActive = true;
	State.Camera = Scene->AddEntity();
	State.Camera->AddComponent<Components::Camera>();
	State.Camera->AddComponent<Components::FreeLook>();
	State.Camera->GetTransform()->SetSpacing(Positioning::Global, State.Space);

	auto* Fly = State.Camera->AddComponent<Components::Fly>();
	Fly->Moving.Slower *= 0.25f;
	Fly->Moving.Normal *= 0.35f;
	Scene->SetCamera(State.Camera);

	auto* fRenderer = Scene->GetRenderer();
	fRenderer->AddRenderer<Renderers::Model>();
	fRenderer->AddRenderer<Renderers::Skin>();
	fRenderer->AddRenderer<Renderers::SoftBody>();
	fRenderer->AddRenderer<Renderers::Emitter>();
	fRenderer->AddRenderer<Renderers::Decal>();
	fRenderer->AddRenderer<Renderers::Lighting>();
	fRenderer->AddRenderer<Renderers::Transparency>();
}
void Sandbox::UnloadCamera()
{
	State.IsCameraActive = false;
	if (!State.Camera)
		return;

	State.Space = State.Camera->GetTransform()->GetSpacing(Positioning::Global);
	if (Scene != nullptr)
		Scene->DeleteEntity(State.Camera);
	State.Camera = nullptr;
}
void Sandbox::UpdateProject()
{
	SetStatus("Project's hierarchy was updated");
	String Directory = "";
	if (Selection.Directory != nullptr)
	{
		Directory = Selection.Directory->Path;
		Selection.Directory = nullptr;
	}

	VI_RELEASE(State.Directory);
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
	UnloadCamera();

	State.Entities->Clear();
	State.Materials->Clear();
	if (Scene != nullptr)
		VI_CLEAR(Scene);

	VM->ClearCache();
	if (!Resource.NextPath.empty())
	{
		VariantArgs Args;
		Args["active"] = Var::Boolean(false);
		Args["mutations"] = Var::Boolean(true);

		Scene = Content->Load<SceneGraph>(Resource.NextPath, Args);
	}

	if (Scene == nullptr)
	{
		SceneGraph::Desc I = SceneGraph::Desc::Get(this);
		I.Simulator.EnableSoftBody = true;
		I.Mutations = true;

		Scene = new SceneGraph(I);
		Scene->SetActive(false);

		SetStatus("Created new scene from scratch");
	}
	else
		SetStatus("Scene was loaded");

	Scene->SetListener("mutation", std::bind(&Sandbox::UpdateMutation, this, std::placeholders::_1, std::placeholders::_2));
	Resource.ScenePath = Resource.NextPath;
	Resource.NextPath.clear();
	LoadCamera();
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
	Renderer->SetShader(Constants->GetBasicEffect(), VI_VS | VI_PS);
	Renderer->SetVertexBuffer(Cache.Primitives->GetQuad());

	for (uint32_t i = 0; i < Scene->GetEntitiesCount(); i++)
	{
		Entity* Value = Scene->GetEntity(i);
		if (State.Camera == Value)
			continue;

		auto& From = Value->GetTransform()->GetPosition();
		auto& To = State.Camera->GetTransform()->GetPosition();
		float Direction = Vector2(From.X, From.Z).LookAt(Vector2(To.X, To.Z));
		Constants->Render.TexCoord = (Value == Selection.Entity ? 0.5f : 0.05f);
		Constants->Render.Transform = Matrix4x4::Create(Value->GetTransform()->GetPosition(), 0.5f, Vector3(0, Direction)) * State.Camera->GetComponent<Components::Camera>()->GetViewProjection();
		Constants->UpdateConstantBuffer(RenderBufferType::Render);
		Renderer->SetTexture2D(GetIcon(Value), 1, VI_PS);
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
			if (KeyAnimator != nullptr && KeyAnimator->IsExists(KeyAnimator->State.Clip))
			{
				Vector<AnimatorKey>* Keys = KeyAnimator->GetClip(KeyAnimator->State.Clip);
				if (Keys != nullptr)
				{
					Matrix4x4 Offset = (Value->GetTransform()->GetRoot() ? Value->GetTransform()->GetRoot()->GetBias() : Matrix4x4::Identity());
					for (size_t j = 0; j < Keys->size(); j++)
					{
						auto& Pos = Keys->at(j).Position;
						Renderer->ImBegin();
						Renderer->ImTopology(PrimitiveTopology::Line_Strip);
						Renderer->ImTransform(Offset * ((Components::Camera*)Scene->GetCamera())->GetViewProjection());
						Renderer->ImEmit();
						Renderer->ImPosition(Pos.X, Pos.Y, -Pos.Z);
						Renderer->ImEmit();

						if (KeyAnimator->State.Frame == j)
							Renderer->ImColor(1, 0, 1, 1);

						if (j + 1 >= Keys->size())
						{
							auto& xPos = Keys->at(0).Position;
							Renderer->ImPosition(xPos.X, xPos.Y, -xPos.Z);
						}
						else
						{
							auto& xPos = Keys->at(j + 1).Position;
							Renderer->ImPosition(xPos.X, xPos.Y, -xPos.Z);
						}

						Renderer->ImEnd();
					}
				}
			}
		}

		Matrix4x4 Transform = Value->GetBox();
		for (int j = 0; j < 4; j++)
		{
			Renderer->ImBegin();
			Renderer->ImTopology(PrimitiveTopology::Line_Strip);
			Renderer->ImTransform(Origin[j] * Transform * ((Components::Camera*)Scene->GetCamera())->GetViewProjection());
			Renderer->ImEmit();
			if (Value == Selection.Might)
				Renderer->ImColor(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->ImColor(1, 1, 1, 0.1f);
			else
				Renderer->ImColor(1, 1, 1, 0.5f);
			Renderer->ImPosition(1, 1, 1);

			Renderer->ImEmit();
			if (Value == Selection.Might)
				Renderer->ImColor(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->ImColor(1, 1, 1, 0.1f);
			else
				Renderer->ImColor(1, 1, 1, 0.5f);
			Renderer->ImPosition(-1, 1, 1);

			Renderer->ImEmit();
			if (Value == Selection.Might)
				Renderer->ImColor(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->ImColor(1, 1, 1, 0.1f);
			else
				Renderer->ImColor(1, 1, 1, 0.5f);
			Renderer->ImPosition(-1, -1, 1);

			Renderer->ImEmit();
			if (Value == Selection.Might)
				Renderer->ImColor(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->ImColor(1, 1, 1, 0.1f);
			else
				Renderer->ImColor(1, 1, 1, 0.5f);
			Renderer->ImPosition(1, -1, 1);

			Renderer->ImEmit();
			if (Value == Selection.Might)
				Renderer->ImColor(1, 1, 0.75f, 0.15f);
			else if (Value != Selection.Entity)
				Renderer->ImColor(1, 1, 1, 0.1f);
			else
				Renderer->ImColor(1, 1, 1, 0.5f);
			Renderer->ImPosition(1, 1, 1);
			Renderer->ImEnd();
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
				Renderer->ImBegin();
				Renderer->ImTopology(PrimitiveTopology::Line_Strip);
				Renderer->ImTransform(j * Scene->GetEntity(i)->GetTransform()->GetBias() * ViewProjection);
				Renderer->ImEmit();
				Renderer->ImColor(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->ImPosition(1.0f, 1.0f, 1.0f);
				Renderer->ImEmit();
				Renderer->ImColor(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->ImPosition(-1.0f, 1.0f, 1.0f);
				Renderer->ImEmit();
				Renderer->ImColor(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->ImPosition(-1.0f, -1.0f, 1.0f);
				Renderer->ImEmit();
				Renderer->ImColor(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->ImPosition(1.0f, -1.0f, 1.0f);
				Renderer->ImEmit();
				Renderer->ImColor(0.5f, 0.5f, 1.0f, 0.75f);
				Renderer->ImPosition(1.0f, 1.0f, 1.0f);
				Renderer->ImEnd();
			}
		}
	}
}
void Sandbox::UpdateMutation(const String& Name, VariantArgs& Args)
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
		Item.emplace_back(std::move(Var::String(Base->GetName().empty() ? "Material #" + ToString(Base->Slot) : Base->GetName())));
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

	String Name = Base->GetName();
	if (State.GUI->GetElementById("ent_name").CastFormString(&Name))
		Base->SetName(Name);

	auto& Space = Base->GetTransform()->GetSpacing();
	if (State.GUI->GetElementById("ent_pos_x").CastFormFloat(&Space.Position.X) ||
		State.GUI->GetElementById("ent_pos_y").CastFormFloat(&Space.Position.Y) ||
		State.GUI->GetElementById("ent_pos_z").CastFormFloat(&Space.Position.Z) ||
		State.GUI->GetElementById("ent_rot_x").CastFormFloat(&Space.Rotation.X, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById("ent_rot_y").CastFormFloat(&Space.Rotation.Y, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById("ent_rot_z").CastFormFloat(&Space.Rotation.Z, Mathf::Rad2Deg()) ||
		State.GUI->GetElementById("ent_scale_x").CastFormFloat(&Space.Scale.X) ||
		State.GUI->GetElementById("ent_scale_y").CastFormFloat(&Space.Scale.Y) ||
		State.GUI->GetElementById("ent_scale_z").CastFormFloat(&Space.Scale.Z))
	{
		Base->GetTransform()->MakeDirty();
		GetEntitySync();
	}

	bool Scaling = Base->GetTransform()->HasScaling();
	if (State.GUI->GetElementById("ent_const_scale").CastFormBoolean(&Scaling))
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

		if (State.System->SetInteger("sl_cmp_camera_ssgi", fRenderer->GetOffset<Renderers::SSGI>())->GetInteger() >= 0)
			RendererSSGI(State.GUI, fRenderer->GetRenderer<Renderers::SSGI>());

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
	State.GUI->GetElementById("sc_start_grav_x").CastFormFloat(&Conf.Simulator.Gravity.X);
	State.GUI->GetElementById("sc_start_grav_y").CastFormFloat(&Conf.Simulator.Gravity.Y);
	State.GUI->GetElementById("sc_start_grav_z").CastFormFloat(&Conf.Simulator.Gravity.Z);
	State.GUI->GetElementById("sc_wet_norm_x").CastFormFloat(&Conf.Simulator.WaterNormal.X);
	State.GUI->GetElementById("sc_wet_norm_y").CastFormFloat(&Conf.Simulator.WaterNormal.Y);
	State.GUI->GetElementById("sc_wet_norm_z").CastFormFloat(&Conf.Simulator.WaterNormal.Z);
	State.GUI->GetElementById("sc_air_dens").CastFormFloat(&Conf.Simulator.AirDensity);
	State.GUI->GetElementById("sc_wet_dens").CastFormFloat(&Conf.Simulator.WaterDensity);
	State.GUI->GetElementById("sc_wet_off").CastFormFloat(&Conf.Simulator.WaterOffset);
	State.GUI->GetElementById("sc_max_disp").CastFormFloat(&Conf.Simulator.MaxDisplacement);
	State.GUI->GetElementById("sc_softs").CastFormBoolean(&Conf.Simulator.EnableSoftBody);
	State.GUI->GetElementById("sc_gp_qual").CastFormFloat(&Conf.RenderQuality, 100.0f);
	State.GUI->GetElementById("sc_gp_hdr").CastFormBoolean(&Conf.EnableHDR);
	State.GUI->GetElementById("sc_start_mats").CastFormSize(&Conf.StartEntities);
	State.GUI->GetElementById("sc_start_ents").CastFormSize(&Conf.StartComponents);
	State.GUI->GetElementById("sc_start_comps").CastFormSize(&Conf.StartMaterials);
	State.GUI->GetElementById("sc_grow_marg").CastFormSize(&Conf.GrowMargin);
	State.GUI->GetElementById("sc_grow_rate").CastFormDouble(&Conf.GrowRate);
	State.GUI->GetElementById("sc_vp_br").CastFormSize(&Conf.VoxelsSize);
	State.GUI->GetElementById("sc_vp_bl").CastFormSize(&Conf.VoxelsMax);
	State.GUI->GetElementById("sc_sp_plr").CastFormSize(&Conf.PointsSize);
	State.GUI->GetElementById("sc_sp_pll").CastFormSize(&Conf.PointsMax);
	State.GUI->GetElementById("sc_sp_slr").CastFormSize(&Conf.SpotsSize);
	State.GUI->GetElementById("sc_sp_sll").CastFormSize(&Conf.SpotsMax);
	State.GUI->GetElementById("sc_sp_llr").CastFormSize(&Conf.LinesSize);
	State.GUI->GetElementById("sc_sp_lll").CastFormSize(&Conf.LinesMax);

	Vector3 Gravity = Scene->GetSimulator()->GetGravity();
	if (State.GUI->GetElementById("sc_sim_grav_x").CastFormFloat(&Gravity.X) ||
		State.GUI->GetElementById("sc_sim_grav_y").CastFormFloat(&Gravity.Y) ||
		State.GUI->GetElementById("sc_sim_grav_z").CastFormFloat(&Gravity.Z))
		Scene->GetSimulator()->SetGravity(Gravity);

	State.GUI->GetElementById("sc_sim_time").CastFormFloat(&Scene->GetSimulator()->Speedup);
	State.GUI->GetElementById("sc_sim").CastFormBoolean(&Scene->GetSimulator()->Active);
}
void Sandbox::InspectImporter()
{
	State.GUI->GetElementById("im_jiv").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::JoinIdenticalVertices);
	State.GUI->GetElementById("im_tri").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::Triangulate);
	State.GUI->GetElementById("im_cts").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::CalcTangentSpace);
	State.GUI->GetElementById("im_rc").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::RemoveComponent);
	State.GUI->GetElementById("im_mn").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::GenNormals);
	State.GUI->GetElementById("im_msn").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::GenSmoothNormals);
	State.GUI->GetElementById("im_slm").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::SplitLargeMeshes);
	State.GUI->GetElementById("im_ptv").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::PreTransformVertices);
	State.GUI->GetElementById("im_vds").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::ValidateDataStructure);
	State.GUI->GetElementById("im_mlh").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::MakeLeftHanded);
	State.GUI->GetElementById("im_icl").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::ImproveCacheLocality);
	State.GUI->GetElementById("im_rrm").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::RemoveRedundantMaterials);
	State.GUI->GetElementById("im_fin").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::FixInfacingNormals);
	State.GUI->GetElementById("im_sbt").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::SortByPType);
	State.GUI->GetElementById("im_rd").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::RemoveDegenerates);
	State.GUI->GetElementById("im_ri").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::RemoveInstances);
	State.GUI->GetElementById("im_rid").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::RemoveInvalidData);
	State.GUI->GetElementById("im_muv").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::GenUVCoords);
	State.GUI->GetElementById("im_tuv").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::TransformUVCoords);
	State.GUI->GetElementById("im_om").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::OptimizeMeshes);
	State.GUI->GetElementById("im_og").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::OptimizeGraph);
	State.GUI->GetElementById("im_fuv").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::FlipUVs);
	State.GUI->GetElementById("im_fwo").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::FlipWindingOrder);
	State.GUI->GetElementById("im_sbb").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::SplitByBoneCount);
	State.GUI->GetElementById("im_dbn").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::Debone);
	State.GUI->GetElementById("im_gs").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::GlobalScale);
	State.GUI->GetElementById("im_fmn").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::ForceGenNormals);
	State.GUI->GetElementById("im_et").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::EmbedTextures);
	State.GUI->GetElementById("im_dn").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::DropNormals);
	State.GUI->GetElementById("im_cbb").CastFormFlag32(&State.MeshImportOpts, (uint32_t)Processors::MeshOpt::GenBoundingBoxes);
}
void Sandbox::InspectMaterial()
{
	Material* Base = Selection.Material;
	if (!Base)
		return;

	ResolveTexture2D(State.GUI, "mat_diffuse_source", Base->GetDiffuseMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetDiffuseMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_normal_source", Base->GetNormalMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetNormalMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_metallic_source", Base->GetMetallicMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetMetallicMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_roughness_source", Base->GetRoughnessMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetRoughnessMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_height_source", Base->GetHeightMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetHeightMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_occlusion_source", Base->GetOcclusionMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetOcclusionMap(New);
		VI_RELEASE(New);
	}, false);
	ResolveTexture2D(State.GUI, "mat_emission_source", Base->GetEmissionMap() != nullptr, [Base](Texture2D* New)
	{
		Base->SetEmissionMap(New);
		VI_RELEASE(New);
	}, false);

	ResolveColor3(State.GUI, "mat_diffuse", &Base->Surface.Diffuse);
	if (State.GUI->GetElementById("mat_cemn").CastFormColor(&Base->Surface.Emission, false))
		State.GUI->GetElementById("mat_cemn_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Surface.Emission.X * 255.0f), (unsigned int)(Base->Surface.Emission.Y * 255.0f), (unsigned int)(Base->Surface.Emission.Z * 255.0f)).R());

	if (State.GUI->GetElementById("mat_cmet").CastFormColor(&Base->Surface.Metallic, false))
		State.GUI->GetElementById("mat_cmet_color").SetProperty("background-color", Form("rgb(%u, %u, %u)", (unsigned int)(Base->Surface.Metallic.X * 255.0f), (unsigned int)(Base->Surface.Metallic.Y * 255.0f), (unsigned int)(Base->Surface.Metallic.Z * 255.0f)).R());

	String Name = Base->GetName();
	if (State.GUI->GetElementById("mat_name").CastFormString(&Name))
		Base->SetName(Name);

	State.GUI->GetElementById("mat_scat_x").CastFormFloat(&Base->Surface.Scatter.X);
	State.GUI->GetElementById("mat_scat_y").CastFormFloat(&Base->Surface.Scatter.Y);
	State.GUI->GetElementById("mat_scat_z").CastFormFloat(&Base->Surface.Scatter.Z);
	State.GUI->GetElementById("mat_memn").CastFormFloat(&Base->Surface.Emission.W);
	State.GUI->GetElementById("mat_mmet").CastFormFloat(&Base->Surface.Metallic.W);
	State.GUI->GetElementById("mat_rs").CastFormFloat(&Base->Surface.Roughness.X);
	State.GUI->GetElementById("mat_mrs").CastFormFloat(&Base->Surface.Roughness.Y);
	State.GUI->GetElementById("mat_occ").CastFormFloat(&Base->Surface.Occlusion.X);
	State.GUI->GetElementById("mat_mocc").CastFormFloat(&Base->Surface.Occlusion.Y);
	State.GUI->GetElementById("mat_fres").CastFormFloat(&Base->Surface.Fresnel);
	State.GUI->GetElementById("mat_refr").CastFormFloat(&Base->Surface.Refraction);
	State.GUI->GetElementById("mat_alpha").CastFormFloat(&Base->Surface.Transparency);
	State.GUI->GetElementById("mat_env").CastFormFloat(&Base->Surface.Environment);
	State.GUI->GetElementById("mat_rad").CastFormFloat(&Base->Surface.Radius);
	State.GUI->GetElementById("mat_ht_amnt").CastFormFloat(&Base->Surface.Height);
	State.GUI->GetElementById("mat_ht_bias").CastFormFloat(&Base->Surface.Bias);
}
void Sandbox::SetLogging(bool Active)
{
	if (!Active)
		return ErrorHandling::SetCallback(nullptr);

	ErrorHandling::SetCallback([this](ErrorHandling::Details& Data)
	{
		SetStatus(ErrorHandling::GetMessageText(Data));
	});
}
void Sandbox::SetViewModel()
{
	SetStatus("GUI view-model setup done");
	if (!State.GUI)
		return;

	State.System = State.GUI->SetDataModel("editor");
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
	State.System->SetInteger("sl_cmp_camera_ssgi", -1);
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

		Vector<GUI::IElement> Tabs = Target.QuerySelectorAll(".tab");
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

		String fResource = Args[0].Serialize();
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

			UnloadCamera();
			Content->Save<SceneGraph>("./editor/cache.xml", Scene, Args);
			Scene->SetActive(true);
		}
		else
			this->Resource.NextPath = "./editor/cache.xml";
	});
	State.System->SetCallback("import_model_action", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		String From;
		if (!OS::Input::Open("Import mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &From))
			return;

		if (!OS::File::IsExists(From.c_str()))
			return;

		Processors::ModelProcessor* Processor = (Processors::ModelProcessor*)Content->GetProcessor<Model>();
		if (Processor != nullptr)
		{
			Stream* File = OS::File::Open(From, FileMode::Binary_Read_Only);
			Schema* Doc = Processor->Import(File, State.MeshImportOpts);
			VI_RELEASE(File);

			if (Doc != nullptr)
			{
				String To;
				if (!OS::Input::Save("Save mesh", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized mesh (*.xml, *.json, *.jsonb)", &To))
					return;

				VariantArgs Args;
				if (Stringify(&To).EndsWith(".jsonb"))
					Args["type"] = Var::String("JSONB");
				else if (Stringify(&To).EndsWith(".json"))
					Args["type"] = Var::String("JSON");
				else
					Args["type"] = Var::String("XML");

				Content->Save<Schema>(To, Doc, Args);
				VI_RELEASE(Doc);
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
		String From;
		if (!OS::Input::Open("Import animation from mesh", Content->GetEnvironment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &From))
			return;

		if (!OS::File::IsExists(From.c_str()))
			return;

		Processors::SkinAnimationProcessor* Processor = (Processors::SkinAnimationProcessor*)Content->GetProcessor<SkinAnimation>();
		if (Processor != nullptr)
		{
			Stream* File = OS::File::Open(From, FileMode::Binary_Read_Only);
			Schema* Doc = Processor->Import(File, State.MeshImportOpts);
			VI_RELEASE(File);

			if (Doc != nullptr)
			{
				String To;
				if (!OS::Input::Save("Save animation", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized skin animation (*.xml, *.json, *.jsonb)", &To))
					return;

				VariantArgs Args;
				if (Stringify(&To).EndsWith(".jsonb"))
					Args["type"] = Var::String("JSONB");
				else if (Stringify(&To).EndsWith(".json"))
					Args["type"] = Var::String("JSON");
				else
					Args["type"] = Var::String("XML");

				Content->Save<Schema>(To, Doc, Args);
				VI_RELEASE(Doc);
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
			Selection.Entity->RemoveComponent(VI_HASH(Args[0].Serialize()));
	});
	State.System->SetCallback("open_materials", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_Materials);
	});
	State.System->SetCallback("remove_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Material != nullptr)
			Scene->DeleteMaterial(Selection.Material);
		SetSelection(Inspector_Materials);
	});
	State.System->SetCallback("copy_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Material != nullptr)
		{
			Selection.Material = Scene->CloneMaterial(Selection.Material);
			Selection.Material->SetName(Selection.Material->GetName() + "*");
		}
	});
	State.System->SetCallback("open_settings", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_Settings);
	});
	State.System->SetCallback("add_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		Material* New = Scene->AddMaterial();
	    New->SetName("Material " + ToString(Scene->GetMaterialsCount() + 1));
		SetSelection(Inspector_Material, New);
	});
	State.System->SetCallback("import_model", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_ImportModel);
	});
	State.System->SetCallback("import_skin_animation", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		SetSelection(Inspector_ImportAnimation);
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

		String Path;
		if (!OS::Input::Save("Save key animation", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized key animation (*.xml, *.json, *.jsonb)", &Path))
			return;

		Schema* Result = Var::Set::Object();
		Result->Key = "key-animation";

		auto* Animator = Selection.Entity->GetComponent<Components::KeyAnimator>();
		Series::Pack(Result, Animator->Clips);

		VariantArgs Map;
		if (Stringify(&Path).EndsWith(".jsonb"))
			Map["type"] = Var::String("JSONB");
		else if (Stringify(&Path).EndsWith(".json"))
			Map["type"] = Var::String("JSON");
		else
			Map["type"] = Var::String("XML");

		if (!Content->Save<Schema>(Path, Result, Map))
			this->Activity->Message.Setup(AlertType::Error, "Sandbox", "Key animation cannot be saved");
		else
			this->Activity->Message.Setup(AlertType::Info, "Sandbox", "Key animation was saved");

		VI_RELEASE(Result);
		this->Activity->Message.Button(AlertConfirm::Return, "OK", 1);
		this->Activity->Message.Result(nullptr);
	});
	State.System->SetCallback("import_material", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GetResource("material", [this](const String& File)
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
	State.System->SetCallback("compile_shaders", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		String Path = Content->GetEnvironment() + "./shaders";
		Path = OS::Path::Resolve(Path.c_str());
		OS::Directory::Patch(Path);

		GraphicsDevice::Desc D3D11;
		D3D11.Backend = RenderBackend::D3D11;
		D3D11.CacheDirectory = Path;

		GraphicsDevice::Desc OGL;
		OGL.Backend = RenderBackend::OGL;
		OGL.CacheDirectory = Path;

		Vector<GraphicsDevice*> Devices;
		Devices.push_back(Renderer->GetBackend() == D3D11.Backend ? Renderer : GraphicsDevice::Create(D3D11));
		Devices.push_back(Renderer->GetBackend() == OGL.Backend ? Renderer : GraphicsDevice::Create(OGL));
		this->Renderer->AddRef();

		GraphicsDevice::CompileBuiltinShaders(Devices);
		for (auto* Device : Devices)
			VI_RELEASE(Device);
	});
	State.System->SetCallback("open_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		GetResource("scene", [this](const String& File)
		{
			this->Resource.NextPath = File;
		});
	});
	State.System->SetCallback("close_scene", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		String Path;
		if (!OS::Input::Save("Save scene", Content->GetEnvironment(), "*.xml,*.json,*.jsonb", "Serialized scene (*.xml, *.json, *.jsonb)", &Path))
			return;

		VariantArgs Map;
		if (Stringify(&Path).EndsWith(".jsonb"))
			Map["type"] = Var::String("JSONB");
		else if (Stringify(&Path).EndsWith(".json"))
			Map["type"] = Var::String("JSON");
		else
			Map["type"] = Var::String("XML");

		UnloadCamera();
		Content->Save<SceneGraph>(Path, Scene, Map);
		LoadCamera();

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
	    SetSelection(Inspector_Entity, Scene->AddEntity());
	});
	State.System->SetCallback("remove_entity", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			Scene->DeleteEntity(Selection.Entity);
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
	State.System->SetCallback("add_rndr_ssgi", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (!Source)
				Source = Selection.Entity->AddComponent<Components::Camera>();

			Source->GetRenderer()->AddRenderer<Renderers::SSGI>();
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
				Source->GetRenderer()->MoveRenderer(VI_HASH(Args[0].Serialize()), -1);
		}
	});
	State.System->SetCallback("down_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->MoveRenderer(VI_HASH(Args[0].Serialize()), 1);
		}
	});
	State.System->SetCallback("remove_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
				Source->GetRenderer()->RemoveRenderer(VI_HASH(Args[0].Serialize()));
		}
	});
	State.System->SetCallback("toggle_rndr", [this](GUI::IEvent& Event, const VariantList& Args)
	{
		if (Selection.Entity != nullptr && Args.size() == 1)
		{
			auto* Source = Selection.Entity->GetComponent<Components::Camera>();
			if (Source != nullptr)
			{
				auto* fRenderer = Source->GetRenderer()->GetRenderer(VI_HASH(Args[0].Serialize()));
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
				Source->GetSource()->RemoveEffectById(VI_HASH(Args[0].Serialize()));
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
		String Name = Next;
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
		if (IsRoot && (Stringify(&Next->Path).EndsWith("editor") || Stringify(&Next->Path).EndsWith("shaders")))
			continue;

		auto Top = std::make_pair((void*)(uintptr_t)Index++, (size_t)Depth);
		String Result = Next->Path;
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
void Sandbox::SetStatus(const String& Status)
{
	State.Status = Status;
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
		if (Skin->GetDrawable())
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
void Sandbox::GetPathName(String& Path)
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
	if (!State.Camera || !GetSceneFocus() || (Selection.Gizmo && Selection.Gizmo->IsActive()))
		return;

	auto* Camera = State.Camera->GetComponent<Components::Camera>();
	bool WantChange = Activity->IsKeyDownHit(KeyCode::CursorLeft);
	Ray Cursor = Camera->GetCursorRay();
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

		Matrix4x4 Transform = Value->GetBox();
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
}
void Sandbox::GetResource(const String& Name, const std::function<void(const String&)>& Callback)
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
void Sandbox::GetEntity(const String& Name, const std::function<void(Entity*)>& Callback)
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
GUI::Context* Sandbox::GetGUI() const
{
	return State.GUI;
}
bool Sandbox::GetSceneFocus()
{
	return State.IsSceneFocused && !State.GUI->IsInputFocused();
}
bool Sandbox::GetResourceState(const String& Name)
{
	return State.Filename == Name;
}
bool Sandbox::GetEntityState(const String& Name)
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
String Sandbox::GetLabel(Entity* Value)
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
String Sandbox::GetName(Entity* Value)
{
	String Result;
	if (Value == nullptr)
		return "[Empty] unknown";

	if (Value->GetName().empty())
		Result = GetLabel(Value);
	else
		Result = GetLabel(Value) + " " + Value->GetName();

	auto* Scriptable = Value->GetComponent<Components::Scriptable>();
	if (Scriptable != nullptr && !Scriptable->GetSource().empty())
	{
		const String& Module = Scriptable->GetName();
		Result += " " + (Module.empty() ? "anonymous" : Module);
	}
	else if (Value->GetName().empty())
		Result += " unnamed";

	return Result;
}
String Sandbox::GetPascal(const String& Value)
{
	String Result;
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
