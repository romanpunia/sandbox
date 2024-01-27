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
void Demo::Initialize()
{
	auto NewScene = Content->Load<SceneGraph>(Source);
	if (!NewScene)
		return Stop();
	Scene = *NewScene;
}
void Demo::Dispatch(Timer* Time)
{
	Scene->Dispatch(Time);
}
void Demo::Publish(Timer* Time)
{
	Renderer->Clear(0, 0, 0);
	Renderer->ClearDepth();

	Scene->Publish(Time);
	Scene->Submit();

	Renderer->Submit();
}
void Demo::SetSource(const String& Resource)
{
	Source = Resource;
}
String& Demo::GetSource()
{
	return Source;
}
String Demo::Source;