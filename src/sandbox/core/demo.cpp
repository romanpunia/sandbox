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
		case WindowState::Resize:
			Renderer->ResizeBuffers((unsigned int)X, (unsigned int)Y);
			if (Scene != nullptr)
				Scene->ResizeBuffers();
			break;
		case WindowState::Close:
			Activity->Message.Setup(AlertType::Warning, "Demo", "Do you want to go back to sandbox?");
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
void Demo::ScriptHook(VMGlobal* Global)
{
}
void Demo::Initialize(Application::Desc* Conf)
{
	Scene = Content->Load<SceneGraph>(Source);
	if (!Scene)
		return Stop();

	Scene->ScriptHook();
	Enqueue<Demo, &Demo::Update>();
	Enqueue<Demo, &Demo::Synchronize>();
	Enqueue<Demo, &Demo::Simulation>();
}
void Demo::Publish(Timer* Time)
{
	Renderer->Clear(0, 0, 0);
	Renderer->ClearDepth();

	Scene->Render(Time);
	Scene->Submit();

	Renderer->Submit();
}
void Demo::Update(Timer* Time)
{
	Scene->Update(Time);
}
void Demo::Simulation(Timer* Time)
{
	Scene->Simulation(Time);
}
void Demo::Synchronize(Timer* Time)
{
	Scene->Synchronize(Time);
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