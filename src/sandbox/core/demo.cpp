#include "demo.h"

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
}
void Demo::Initialize(Application::Desc* Conf)
{
	Scene = Content->Load<SceneGraph>(Source);
	if (!Scene)
		return Restate(ApplicationState_Terminated);

	Scene->ScriptHook();
	Enqueue([this](Timer* Time)
	{
		Scene->Synchronize(Time);
	});
	Enqueue([this](Timer* Time)
	{
		Scene->Simulation(Time);
	});
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