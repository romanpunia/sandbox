#include <tomahawk/tomahawk.h>
#include <IGizmo.h>
#include <LibBase.h>
#include <GizmoTransformRender.h>
#include <GizmoTransformMove.h>
#include <GizmoTransformRotate.h>
#include <GizmoTransformScale.h>
#include <sstream>
#include <fstream>

using namespace Tomahawk::Rest;
using namespace Tomahawk::Audio;
using namespace Tomahawk::Compute;
using namespace Tomahawk::Engine;
using namespace Tomahawk::Engine::GUI;
using namespace Tomahawk::Network;
using namespace Tomahawk::Graphics;
using namespace Tomahawk::Script;

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

class Demo : public Application
{
private:
	static std::string Source;

public:
	explicit Demo(Application::Desc* Conf);
	~Demo() override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void ScriptHook(VMGlobal* Global) override;
	void Initialize(Application::Desc* Conf) override;
	void Update(Timer* Time) override;
	void Render(Timer* Time) override;

public:
	static void SetSource(const std::string& Resource);
	static std::string& GetSource();
};

class Sandbox : public Application
{
public:
    struct
    {
        FileTree* Folder = nullptr;
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
		std::vector<std::pair<std::string, int>> Logs;
		GUI::Context* GUI = nullptr;
		GUI::DrawLabel* Label = nullptr;
		FileTree* Asset = nullptr;
		Entity* Camera = nullptr;
		Entity* Draggable = nullptr;
		std::string Resource;
		std::string Status;
		std::string Filename;
		std::string Target;
        Vector2 Cursor;
        Matrix4x4 Gizmo;
        float GizmoScale;
		float Frames;
        bool IsPathTracked;
        bool IsSceneFocused;
        bool IsCameraActive;
		bool IsTraceMode;
		bool IsDraggable;
		bool IsDragHovered;
		bool IsInteractive;
		uint32_t MeshImportOpts;
		int ElementsLimit;
    } State;

    struct
    {
		Document* Layout = nullptr;
		Document* Style = nullptr;
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
	explicit Sandbox(Application::Desc* Conf);
	~Sandbox() override;
	void KeyEvent(KeyCode Key, KeyMod Mod, int Virtual, int Repeat, bool Pressed) override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void ScriptHook(VMGlobal* Global) override;
	void Initialize(Application::Desc* Conf) override;
	void Render(Timer* Time) override;
	void Update(Timer* Time) override;
	void UpdateGUI();
    void UpdateHierarchy();
    void UpdateScene();
	void UpdateGrid(Timer* Time);
	void UpdateJoint(PoseBuffer* Map, Joint* Base, Matrix4x4* World);
	void SetSelection(Inspector Window = Inspector_None);
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

int main()
{
    Tomahawk::Initialize();
Entry:
	{
		Application::Desc Init;
		Init.GraphicsDevice.VSyncMode = VSync_Frequency_X1;
		Init.GraphicsDevice.Backend = RenderBackend_D3D11;
		Init.GraphicsDevice.Debug = false;
		Init.Activity.FreePosition = true;
		Init.Activity.Title = "Sandbox";
		Init.Activity.Hidden = true;
		Init.Directory = "sandbox";

		Application* App = new Sandbox(&Init);
		App->Run(&Init);
		delete App;

		if (Demo::GetSource().empty())
			goto Exit;

		Init.Activity.Title = "Demo";
		Init.Activity.Hidden = false;
		Init.Activity.Maximized = true;
		Init.Directory = "sandbox";
		Init.Threading = EventWorkflow_Multithreaded;

		App = new Demo(&Init);
		App->Run(&Init);
		delete App;

		goto Entry;
	}
Exit:
    Tomahawk::Uninitialize();

	return 0;
}