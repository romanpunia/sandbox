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
        FileTree* Folder;
        Material* Material;
		Entity* Might;
        Entity* Entity;
        IGizmo* Gizmo;
        uint64_t Move;
        Inspector Window;
    } Selection;

    struct
    {
		std::function<void(const std::string&)> OnResource;
		std::function<void(Entity*)> OnEntity;
		std::vector<std::pair<std::string, int>> Logs;
		std::string Resource;
		std::string Status;
		std::string Filename;
		std::string Target;
		GUI::Context* GUI;
		GUI::DrawLabel* Label;
        FileTree* Asset;
        Entity* Camera;
		Entity* Draggable;
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
		Document* Layout;
		Document* Style;
        IGizmo* Gizmo[3];
        std::string CurrentPath;
        std::string NextPath;
		std::string ScenePath;
    } Resource;

	struct
	{
		Texture2D* Empty;
		Texture2D* Animation;
		Texture2D* Body;
		Texture2D* Camera;
		Texture2D* Decal;
		Texture2D* Mesh;
		Texture2D* Motion;
		Texture2D* Light;
		Texture2D* Probe;
		Texture2D* Listener;
		Texture2D* Source;
		Texture2D* Emitter;
	} Icons;

	struct
	{
		DepthStencilState* DepthStencil;
		RasterizerState* NoneRasterizer;
		RasterizerState* BackRasterizer;
		BlendState* Blend;
		SamplerState* Sampler;
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