#ifndef SANDBOX_H
#define SANDBOX_H
#include "../core/globals.h"
#include "../utils/gizmo.h"

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

class Sandbox : public Application
{
public:
    struct
    {
		uint64_t Move = 0;
        FileTree* Directory = nullptr;
        Material* Material = nullptr;
		Entity* Might = nullptr;
        Entity* Entity = nullptr;
        IGizmo* Gizmo = nullptr;
        Inspector Window = Inspector_None;
    } Selection;

    struct
    {
		std::function<void(const String&)> OnResource;
		std::function<void(Entity*)> OnEntity;
		GUI::DataModel* System = nullptr;
		GUI::DataNode* Entities = nullptr;
		GUI::DataNode* Materials = nullptr;
		GUI::DataNode* Models = nullptr;
		GUI::DataNode* Skins = nullptr;
		GUI::DataNode* Directories = nullptr;
		GUI::DataNode* Files = nullptr;
		GUI::Context* GUI = nullptr;
		FileTree* Directory = nullptr;
		Entity* Camera = nullptr;
		Entity* Draggable = nullptr;
		Transform::Spacing Space;
		String Status;
		String Filename;
		String Target;
        Vector2 Cursor;
        Matrix4x4 Gizmo;
        float GizmoScale = 1.0f;
		float Frames = 0.0f;
        bool IsPathTracked = false;
        bool IsSceneFocused = false;
        bool IsCameraActive = false;
		bool IsTraceMode = false;
		bool IsDraggable = false;
		bool IsDragHovered = false;
		bool IsInteractive = false;
		bool IsCaptured = false;
		bool IsMounted = false;
		uint32_t MeshImportOpts = 0;
		int ElementsLimit = 0;
    } State;

    struct
    {
		IGizmo* Gizmo[3] = { nullptr };
        String CurrentPath;
        String NextPath;
		String ScenePath;
    } Resource;

	struct
	{
		Texture2D* Sandbox = nullptr;
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
		Surface* Sandbox = nullptr;
	} Favicons;

	struct
	{
		DepthStencilState* DepthStencil = nullptr;
		RasterizerState* NoneRasterizer = nullptr;
		RasterizerState* BackRasterizer = nullptr;
		BlendState* Blend = nullptr;
		InputLayout* Layout = nullptr;
	} States;

public:
	explicit Sandbox(Application::Desc* Conf, const String& Path = "");
	~Sandbox() override;
	void KeyEvent(KeyCode Key, KeyMod Mod, int Virtual, int Repeat, bool Pressed) override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void Initialize() override;
	void Dispatch(Timer* Time) override;
	void Publish(Timer* Time) override;
	void LoadCamera();
	void UnloadCamera();
    void UpdateProject();
    void UpdateScene();
	void UpdateGrid(Timer* Time);
	void UpdateMutation(const std::string_view& Name, VariantArgs& Args);
	void UpdateSystem();
	void InspectEntity();
	void InspectSettings();
	void InspectImporter();
	void InspectMaterial();
	void SetLogging(bool Active);
	void SetViewModel();
	void SetDirectory(FileTree* Base);
	void SetContents(FileTree* Base, int64_t Depth = 0);
	void SetSelection(Inspector Window, void* Object = nullptr);
	void SetStatus(const String& Status);
	void SetMutation(Entity* Parent, const std::string_view& Type);
	void SetMetadata(Entity* Source);
    void GetPathName(String& Path);
    void GetEntityCell();
    void GetEntitySync();
	void GetResource(const String& Name, const std::function<void(const String&)>& Callback);
	void GetEntity(const String& Name, const std::function<void(Entity*)>& Callback);
	bool GetSceneFocus();
	bool GetResourceState(const String& Name);
	bool GetEntityState(const String& Name);
	bool GetSelectionState();
	Texture2D* GetIcon(Entity* Value);
	void* GetEntityIndex(Entity* Value);
	uint64_t GetEntityNesting(Entity* Value);
    String GetLabel(Entity* Value);
    String GetName(Entity* Value);
	String GetPascal(const String& Value);
};
#endif