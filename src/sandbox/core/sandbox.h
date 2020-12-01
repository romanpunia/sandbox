#ifndef SANDBOX_H
#define SANDBOX_H
#include "../app.h"
#include "../cursor/gizmo.h"

enum Inspector
{
	Inspector_None,
	Inspector_Entity,
	Inspector_Material,
	Inspector_Settings,
	Inspector_Materials,
	Inspector_ImportModel,
	Inspector_ImportAnimation
};

struct IFile
{
	std::string Path;
	std::string Name;
};

class Sandbox : public Application
{
public:
    struct
    {
        FileTree* Directory = nullptr;
        Material* Material = nullptr;
		Entity* Might = nullptr;
        Entity* Entity = nullptr;
        IGizmo* Gizmo = nullptr;
        uint64_t Move;
        Inspector Window;
    } Selection;

    struct
    {
		std::function<void(const std::string&)> OnResource;
		std::function<void(Entity*)> OnEntity;
		GUI::Context* GUI = nullptr;
		FileTree* Directory;
		Entity* Camera = nullptr;
		Entity* Draggable = nullptr;
		std::string Status;
		std::string Filename;
		std::string Target;
        Vector2 Cursor;
        Matrix4x4 Gizmo;
        float GizmoScale;
		float Frames = 0.0f;
        bool IsPathTracked;
        bool IsSceneFocused;
        bool IsCameraActive;
		bool IsTraceMode;
		bool IsDraggable;
		bool IsDragHovered;
		bool IsInteractive;
		bool IsOutdated;
		bool IsCaptured;
		uint32_t MeshImportOpts;
		int ElementsLimit;
    } State;

	struct
	{
		GUI::DataSource* Hierarchy = nullptr;
		GUI::DataSource* Project = nullptr;
		GUI::DataSource* Files = nullptr;
		GUI::DataSource* Materials = nullptr;
		GUI::DataSource* Surfaces = nullptr;
		GUI::DataSource* Model = nullptr;
		GUI::DataSource* Skin = nullptr;
		GUI::DataModel* System = nullptr;
	} Models;

    struct
    {
		IGizmo* Gizmo[3] = { nullptr };
        std::string CurrentPath;
        std::string NextPath;
		std::string ScenePath;
    } Resource;

	struct
	{
		Texture2D* Empty = nullptr;
		Texture2D* Animation = nullptr;
		Texture2D* Body = nullptr;
		Texture2D* Camera = nullptr;
		Texture2D* Decal = nullptr;
		Texture2D* Mesh = nullptr;
		Texture2D* Motion = nullptr;
		Texture2D* Light = nullptr;
		Texture2D* Probe = nullptr;
		Texture2D* Listener = nullptr;
		Texture2D* Source = nullptr;
		Texture2D* Emitter = nullptr;
	} Icons;

	struct
	{
		DepthStencilState* DepthStencil = nullptr;
		RasterizerState* NoneRasterizer = nullptr;
		RasterizerState* BackRasterizer = nullptr;
		BlendState* Blend = nullptr;
		SamplerState* Sampler = nullptr;
	} States;

public:
	explicit Sandbox(Application::Desc* Conf, const std::string& Path = "");
	virtual ~Sandbox() override;
	void KeyEvent(KeyCode Key, KeyMod Mod, int Virtual, int Repeat, bool Pressed) override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void ScriptHook(VMGlobal* Global) override;
	void Initialize(Application::Desc* Conf) override;
	void Render(Timer* Time) override;
	void Update(Timer* Time) override;
	void UpdateHierarchy();
    void UpdateProject();
    void UpdateScene();
	void UpdateGrid(Timer* Time);
	void UpdateJoint(PoseBuffer* Map, Joint* Base, Matrix4x4* World);
	void UpdateSystem();
	void InspectEntity();
	void InspectSettings();
	void InspectImporter();
	void InspectMaterial();
	void SetViewModel();
	void SetDirectory(FileTree* Base);
	void SetSelection(Inspector Window, void* Object = nullptr);
	void SetStatus(const std::string& Status);
    void GetPathName(std::string& Path);
    void GetEntityCell();
    void GetEntitySync();
	void GetResource(const std::string& Name, const std::function<void(const std::string&)>& Callback);
	void GetEntity(const std::string& Name, const std::function<void(Entity*)>& Callback);
	void* GetGUI() override;
	bool GetSceneFocus();
	bool GetResourceState(const std::string& Name);
	bool GetEntityState(const std::string& Name);
	bool GetSelectionState();
	Texture2D* GetIcon(Entity* Value);
    std::string GetLabel(Entity* Value);
    std::string GetName(Entity* Value);
	std::string GetPascal(const std::string& Value);
};
#endif